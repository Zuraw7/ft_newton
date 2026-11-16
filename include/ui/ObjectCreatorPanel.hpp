#ifndef UI_OBJECTCREATORPANEL_HPP
#define UI_OBJECTCREATORPANEL_HPP

#include <glm/ext/vector_float3.hpp>
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
			struct ShapeOption
			{
					const char *name;           ///< Text shown in the combo. Must stay valid, e.g. a string literal.
					const graphics::Mesh *mesh; ///< Mesh given to created objects, not owned.
			};

			explicit ObjectCreatorPanel(std::vector<ShapeOption> shapes);

			float draw(scene::Scene &scene, float top);

		private:
			void drawShapeCombo();
			void drawTransformFields();
			void drawAppearanceFields();
			void createObject(scene::Scene &scene) const;

			// Shape
			std::vector<ShapeOption> m_shapes; ///< Shapes offered in the combo.
			std::size_t m_selectedShape = 0;   ///< Index of the chosen shape in @ref m_shapes.

			// Transform
			glm::vec3 m_position{0.0f, 0.5f, 0.0f}; ///< Center of the object.
			glm::vec3 m_rotationDegrees{0.0f};      ///< Rotation around X, Y and Z in degrees, converted when creating.
			glm::vec3 m_scale{1.0f};                ///< Size multiplier along each axis.

			// Appearance
			glm::vec3 m_color{1.0f}; ///< Color with components in the [0, 1] range.
	};

} // namespace newton::ui

#endif // UI_OBJECTCREATORPANEL_HPP
