#ifndef PHYSICS_COLLISION_HPP
#define PHYSICS_COLLISION_HPP

#include <glm/ext/vector_float3.hpp>

#include <cstddef>
#include <optional>

namespace newton::physics
{
	struct RigidBody;

	/**
	 * @brief Overlap of two bodies found by collision detection.
	 */
	struct Contact
	{
			std::size_t bodyA = 0;    ///< Index of the first body in @ref PhysicsWorld.
			std::size_t bodyB = 0;    ///< Index of the second body in @ref PhysicsWorld.
			glm::vec3 normal{0.0f};   ///< Unit vector pointing from body A to body B.
			float penetration = 0.0f; ///< How deep the bodies overlap along the normal, in meters. Never negative.
	};

	std::optional<Contact> detectCollision(const RigidBody &a, const RigidBody &b);

} // namespace newton::physics

#endif // PHYSICS_COLLISION_HPP
