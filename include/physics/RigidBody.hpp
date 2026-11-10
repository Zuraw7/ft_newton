#ifndef PHYSICS_RIGIDBODY_HPP
#define PHYSICS_RIGIDBODY_HPP

#include <glm/vec3.hpp>

#include "physics/Collider.hpp"

namespace newton::physics
{
	/**
	 * @brief State of a single body simulated by @ref PhysicsWorld.
	 */
	struct RigidBody
	{
			glm::vec3 position{0.0f}; ///< Center of the body in world space.
			glm::vec3 velocity{0.0f}; ///< Linear velocity in units per second.
			glm::vec3 force{0.0f}; ///< Sum of forces applied since the last step, in newtons. Cleared after each step.

			float inverseMass = 1.0f; ///< 1 / mass in kg, 0 for a static body (infinite mass) that never moves.

			Collider collider; ///< Shape used for collisions, already fitted to the object size.
	};

} // namespace newton::physics

#endif // PHYSICS_RIGIDBODY_HPP
