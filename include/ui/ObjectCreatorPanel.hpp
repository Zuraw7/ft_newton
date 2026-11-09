#ifndef UI_OBJECTCREATORPANEL_HPP
#define UI_OBJECTCREATORPANEL_HPP

#include <glm/vec3.hpp>

#include <cstddef>
#include <vector>

#include "graphics/Mesh.hpp"
#include "scene/Scene.hpp"

namespace newton::ui
{
	/**
	 * @brief Window with a form creating new objects in a scene.
	 *
	 * It is a class, unlike the other panels, because the form values must last between frames.
	 * They stay after creating an object, so similar objects are quick to add.
	 */
	class ObjectCreatorPanel
	{
		public:
			/**
			 * @brief Shape offered in the combo.
			 */
			struct MeshOption
			{
					const char *name;           ///< Text shown in the combo. Must stay valid, e.g. a string literal.
					const graphics::Mesh *mesh; ///< Shape given to created objects, not owned.
			};

			explicit ObjectCreatorPanel(std::vector<MeshOption> meshes);

			float draw(scene::Scene &scene, float top);

		private:
			std::vector<MeshOption> m_meshes; ///< Shapes offered in the combo.
			std::size_t m_selectedMesh = 0;   ///< Index of the chosen shape in @ref m_meshes.

			glm::vec3 m_position{0.0f, 0.5f, 0.0f}; ///< Center of the object.
			glm::vec3 m_rotationDegrees{0.0f};      ///< Rotation around X, Y and Z in degrees, converted when creating.
			glm::vec3 m_scale{1.0f};                ///< Size multiplier along each axis.
			glm::vec3 m_color{1.0f};                ///< Color with components in the [0, 1] range.
	};

} // namespace newton::ui

#endif // UI_OBJECTCREATORPANEL_HPP
