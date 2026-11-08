#include "scene/Scene.hpp"

#include <cstddef>
#include <optional>
#include <vector>

#include "common/Logger.hpp"
#include "scene/SceneObject.hpp"

namespace
{
	const char *cClassName = "Scene";

} // unnamed namespace

namespace newton::scene
{
	/**
	 * @brief Adds a copy of the object to the scene.
	 *
	 * Keep the returned index rather than a pointer to the object, because adding may move the objects in memory.
	 *
	 * @param object Object to add, must have a mesh. Without one it is not added and a warning is logged.
	 * @return Index of the object in @ref objects(), or `std::nullopt` if it was not added.
	 */
	std::optional<std::size_t> Scene::add(const SceneObject &object)
	{
		if (object.mesh == nullptr) {
			common::Logger::getInstance().printf(common::LogLevel::WARN, "[%s] Object without a mesh was not added\n",
			                                     cClassName);
			return std::nullopt;
		}

		m_objects.emplace_back(object);
		return m_objects.size() - 1;
	}

	const std::vector<SceneObject> &Scene::objects() const
	{
		return m_objects;
	}

	std::size_t Scene::size() const
	{
		return m_objects.size();
	}

} // namespace newton::scene
