#ifndef SCENE_TRANSFORM_HPP
#define SCENE_TRANSFORM_HPP

#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/quaternion_float.hpp>
#include <glm/ext/vector_float3.hpp>

namespace newton::scene
{
	/**
	 * @brief Position, rotation and scale of an object. The default values leave a mesh unchanged.
	 *
	 * Primitives are centered at the origin, so @ref position is the center of the object: a unit cube standing
	 * on the ground at y = 0 has position.y = 0.5.
	 */
	struct Transform
	{
			glm::vec3 position{0.0f};                   ///< Center of the object in world space.
			glm::quat rotation{1.0f, 0.0f, 0.0f, 0.0f}; ///< Rotation around the center, order w, x, y, z.
			glm::vec3 scale{1.0f};                      ///< Size multiplier along the object's own axes.

			glm::mat4 matrix() const;
	};

} // namespace newton::scene

#endif // SCENE_TRANSFORM_HPP
