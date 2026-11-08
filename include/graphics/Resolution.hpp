#ifndef GRAPHICS_RESOLUTION_HPP
#define GRAPHICS_RESOLUTION_HPP

#include <string>

namespace newton::graphics
{
	/**
	 * @brief Size of a window or a monitor, in screen coordinates.
	 *
	 * The presets below are common sizes grouped by aspect ratio, but any size is valid.
	 */
	struct Resolution
	{
			int width;
			int height;

			constexpr Resolution(const int w, const int h)
				: width(w)
				, height(h)
			{}

			constexpr bool operator==(const Resolution &other) const
			{
				return width == other.width && height == other.height;
			}

			constexpr bool operator!=(const Resolution &other) const
			{
				return !(*this == other);
			}

			/**
			 * @brief Checks that both dimensions are at most those of the other size.
			 * @param other Size to fit in, e.g. the monitor.
			 * @return True if this size fits, equal sizes included.
			 */
			constexpr bool fitsIn(const Resolution &other) const
			{
				return width <= other.width && height <= other.height;
			}

			/**
			 * @return The size as text, e.g. "1920x1080".
			 */
			inline std::string toString() const
			{
				return std::to_string(width) + "x" + std::to_string(height);
			}
	};

	// 4:3
	inline constexpr Resolution XGA(1024, 768);

	// 16:9
	inline constexpr Resolution qHD(960, 540);
	inline constexpr Resolution HD(1280, 720);
	inline constexpr Resolution HDP(1600, 900);
	inline constexpr Resolution FHD(1920, 1080);
	inline constexpr Resolution QHD(2560, 1440);
	inline constexpr Resolution UHD(3840, 2160);

	// 16:10
	inline constexpr Resolution WXGA(1280, 800);
	inline constexpr Resolution WUXGA(1920, 1200);
	inline constexpr Resolution WQXGA(2560, 1600);

	// 21:9
	inline constexpr Resolution UWFHD(2560, 1080);
	inline constexpr Resolution UWQHD(3440, 1440);

} // namespace newton::graphics

#endif // GRAPHICS_RESOLUTION_HPP
