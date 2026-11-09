#ifndef SCENE_SCENE_HPP
#define SCENE_SCENE_HPP

#include <cstddef>
#include <optional>
#include <vector>

#include "scene/SceneObject.hpp"

namespace newton::scene
{
	/**
	 * @brief Contains objects in the world.
	 *
	 * Only stores the objects: it does not draw them and does not own their meshes. Meshes must outlive the scene.
	 */
	class Scene
	{
		public:
			std::optional<std::size_t> add(const SceneObject &object);
			const std::vector<SceneObject> &objects() const;
			std::vector<SceneObject> &objects();
			std::size_t size() const;

		private:
			std::vector<SceneObject> m_objects; ///< Objects in the order they were added.
	};

} // namespace newton::scene

#endif // SCENE_SCENE_HPP
