#include "scene/Transform.hpp"

#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

namespace newton::scene
{
	/**
	 * @brief Builds the model matrix as translation * rotation * scale.
	 *
	 * @return Matrix from the object's local space to world space.
	 */
	glm::mat4 Transform::matrix() const
	{
		glm::mat4 model = glm::translate(glm::mat4(1.0f), position);
		model *= glm::mat4_cast(rotation);
		model = glm::scale(model, scale);
		return model;
	}

} // namespace newton::scene
