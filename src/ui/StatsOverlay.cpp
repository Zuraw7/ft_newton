#include "ui/StatsOverlay.hpp"

#include <imgui.h>

#include "ui/Layout.hpp"

namespace newton::ui
{
	/**
	 * @brief Draws the stats as plain text in the top-left corner, without a window background.
	 *
	 * @param fps Frames per second to show, also converted to the average frame time in ms.
	 * @param objectCount Number of objects in the scene to show.
	 */
	void drawStatsOverlay(int fps, std::size_t objectCount)
	{
		ImGui::SetNextWindowPos({cMargin, cMargin}, ImGuiCond_Always, {0.0f, 0.0f});
		ImGui::Begin("Stats", nullptr,
		             ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoInputs |
		                 ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings |
		                 ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav);
		ImGui::Text("%d FPS (%.1f ms)", fps, 1000.0 / fps);
		ImGui::Text("Objects in scene: %zu", objectCount);
		ImGui::End();
	}

} // namespace newton::ui
