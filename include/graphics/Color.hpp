#ifndef GRAPHICS_COLOR_HPP
#define GRAPHICS_COLOR_HPP

#include <glm/vec3.hpp>

namespace newton::graphics
{
	/**
	 * @brief Converts an 8-bit RGB color to the [0, 1] range used by shaders and OpenGL.
	 *
	 * @param r Red component, 0-255.
	 * @param g Green component, 0-255.
	 * @param b Blue component, 0-255.
	 * @return Color with components in the [0, 1] range.
	 */
	constexpr glm::vec3 rgb(unsigned char r, unsigned char g, unsigned char b)
	{
		return glm::vec3(r, g, b) / 255.0f;
	}

	/**
	 * @brief Converts a hex color written as 0xRRGGBB to the [0, 1] range.
	 *
	 * Bits above the lowest 24 are ignored, so there is no alpha channel (0xRRGGBBAA gives a wrong color).
	 *
	 * @param hex Color in the 0xRRGGBB format, e.g. 0x40E0D0.
	 * @return Color with components in the [0, 1] range.
	 */
	constexpr glm::vec3 rgb(unsigned int hex)
	{
		return glm::vec3((hex >> 16) & 0xFF, (hex >> 8) & 0xFF, hex & 0xFF) / 255.0f;
	}

} // namespace newton::graphics

#endif // GRAPHICS_COLOR_HPP
