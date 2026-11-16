#include "newton/ui/ObjectCreatorPanel.hpp"

#include <imgui.h>

#include <glm/ext/vector_float3.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/trigonometric.hpp>
#include <utility>
#include <vector>

#include "newton/scene/Scene.hpp"
#include "newton/scene/SceneObject.hpp"
#include "newton/ui/Layout.hpp"

namespace
{
	void drawAxisLabels();

} // unnamed namespace

namespace newton::ui
{
	/**
	 * @param shapes Shapes to choose from, must not be empty. Each mesh must outlive the scene objects using it.
	 */
	ObjectCreatorPanel::ObjectCreatorPanel(std::vector<ShapeOption> shapes)
		: m_shapes(std::move(shapes))
	{}

	/**
	 * @brief Draws the "Object" window with the form and the button creating an object from its values.
	 *
	 * Call between @ref ImGuiLayer::beginFrame() and @ref ImGuiLayer::endFrame(). The window stays at the right
	 * edge of the screen, also when the main window is resized, and cannot be moved.
	 *
	 * @param scene Scene the created objects are added to.
	 * @param top Position to place the window below, in pixels. @ref cMargin is added to it.
	 * @return Bottom edge of this window, in pixels, for placing the next one.
	 */
	float ObjectCreatorPanel::draw(scene::Scene &scene, float top)
	{
		const ImVec2 display = ImGui::GetIO().DisplaySize;
		ImGui::SetNextWindowPos({display.x - cMargin, top + cMargin}, ImGuiCond_Always, {1.0f, 0.0f});

		ImGui::SetNextWindowSize({cPanelWidth, 0.0f}, ImGuiCond_Always);
		if (ImGui::Begin("Object", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove)) {
			drawShapeCombo();
			drawTransformFields();
			drawAppearanceFields();

			ImGui::BeginDisabled(m_shapes.empty());
			if (ImGui::Button("Create"))
				createObject(scene);
			ImGui::EndDisabled();
		}

		const float bottom = ImGui::GetWindowPos().y + ImGui::GetWindowHeight();
		ImGui::End();

		return bottom;
	}

	void ObjectCreatorPanel::drawShapeCombo()
	{
		const char *preview = m_shapes.empty() ? "None" : m_shapes[m_selectedShape].name;
		if (ImGui::BeginCombo("Shape", preview)) {
			for (std::size_t i = 0; i < m_shapes.size(); i++) {
				const bool selected = i == m_selectedShape;
				if (ImGui::Selectable(m_shapes[i].name, selected))
					m_selectedShape = i;
				if (selected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}
	}

	void ObjectCreatorPanel::drawTransformFields()
	{
		ImGui::SeparatorText("Transform");
		drawAxisLabels();
		ImGui::DragFloat3("Position", &m_position.x, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::DragFloat3("Rotation", &m_rotationDegrees.x, 1.0f, 0.0f, 0.0f, "%.2f");

		ImGui::DragFloat3("Scale", &m_scale.x, 0.1f, 0.01f, 100.0f, "%.2f", ImGuiSliderFlags_AlwaysClamp);
	}

	void ObjectCreatorPanel::drawAppearanceFields()
	{
		ImGui::SeparatorText("Appearance");
		ImGui::ColorEdit3("Color", &m_color.x, ImGuiColorEditFlags_PickerHueWheel);
	}

	/**
	 * @brief Creates an object from the form values and adds it to the scene.
	 *
	 * @param scene Scene the object is added to.
	 */
	void ObjectCreatorPanel::createObject(scene::Scene &scene) const
	{
		scene::SceneObject object;
		object.mesh = m_shapes[m_selectedShape].mesh;
		object.transform.position = m_position;
		object.transform.rotation = glm::quat(glm::radians(m_rotationDegrees));
		object.transform.scale = m_scale;
		object.color = m_color;

		scene.add(object);
	}

} // namespace newton::ui

namespace
{
	/**
	 * @brief Draws the X, Y and Z labels centered above the fields of the next `ImGui::DragFloat3`.
	 *
	 * The columns are calculated the same way as in `DragFloat3`, so both must use the current item width.
	 */
	void drawAxisLabels()
	{
		const float spacing = ImGui::GetStyle().ItemInnerSpacing.x;
		const float width = (ImGui::CalcItemWidth() - 2.0f * spacing) / 3.0f;
		const float startX = ImGui::GetCursorPosX();

		const char *const labels[] = {"X", "Y", "Z"};
		for (int i = 0; i < 3; ++i) {
			const float column = static_cast<float>(i) * (width + spacing);
			const float center = (width - ImGui::CalcTextSize(labels[i]).x) / 2.0f;

			if (i > 0)
				ImGui::SameLine();
			ImGui::SetCursorPosX(startX + column + center);
			ImGui::TextUnformatted(labels[i]);
		}
	}

} // unnamed namespace
