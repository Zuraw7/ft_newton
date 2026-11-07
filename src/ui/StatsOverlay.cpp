#include "ui/StatsOverlay.hpp"

#include <imgui.h>

namespace newton::ui
{
	/**
	 * @brief Draws the stats as plain text in the top-left corner, without a window background.
	 *
	 * @param fps Frames per second to show, also converted to the average frame time in ms.
	 */
	void drawStatsOverlay(int fps)
	{
		ImGui::SetNextWindowPos({10, 10}, ImGuiCond_Always);
		ImGui::Begin("Stats", nullptr,
		             ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoInputs |
		                 ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings |
		                 ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav);
		ImGui::Text("%d FPS (%.1f ms)", fps, 1000.0 / fps);
		ImGui::End();
	}

} // namespace newton::ui
