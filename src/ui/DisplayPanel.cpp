#include "ui/DisplayPanel.hpp"

#include <imgui.h>
#include <string>

#include "graphics/Context.hpp"
#include "graphics/GridRenderer.hpp"
#include "graphics/Resolution.hpp"
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

	/**
	 * @brief Entry of the resolution combo.
	 */
	struct ResolutionPreset
	{
			newton::graphics::Resolution resolution; ///< Value passed to Context::setResolution().
			const char *name;                        ///< Name shown in the combo, before the size.
	};

	/// Window sizes offered in the combo, besides the detected monitor size.
	constexpr ResolutionPreset cResolutionPresets[] = {
		{newton::graphics::XGA, "XGA 4:3"},       {newton::graphics::qHD, "qHD 16:9"},
		{newton::graphics::HD, "HD 16:9"},        {newton::graphics::HDP, "HD+ 16:9"},
		{newton::graphics::FHD, "FHD 16:9"},      {newton::graphics::QHD, "QHD 16:9"},
		{newton::graphics::UHD, "UHD 16:9"},      {newton::graphics::WXGA, "WXGA 16:10"},
		{newton::graphics::WUXGA, "WUXGA 16:10"}, {newton::graphics::WQXGA, "WQXGA 16:10"},
		{newton::graphics::UWFHD, "UWFHD 21:9"},  {newton::graphics::UWQHD, "UWQHD 21:9"},
	};

	/**
	 * @return Text of a resolution combo entry, e.g. "FHD 16:9 (1920x1080)".
	 */
	std::string presetLabel(const ResolutionPreset &preset)
	{
		return std::string(preset.name) + " (" + preset.resolution.toString() + ")";
	}

	void drawVSyncCheckbox(newton::graphics::Context &context);
	void drawGridCheckbox(newton::graphics::GridRenderer &gridRenderer);
	void drawFullscreenCheckbox(newton::graphics::Context &context);
	void drawFrameLimitCombo(newton::graphics::Context &context);
	void drawResolutionCombo(newton::graphics::Context &context);

} // unnamed namespace

namespace newton::ui
{
	/**
	 * @brief Draws the "Display" window with the VSync, grid and fullscreen checkboxes and the FPS limit
	 *        and resolution combos.
	 *
	 * Call between @ref ImGuiLayer::beginFrame() and @ref ImGuiLayer::endFrame(). Changes are applied
	 * immediately. The window stays in the top-right corner, also when the main window is resized,
	 * and cannot be moved. The resolution combo is disabled while fullscreen.
	 *
	 * @param context Context whose settings are shown and changed.
	 * @param gridRenderer Grid whose visibility is shown and changed.
	 */
	void drawDisplayPanel(graphics::Context &context, graphics::GridRenderer &gridRenderer)
	{
		const ImVec2 display = ImGui::GetIO().DisplaySize;
		ImGui::SetNextWindowPos({display.x - cScreenMargin, cScreenMargin}, ImGuiCond_Always, {1.0f, 0.0f});

		if (ImGui::Begin("Display", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove)) {
			drawVSyncCheckbox(context);
			drawGridCheckbox(gridRenderer);
			drawFullscreenCheckbox(context);
			drawFrameLimitCombo(context);
			ImGui::BeginDisabled(context.isFullscreen());
			drawResolutionCombo(context);
			ImGui::EndDisabled();
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
	 * @brief Draws the grid checkbox, showing and changing the grid's visibility.
	 */
	void drawGridCheckbox(newton::graphics::GridRenderer &gridRenderer)
	{
		bool grid = gridRenderer.isVisible();
		if (ImGui::Checkbox("Grid", &grid))
			gridRenderer.visible(grid);
	}

	/**
	 * @brief Draws the fullscreen checkbox, showing and changing the context's fullscreen state.
	 */
	void drawFullscreenCheckbox(newton::graphics::Context &context)
	{
		bool fullscreen = context.isFullscreen();
		if (ImGui::Checkbox("Fullscreen", &fullscreen))
			context.setFullscreen(fullscreen);
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

	/**
	 * @brief Draws the resolution combo: the monitor size (if detected) followed by @ref cResolutionPresets.
	 *
	 * Presets larger than the monitor are skipped, all of them are listed if its size is unknown. When the window
	 * has the monitor size, only the "Monitor" entry is shown as selected, even if a preset has the same value.
	 * Any other size, e.g. after resizing the window by hand or when maximized, is shown as "Custom" with its value.
	 */
	void drawResolutionCombo(newton::graphics::Context &context)
	{
		using namespace newton::graphics;

		const Resolution res = context.resolution();
		const Resolution monitorRes = context.monitorResolution();
		const bool hasSize = monitorRes.width > 0 && monitorRes.height > 0;
		const bool monitorResSelected = hasSize && res == monitorRes;

		const std::string monitorLabel = "Monitor(" + monitorRes.toString() + ")";

		std::string preview = "Custom (" + res.toString() + ")";
		if (monitorResSelected)
			preview = monitorLabel;
		else
			for (const ResolutionPreset &preset : cResolutionPresets)
				if (preset.resolution == res)
					preview = presetLabel(preset);

		if (ImGui::BeginCombo("Resolution", preview.c_str())) {
			if (hasSize) {
				if (ImGui::Selectable(monitorLabel.c_str(), monitorResSelected))
					context.setResolution(monitorRes);
				if (monitorResSelected)
					ImGui::SetItemDefaultFocus();
			}

			for (const ResolutionPreset &preset : cResolutionPresets) {
				if (hasSize && !preset.resolution.fitsIn(monitorRes))
					continue;

				const bool selected = !monitorResSelected && preset.resolution == res;
				if (ImGui::Selectable(presetLabel(preset).c_str(), selected))
					context.setResolution(preset.resolution);
				if (selected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}
	}

} // unnamed namespace
