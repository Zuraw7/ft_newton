#include "ui/DisplayPanel.hpp"

#include <imgui.h>
#include <string>

#include "graphics/Context.hpp"
#include "ui/Layout.hpp"

namespace
{
	/**
	 * @brief Entry of the FPS limit combo.
	 */
	struct FpsPreset
	{
			int limit;         ///< Value passed to Context::setFrameLimit(), 0 means unlimited.
			const char *label; ///< Text shown in the combo.
	};

	/// FPS limits offered in the combo, besides the detected monitor refresh rate.
	constexpr FpsPreset cFpsPresets[] = {{30, "30"},   {60, "60"},   {120, "120"}, {144, "144"},    {165, "165"},
	                                     {180, "180"}, {240, "240"}, {500, "500"}, {0, "Unlimited"}};

	void drawVSyncCheckbox(newton::graphics::Context &context);
	void drawFrameLimitCombo(newton::graphics::Context &context);

} // unnamed namespace

namespace newton::ui
{
	/**
	 * @brief Draws the "Display" window with the VSync checkbox and the FPS limit combo.
	 *
	 * Call between @ref ImGuiLayer::beginFrame() and @ref ImGuiLayer::endFrame(). Changes are applied
	 * to the context immediately. The window starts in the top-right corner and can be moved freely,
	 * it does not follow the corner when the main window is resized.
	 *
	 * @param context Context whose settings are shown and changed.
	 */
	void drawDisplayPanel(graphics::Context &context)
	{
		const ImVec2 display = ImGui::GetIO().DisplaySize;
		ImGui::SetNextWindowPos({display.x - cScreenMargin, cScreenMargin}, ImGuiCond_FirstUseEver, {1.0f, 0.0f});

		if (ImGui::Begin("Display", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
			drawVSyncCheckbox(context);
			drawFrameLimitCombo(context);
		}

		ImGui::End();
	}

} // namespace newton::ui

namespace
{
	/**
	 * @brief Draws the VSync checkbox, showing and changing the context's VSync state.
	 */
	void drawVSyncCheckbox(newton::graphics::Context &context)
	{
		bool vsync = context.isVSync();
		if (ImGui::Checkbox("VSync", &vsync))
			context.setVSync(vsync);
	}

	/**
	 * @brief Draws the FPS limit combo: the monitor refresh rate (if detected) followed by @ref cFpsPresets.
	 *
	 * When the limit equals the monitor rate, only the "Monitor" entry is shown as selected, even if
	 * a preset has the same value. A limit matching no entry is shown as "Custom".
	 */
	void drawFrameLimitCombo(newton::graphics::Context &context)
	{
		const int limit = context.frameLimit();
		const int monitorHz = context.monitorRefreshRate();
		const bool hasMonitorHz = monitorHz > 0;
		const bool monitorSelected = hasMonitorHz && limit == monitorHz;

		const std::string monitorLabel = "Monitor(" + std::to_string(monitorHz) + " Hz)";

		const char *preview = "Custom";
		if (monitorSelected)
			preview = monitorLabel.c_str();
		else
			for (const FpsPreset &preset : cFpsPresets)
				if (preset.limit == limit)
					preview = preset.label;

		if (ImGui::BeginCombo("FPS limit", preview)) {
			if (hasMonitorHz) {
				if (ImGui::Selectable(monitorLabel.c_str(), monitorSelected))
					context.setFrameLimit(monitorHz);
				if (monitorSelected)
					ImGui::SetItemDefaultFocus();
			}

			for (const FpsPreset &preset : cFpsPresets) {
				const bool selected = !monitorSelected && preset.limit == limit;
				if (ImGui::Selectable(preset.label, selected))
					context.setFrameLimit(preset.limit);
				if (selected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}
	}

} // unnamed namespace
