#ifndef PHYSICS_PHYSICSWORLD_HPP
#define PHYSICS_PHYSICSWORLD_HPP

#include <cstddef>
#include <glm/ext/vector_float3.hpp>
#include <glm/vec3.hpp>
#include <vector>

#include "physics/Collision.hpp"
#include "physics/RigidBody.hpp"

namespace newton::physics
{
	/**
	 * @brief Owns the rigid bodies, moves them with a fixed time step and resolves their collisions.
	 *
	 * Bodies are referred to by the index returned from @ref add(). Bodies are never removed,
	 * so the indices stay valid.
	 */
	class PhysicsWorld
	{
		public:
			static constexpr glm::vec3 DEFAULT_GRAVITY{0.0f, -9.81f, 0.0f}; ///< Earth gravity along -Y, in units/s^2.

			void update(double frameTime);
			void step();

			void setGravity(const glm::vec3 &gravity);
			const glm::vec3 &gravity() const;

			std::size_t add(const RigidBody &body);
			const std::vector<RigidBody> &bodies() const;
			RigidBody &body(std::size_t index);
			const RigidBody &body(std::size_t index) const;

			std::size_t size() const;

			void setPaused(bool paused);
			bool isPaused() const;
			void setTimeScale(float scale);
			float timeScale() const;

			void applyForce(std::size_t index, const glm::vec3 &force);
			void applyImpulse(std::size_t index, const glm::vec3 &impulse);

		private:
			void integrate();
			std::vector<Contact> detectCollisions() const;
			void resolveContacts(const std::vector<Contact> &contacts);

			std::vector<RigidBody> m_bodies;       ///< Bodies in the order they were added.
			glm::vec3 m_gravity = DEFAULT_GRAVITY; ///< Acceleration applied to every dynamic body, in units/s^2.
			double m_accumulator = 0.0;            ///< Unsimulated time left from previous frames, in seconds.

			bool m_paused = false;    ///< While true, @ref update() does not advance the simulation.
			float m_timeScale = 1.0f; ///< Multiplier of the simulated time, 1 is real time.
	};

} // namespace newton::physics

#endif // PHYSICS_PHYSICSWORLD_HPP
