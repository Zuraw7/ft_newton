#include "physics/PhysicsWorld.hpp"

#include <algorithm>
#include <glm/ext/vector_float3.hpp>
#include <glm/vec3.hpp>

#include <cstddef>
#include <vector>

#include "physics/RigidBody.hpp"

namespace
{
	constexpr double FIXED_STEP = 1.0 / 120.0;
	constexpr double MAX_FRAME_TIME = 0.25;

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
	 * Uses semi-implicit Euler: velocity is updated first and the position is moved with the new velocity,
	 * which is stable and fits velocity-changing impulses. Static bodies are skipped.
	 */
	void PhysicsWorld::step()
	{
		// Semi-implicit Euler
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

} // namespace newton::physics
