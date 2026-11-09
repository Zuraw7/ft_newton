#ifndef PHYSICS_RIGIDBODY_HPP
#define PHYSICS_RIGIDBODY_HPP

#include <glm/vec3.hpp>

namespace newton::physics
{
	/**
	 * @brief State of a single body simulated by @ref PhysicsWorld.
	 */
	struct RigidBody
	{
			glm::vec3 position{0.0f}; ///< Center of the body in world space.
			glm::vec3 velocity{0.0f}; ///< Linear velocity in units per second.
			float inverseMass = 1.0f; ///< 1 / mass in kg, 0 for a static body (infinite mass) that never moves.
	};

} // namespace newton::physics

#endif // PHYSICS_RIGIDBODY_HPP
