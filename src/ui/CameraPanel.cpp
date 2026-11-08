#include "ui/CameraPanel.hpp"

#include <imgui.h>
#include "controllers/CameraController.hpp"
#include "ui/Layout.hpp"

namespace newton::ui
{
	/**
	 * @brief Draws the "Camera" window with the current FOV, sliders for the controller's settings and a reset button.
	 *
	 * Call between @ref ImGuiLayer::beginFrame() and @ref ImGuiLayer::endFrame(). Changes are applied
	 * immediately. The window stays at the right edge of the screen and cannot be moved.
	 *
	 * @param controller Controller whose settings are shown and changed.
	 * @param top Position to place the window below, in pixels: the value returned by the previous panel, 0 for the
	 *            first one. @ref cMargin is added to it.
	 * @return Bottom edge of this window, in pixels, for placing the next one.
	 */
	float drawCameraPanel(controllers::CameraController &controller, float top)
	{
		const ImVec2 display = ImGui::GetIO().DisplaySize;
		ImGui::SetNextWindowPos({display.x - cMargin, top + cMargin}, ImGuiCond_Always, {1.0f, 0.0f});

		ImGui::SetNextWindowSize({cPanelWidth, 0.0f}, ImGuiCond_Always);
		if (ImGui::Begin("Camera", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove)) {
			controllers::CameraController::Settings settings = controller.settings();

			bool changed = false;
			changed |= ImGui::SliderFloat("Speed", &settings.speed, 1.0f, 100.0f, "%.1f", ImGuiSliderFlags_AlwaysClamp);
			changed |= ImGui::SliderFloat("Fast multiplier", &settings.fastMultiplier, 1.0f, 10.0f, "%.1f",
			                              ImGuiSliderFlags_AlwaysClamp);
			changed |= ImGui::SliderFloat("Sensitivity", &settings.sensitivity, 0.01f, 1.0f, "%.2f",
			                              ImGuiSliderFlags_AlwaysClamp);
			changed |= ImGui::SliderFloat("Zoom sensitivity", &settings.zoomSensitivity, 0.1f, 10.0f, "%.1f",
			                              ImGuiSliderFlags_AlwaysClamp);

			ImGui::Text("FOV: %.1f", static_cast<double>(controller.fov()));

			if (changed)
				controller.setSettings(settings);
			if (ImGui::Button("Reset"))
				controller.setSettings({});
		}

		const float bottom = ImGui::GetWindowPos().y + ImGui::GetWindowHeight();
		ImGui::End();

		return bottom;
	}

} // namespace newton::ui
