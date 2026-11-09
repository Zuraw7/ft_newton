#include "ui/PhysicsPanel.hpp"

#include <imgui.h>
#include <glm/vec3.hpp>

#include "physics/PhysicsWorld.hpp"
#include "ui/Layout.hpp"

namespace newton::ui
{
	/**
	 * @brief Draws the "Physics" window controlling the simulation: pause, single step, time scale and gravity.
	 *
	 * Call between @ref ImGuiLayer::beginFrame() and @ref ImGuiLayer::endFrame(). Changes are applied
	 * immediately. The window stays at the right edge of the screen, also when the main window is resized,
	 * and cannot be moved. It starts collapsed. The step button works only while paused, and the reset button
	 * restores gravity and time scale but does not change the pause.
	 *
	 * @param world Physics world whose settings are shown and changed.
	 * @param top Position to place the window below, in pixels. @ref cMargin is added to it.
	 * @return Bottom edge of this window, in pixels, for placing the next one.
	 */
	float drawPhysicsPanel(physics::PhysicsWorld &world, float top)
	{
		const ImVec2 display = ImGui::GetIO().DisplaySize;
		ImGui::SetNextWindowPos({display.x - cMargin, top + cMargin}, ImGuiCond_Always, {1.0f, 0.0f});

		ImGui::SetNextWindowCollapsed(true, ImGuiCond_Once);
		ImGui::SetNextWindowSize({cPanelWidth, 0.0f}, ImGuiCond_Always);

		if (ImGui::Begin("Physics", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove)) {
			bool paused = world.isPaused();
			if (ImGui::Checkbox("Pause", &paused))
				world.setPaused(paused);

			ImGui::SameLine();
			ImGui::BeginDisabled(!paused);
			if (ImGui::Button("Step"))
				world.step();
			ImGui::EndDisabled();

			float scale = world.timeScale();

			if (ImGui::SliderFloat("Time scale", &scale, 0.1f, 4.0f, "%.2f", ImGuiSliderFlags_AlwaysClamp))
				world.setTimeScale(scale);

			glm::vec3 gravity = world.gravity();
			if (ImGui::DragFloat3("Gravity", &gravity.x, 0.1f, 0.0f, 0.0f, "%.2f"))
				world.setGravity(gravity);

			if (ImGui::Button("Reset")) {
				world.setGravity(physics::PhysicsWorld::DEFAULT_GRAVITY);
				world.setTimeScale(1.0f);
			}
		}

		const float bottom = ImGui::GetWindowPos().y + ImGui::GetWindowHeight();
		ImGui::End();

		return bottom;
	}

} // namespace newton::ui
