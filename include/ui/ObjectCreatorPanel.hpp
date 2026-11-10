#ifndef UI_OBJECTCREATORPANEL_HPP
#define UI_OBJECTCREATORPANEL_HPP

#include <glm/ext/vector_float3.hpp>
#include <glm/vec3.hpp>

#include <cstddef>
#include <vector>

#include "graphics/Mesh.hpp"
#include "physics/Collider.hpp"
#include "physics/PhysicsWorld.hpp"
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
					physics::Collider collider; ///< Collider of the mesh at scale 1, fitted to each created object.
			};

			explicit ObjectCreatorPanel(std::vector<MeshOption> meshes);

			float draw(scene::Scene &scene, physics::PhysicsWorld &world, float top);

		private:
			void drawMeshCombo();
			void drawTransformFields();
			void drawAppearanceFields();
			void drawRigidBodyFields();
			void createObject(scene::Scene &scene, physics::PhysicsWorld &world) const;

			// Shape
			std::vector<MeshOption> m_meshes; ///< Shapes offered in the combo.
			std::size_t m_selectedMesh = 0;   ///< Index of the chosen shape in @ref m_meshes.

			// Transform
			glm::vec3 m_position{0.0f, 0.5f, 0.0f}; ///< Center of the object.
			glm::vec3 m_rotationDegrees{0.0f};      ///< Rotation around X, Y and Z in degrees, converted when creating.
			glm::vec3 m_scale{1.0f};                ///< Size multiplier along each axis.

			// Appearance
			glm::vec3 m_color{1.0f}; ///< Color with components in the [0, 1] range.

			// Rigid body
			bool m_physics = true;      ///< Whether the created object gets a rigid body.
			bool m_static = false;      ///< Whether the body is static (infinite mass), e.g. the ground.
			float m_mass = 1.0f;        ///< Mass in kg, ignored for a static body.
			glm::vec3 m_velocity{0.0f}; ///< Starting velocity in units per second, ignored for a static body.
	};

} // namespace newton::ui

#endif // UI_OBJECTCREATORPANEL_HPP
