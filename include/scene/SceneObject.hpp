#ifndef SCENE_SCENEOBJECT_HPP
#define SCENE_SCENEOBJECT_HPP

#include <glm/vec3.hpp>

#include "scene/Transform.hpp"

namespace newton::graphics
{
	class Mesh;

} // namespace newton::graphics

namespace newton::scene
{
	/**
	 * @brief Single object in the world: what it looks like and where it is.
	 */
	struct SceneObject
	{
			const graphics::Mesh *mesh = nullptr; ///< Shape to draw, not owned. Many objects can share one mesh.
			Transform transform;                  ///< Position, rotation and scale in the world.
			glm::vec3 color{1.0f};                ///< Color with components in the [0, 1] range.
	};

} // namespace newton::scene

#endif // SCENE_SCENEOBJECT_HPP
