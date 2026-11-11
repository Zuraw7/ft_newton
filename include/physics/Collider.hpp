#ifndef PHYSICS_COLLIDER_HPP
#define PHYSICS_COLLIDER_HPP

#include <glm/ext/vector_float3.hpp>
#include <glm/vec3.hpp>

#include <variant>

namespace newton::physics
{
	/**
	 * @brief Shape of a body used for collision detection.
	 *
	 * Describes only the shape and its size, in the local space of the body. The position and rotation are taken
	 * from @ref RigidBody::position and @ref RigidBody::rotation. The collider follows the body, but its data
	 * stays the same when the body moves or turns.
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
			 *
			 * The normal is the local +Y axis, so the plane has no fields: it is turned by the body rotation,
			 * e.g. 180° around X for a ceiling.
			 */
			struct Plane
			{};

			/**
			 * @brief Box centered on the body, with its edges along the local axes.
			 */
			struct Box
			{
					glm::vec3 halfExtents{0.5f, 0.5f, 0.5f}; ///< Half of the size along each axis, in meters.
			};

			std::variant<Sphere, Plane, Box> shape; ///< The shape with its size. Defaults to a sphere of radius 0.5.
	};

	Collider scaleCollider(Collider collider, const glm::vec3 &scale);

} // namespace newton::physics

#endif // PHYSICS_COLLIDER_HPP
