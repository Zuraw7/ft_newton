#ifndef PHYSICS_PHYSICSWORLD_HPP
#define PHYSICS_PHYSICSWORLD_HPP

#include <cstddef>
#include <glm/ext/vector_float3.hpp>
#include <glm/vec3.hpp>
#include <vector>

#include "physics/RigidBody.hpp"

namespace newton::physics
{
	/**
	 * @brief Owns the rigid bodies and moves them with a fixed time step.
	 *
	 * Bodies are referred to by the index returned from @ref add(). Bodies are never removed,
	 * so the indices stay valid.
	 */
	class PhysicsWorld
	{
		public:
			void update(double frameTime);
			void step();

			void setGravity(const glm::vec3 &gravity);
			const glm::vec3 &gravity() const;

			std::size_t add(const RigidBody &body);
			const std::vector<RigidBody> &bodies() const;
			RigidBody &body(std::size_t index);
			const RigidBody &body(std::size_t index) const;

			std::size_t size() const;

		private:
			std::vector<RigidBody> m_bodies;         ///< Bodies in the order they were added.
			glm::vec3 m_gravity{0.0f, -9.81f, 0.0f}; ///< Acceleration applied to every dynamic body, in units/s^2.
			double m_accumulator = 0.0;              ///< Unsimulated time left from previous frames, in seconds.
	};

} // namespace newton::physics

#endif // PHYSICS_PHYSICSWORLD_HPP
