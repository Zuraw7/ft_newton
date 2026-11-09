#include "ui/CameraPanel.hpp"

#include <imgui.h>

#include "controllers/CameraController.hpp"
#include "graphics/Camera.hpp"
#include "ui/Layout.hpp"

namespace
{
	void drawCameraInfo(const newton::graphics::Camera &camera);
	void drawSettingsSliders(newton::controllers::CameraController &controller);

} // unnamed namespace

namespace newton::ui
{
	/**
	 * @brief Draws the "Camera" window with the camera position and FOV, sliders for the controller's settings
	 *        and buttons resetting the settings and the camera.
	 *
	 * Call between @ref ImGuiLayer::beginFrame() and @ref ImGuiLayer::endFrame(). Changes are applied
	 * immediately. The window stays at the right edge of the screen, also when the main window is resized,
	 * and cannot be moved. It starts collapsed.
	 *
	 * @param controller Controller whose settings and camera are shown and changed.
	 * @param top Position to place the window below, in pixels: the value returned by the previous panel, 0 for the
	 *            first one. @ref cMargin is added to it.
	 * @return Bottom edge of this window, in pixels, for placing the next one.
	 */
	float drawCameraPanel(controllers::CameraController &controller, float top)
	{
		const ImVec2 display = ImGui::GetIO().DisplaySize;
		ImGui::SetNextWindowPos({display.x - cMargin, top + cMargin}, ImGuiCond_Always, {1.0f, 0.0f});

		ImGui::SetNextWindowCollapsed(true, ImGuiCond_Once);
		ImGui::SetNextWindowSize({cPanelWidth, 0.0f}, ImGuiCond_Always);
		if (ImGui::Begin("Camera", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove)) {
			drawCameraInfo(controller.camera());
			ImGui::Separator();
			drawSettingsSliders(controller);

			if (ImGui::Button("Reset Settings"))
				controller.setSettings({});
			ImGui::SameLine();
			if (ImGui::Button("Reset Camera"))
				controller.resetCamera();
		}

		const float bottom = ImGui::GetWindowPos().y + ImGui::GetWindowHeight();
		ImGui::End();

		return bottom;
	}

} // namespace newton::ui

namespace
{
	/**
	 * @brief Draws the camera's current position and vertical FOV as text.
	 */
	void drawCameraInfo(const newton::graphics::Camera &camera)
	{
		const glm::vec3 &position = camera.getPosition();

		ImGui::Text("Position: x %.1f, y %.1f, z %.1f", static_cast<double>(position.x),
		            static_cast<double>(position.y), static_cast<double>(position.z));
		ImGui::Text("FOV: %.1f", static_cast<double>(camera.getFOV()));
	}

	/**
	 * @brief Draws the sliders for the controller's settings, applying the values when any of them changes.
	 */
	void drawSettingsSliders(newton::controllers::CameraController &controller)
	{
		newton::controllers::CameraController::Settings settings = controller.settings();

		bool changed = false;
		changed |= ImGui::SliderFloat("Speed", &settings.speed, 1.0f, 100.0f, "%.1f", ImGuiSliderFlags_AlwaysClamp);
		changed |= ImGui::SliderFloat("Fast multiplier", &settings.fastMultiplier, 1.0f, 10.0f, "%.1f",
		                              ImGuiSliderFlags_AlwaysClamp);
		changed |=
			ImGui::SliderFloat("Sensitivity", &settings.sensitivity, 0.01f, 1.0f, "%.2f", ImGuiSliderFlags_AlwaysClamp);
		changed |= ImGui::SliderFloat("Zoom sensitivity", &settings.zoomSensitivity, 0.1f, 10.0f, "%.1f",
		                              ImGuiSliderFlags_AlwaysClamp);

		if (changed)
			controller.setSettings(settings);
	}

} // unnamed namespace
