#ifndef GRAPHICS_RESOLUTION_HPP
#define GRAPHICS_RESOLUTION_HPP

namespace newton::graphics
{
	/**
	 * @brief Window size preset.
	 *
	 * All presets are 16:9, matching @ref aspect. The same ratio is forced on the window
	 * and used by the camera projection.
	 */
	struct Resolution
	{
			int width;
			int height;
			constexpr static float aspect = 16.0f / 9.0f;

			constexpr Resolution(const int w, const int h)
				: width(w)
				, height(h)
			{}
	};

	inline constexpr Resolution qHD(960, 540);
	inline constexpr Resolution HD(1280, 720);
	inline constexpr Resolution HDP(1600, 900);
	inline constexpr Resolution FHD(1920, 1080);
	inline constexpr Resolution QHD(2560, 1440);

} // namespace newton::graphics

#endif // GRAPHICS_RESOLUTION_HPP
