#ifndef PHYSICS_COLLISION_HPP
#define PHYSICS_COLLISION_HPP

#include <glm/ext/vector_float3.hpp>

#include <cstddef>
#include <vector>

namespace newton::physics
{
	struct RigidBody;

	/**
	 * @brief Overlap of two bodies found by collision detection.
	 *
	 * One pair of bodies can give several contacts, e.g. one for each corner of a box sunk into a plane.
	 */
	struct Contact
	{
			std::size_t bodyA = 0; ///< Index of the first body in @ref PhysicsWorld.
			std::size_t bodyB = 0; ///< Index of the second body in @ref PhysicsWorld.

			glm::vec3 contactPoint{0.0f}; ///< Point in world space where the bodies touch, inside the overlap.

			glm::vec3 normal{0.0f};   ///< Unit vector pointing from body A to body B.
			float penetration = 0.0f; ///< How deep the bodies overlap along the normal, in meters. Never negative.
	};

	void detectCollision(const RigidBody &a, const RigidBody &b, std::vector<Contact> &contacts);

} // namespace newton::physics

#endif // PHYSICS_COLLISION_HPP
