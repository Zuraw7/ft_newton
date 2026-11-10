#ifndef PHYSICS_COLLIDER_HPP
#define PHYSICS_COLLIDER_HPP

#include <glm/ext/vector_float3.hpp>
#include <glm/fwd.hpp>
#include <glm/vec3.hpp>

#include <variant>

namespace newton::physics
{
	/**
	 * @brief Shape of a body used for collision detection.
	 *
	 * Describes only the shape and its size, relative to the center of the body. The position is taken from
	 * @ref RigidBody::position, so the collider does not change when the body moves.
	 */
	struct Collider
	{
			/**
			 * @brief Sphere centered on the body.
			 */
			struct Sphere
			{
					float radius = 0.5f; ///< Radius in meters.
			};

			/**
			 * @brief Infinite plane passing through the body position. Only the side the normal points to is
			 *        outside, everything behind it counts as penetrating.
			 */
			struct Plane
			{
					glm::vec3 normal{0.0f, 1.0f, 0.0f}; ///< Unit vector pointing out of the plane, in world space.
			};

			/**
			 * @brief Box centered on the body, aligned with the world axes.
			 */
			struct Box
			{
					glm::vec3 halfExtents{0.5f, 0.5f, 0.5f}; ///< Half of the size along each axis, in meters.
			};

			std::variant<Sphere, Plane, Box> shape; ///< The shape with its size. Defaults to a sphere of radius 0.5.
	};

	Collider transformCollider(Collider collider, const glm::quat &rotation, const glm::vec3 &scale);

} // namespace newton::physics

#endif // PHYSICS_COLLIDER_HPP
