#include "physics/PhysicsWorld.hpp"

#include <algorithm>
#include <glm/ext/vector_float3.hpp>
#include <glm/geometric.hpp>
#include <glm/vec3.hpp>

#include <cstddef>
#include <vector>

#include "physics/Collision.hpp"
#include "physics/RigidBody.hpp"

namespace
{
	constexpr double FIXED_STEP = 1.0 / 120.0;
	constexpr double MAX_FRAME_TIME = 0.25;
	constexpr float RESTITUTION = 0.5f; // TODO: move to Collider -> objects might have different values

} // unnamed namespace

namespace newton::physics
{
	/**
	 * @brief Advances the simulation by the time of one frame, in fixed steps.
	 *
	 * The frame time is added to an accumulator and @ref step() runs as many times as whole steps fit
	 * (1/120 s each). The rest carries over to the next frame, so the result does not depend on FPS.
	 * The frame time is limited to 0.25 s, so a long frame, e.g. the first one or while the window
	 * is being dragged, does not run hundreds of steps at once.
	 *
	 * @param frameTime Time since the previous frame in seconds.
	 */
	void PhysicsWorld::update(double frameTime)
	{
		if (m_paused)
			return;

		m_accumulator += std::min(frameTime, MAX_FRAME_TIME) * static_cast<double>(m_timeScale);

		while (m_accumulator >= FIXED_STEP) {
			step();
			m_accumulator -= FIXED_STEP;
		}
	}

	/**
	 * @brief Runs one fixed step of the simulation (1/120 s), regardless of the frame time.
	 *
	 * Moves the bodies first, then finds and resolves the collisions, so the positions left after the step
	 * no longer overlap.
	 */
	void PhysicsWorld::step()
	{
		integrate();

		std::vector<Contact> contacts = detectCollisions();

		resolveContacts(contacts);
	}

	/**
	 * @brief Sets the acceleration applied to every dynamic body. Starts as (0, -9.81, 0).
	 *
	 * It is an acceleration, not a force, so all bodies fall the same way regardless of their mass.
	 *
	 * @param gravity Acceleration in units/s^2.
	 */
	void PhysicsWorld::setGravity(const glm::vec3 &gravity)
	{
		m_gravity = gravity;
	}

	const glm::vec3 &PhysicsWorld::gravity() const
	{
		return m_gravity;
	}

	/**
	 * @brief Adds a copy of the body to the world.
	 *
	 * @param body Body to add.
	 * @return Index of the body, for @ref body().
	 */
	std::size_t PhysicsWorld::add(const RigidBody &body)
	{
		m_bodies.push_back(body);
		return m_bodies.size() - 1;
	}

	const std::vector<RigidBody> &PhysicsWorld::bodies() const
	{
		return m_bodies;
	}

	/**
	 * @param index Index returned by @ref add().
	 * @return The body at the index.
	 */
	RigidBody &PhysicsWorld::body(std::size_t index)
	{
		return m_bodies.at(index);
	}

	/**
	 * @copydoc body(std::size_t)
	 */
	const RigidBody &PhysicsWorld::body(std::size_t index) const
	{
		return m_bodies.at(index);
	}

	std::size_t PhysicsWorld::size() const
	{
		return m_bodies.size();
	}

	/**
	 * @brief Stops or resumes @ref update(). Not paused by default.
	 *
	 * While paused, the simulation can still be advanced with @ref step(). The time left in the accumulator
	 * is kept and used after resuming.
	 *
	 * @param paused True to stop the simulation.
	 */
	void PhysicsWorld::setPaused(bool paused)
	{
		m_paused = paused;
	}

	bool PhysicsWorld::isPaused() const
	{
		return m_paused;
	}

	/**
	 * @brief Sets how fast simulated time passes compared to real time. Starts as 1.
	 *
	 * Scales the frame time added to the accumulator. Values above 1 run more steps per frame.
	 *
	 * @param scale Multiplier, e.g. 0.5 for half speed. Must be greater than 0.
	 */
	void PhysicsWorld::setTimeScale(float scale)
	{
		m_timeScale = scale;
	}

	float PhysicsWorld::timeScale() const
	{
		return m_timeScale;
	}

	/**
	 * @brief Adds a force acting on the body during the next step. Static bodies are ignored.
	 *
	 * @param index Index returned by @ref add().
	 * @param force Force in newtons.
	 */
	void PhysicsWorld::applyForce(std::size_t index, const glm::vec3 &force)
	{
		RigidBody &body = m_bodies.at(index);
		if (body.inverseMass == 0.0f)
			return;

		body.force += force;
	}

	/**
	 * @brief Changes the velocity of the body immediately by impulse / mass.
	 *
	 * @param index Index returned by @ref add().
	 * @param impulse Impulse in newton-seconds (kg·m/s).
	 */
	void PhysicsWorld::applyImpulse(std::size_t index, const glm::vec3 &impulse)
	{
		RigidBody &body = m_bodies.at(index);
		body.velocity += impulse * body.inverseMass;
	}

	/**
	 * @brief Applies gravity and forces to the velocities and moves the bodies by one step.
	 *
	 * Uses semi-implicit Euler: velocity is updated first and the position is moved with the new velocity,
	 * which is stable and fits velocity-changing impulses. Static bodies are skipped. Forces are cleared.
	 */
	void PhysicsWorld::integrate()
	{
		const float dt = static_cast<float>(FIXED_STEP);

		for (RigidBody &body : m_bodies) {
			if (body.inverseMass == 0.0f)
				continue;

			const glm::vec3 acceleration = m_gravity + body.force * body.inverseMass;
			body.velocity += acceleration * dt;
			body.position += body.velocity * dt;
			body.force = glm::vec3(0.0f);
		}
	}

	/**
	 * @brief Tests every pair of bodies once, skipping pairs of two static bodies.
	 *
	 * @return Contacts of the overlapping pairs, with body indices set.
	 */
	std::vector<Contact> PhysicsWorld::detectCollisions() const
	{
		std::vector<Contact> contacts;

		const std::size_t count = size();
		for (std::size_t bodyA = 0; bodyA < count; bodyA++) {
			for (std::size_t bodyB = bodyA + 1; bodyB < count; bodyB++) {
				if (m_bodies[bodyA].inverseMass == 0.0f && m_bodies[bodyB].inverseMass == 0.0f)
					continue;

				auto contact = detectCollision(m_bodies[bodyA], m_bodies[bodyB]);
				if (contact) {
					contact->bodyA = bodyA;
					contact->bodyB = bodyB;
					contacts.push_back(*contact);
				}
			}
		}
		return contacts;
	}

	/**
	 * @brief Pushes overlapping bodies apart and makes them bounce.
	 *
	 * For each contact, the bodies are moved apart along the normal by the whole penetration, shared by inverse
	 * mass, so a static body never moves and a lighter one moves more. Then, if they are still approaching,
	 * an impulse along the normal changes their velocities. Restitution decides how much of the approach speed
	 * is kept: 0 stops them, 1 is a perfectly elastic bounce. Momentum is conserved.
	 *
	 * Contacts are resolved one by one, in a single pass.
	 *
	 * @param contacts Contacts from @ref detectCollisions().
	 */
	void PhysicsWorld::resolveContacts(const std::vector<Contact> &contacts)
	{
		for (const Contact &contact : contacts) {
			RigidBody &bodyA = m_bodies[contact.bodyA];
			RigidBody &bodyB = m_bodies[contact.bodyB];

			const float invA = bodyA.inverseMass;
			const float invB = bodyB.inverseMass;

			if (invA == 0.0f && invB == 0.0f)
				continue;

			// Correct position
			const float shareA = invA / (invA + invB);
			const float shareB = invB / (invA + invB);

			glm::vec3 moveA = -contact.normal * (contact.penetration * shareA);
			glm::vec3 moveB = contact.normal * (contact.penetration * shareB);

			bodyA.position += moveA;
			bodyB.position += moveB;

			// Apply impulse
			glm::vec3 relativeVel = bodyB.velocity - bodyA.velocity;
			const float separatingVelocity = glm::dot(contact.normal, relativeVel);
			if (separatingVelocity > 0.0f)
				continue;

			const float j = -(1.0f + RESTITUTION) * separatingVelocity / (invA + invB);
			applyImpulse(contact.bodyA, -j * contact.normal);
			applyImpulse(contact.bodyB, j * contact.normal);
		}
	}

} // namespace newton::physics
