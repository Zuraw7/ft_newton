#ifndef GRAPHICS_RESOLUTION_HPP
#define GRAPHICS_RESOLUTION_HPP

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

		Resolution(const int w, const int h)
			: width(w)
			, height(h)
		{}
};

extern Resolution qHD; ///< 960x540
extern Resolution HD;  ///< 1280x720
extern Resolution HDP; ///< 1600x900
extern Resolution FHD; ///< 1920x1080
extern Resolution QHD; ///< 2560x1440

#endif // GRAPHICS_RESOLUTION_HPP
