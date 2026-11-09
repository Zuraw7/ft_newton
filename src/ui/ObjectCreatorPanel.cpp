#include "ui/ObjectCreatorPanel.hpp"

#include <imgui.h>

#include <glm/gtc/quaternion.hpp>
#include <glm/trigonometric.hpp>
#include <utility>
#include <vector>

#include "scene/Scene.hpp"
#include "scene/SceneObject.hpp"
#include "ui/Layout.hpp"

namespace
{
	void drawAxisLabels();

} // unnamed namespace

namespace newton::ui
{
	/**
	 * @param meshes Shapes to choose from, must not be empty. Each mesh must outlive the scene objects using it.
	 */
	ObjectCreatorPanel::ObjectCreatorPanel(std::vector<MeshOption> meshes)
		: m_meshes(std::move(meshes))
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
			const char *preview = m_meshes.empty() ? "None" : m_meshes[m_selectedMesh].name;
			if (ImGui::BeginCombo("Mesh", preview)) {
				for (std::size_t i = 0; i < m_meshes.size(); i++) {
					const bool selected = i == m_selectedMesh;
					if (ImGui::Selectable(m_meshes[i].name, selected))
						m_selectedMesh = i;
					if (selected)
						ImGui::SetItemDefaultFocus();
				}
				ImGui::EndCombo();
			}

			drawAxisLabels();
			ImGui::DragFloat3("Position", &m_position.x, 0.1f, 0.0f, 0.0f, "%.2f");
			ImGui::DragFloat3("Rotation", &m_rotationDegrees.x, 1.0f, 0.0f, 0.0f, "%.2f");
			ImGui::DragFloat3("Scale", &m_scale.x, 0.1f, 0.01f, 100.0f, "%.2f", ImGuiSliderFlags_AlwaysClamp);
			ImGui::ColorEdit3("Color", &m_color.x, ImGuiColorEditFlags_PickerHueWheel);

			ImGui::BeginDisabled(m_meshes.empty());
			if (ImGui::Button("Create")) {
				scene::SceneObject object;
				object.mesh = m_meshes[m_selectedMesh].mesh;
				object.transform.position = m_position;
				object.transform.rotation = glm::quat(glm::radians(m_rotationDegrees));
				object.transform.scale = m_scale;
				object.color = m_color;
				scene.add(object);
			}
			ImGui::EndDisabled();
		}

		const float bottom = ImGui::GetWindowPos().y + ImGui::GetWindowHeight();
		ImGui::End();

		return bottom;
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
