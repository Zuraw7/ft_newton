#ifndef GRAPHICS_COLOR_HPP
#define GRAPHICS_COLOR_HPP

#include <glm/ext/vector_float3.hpp>
#include <glm/vec3.hpp>

namespace newton::graphics
{
	constexpr glm::vec3 rgb(unsigned char r, unsigned char g, unsigned char b)
	{
		return glm::vec3(r, g, b) / 255.0f;
	}

	constexpr glm::vec3 rgb(unsigned int hex)
	{
		return glm::vec3((hex >> 16) & 0xFF, (hex >> 8) & 0xFF, hex & 0xFF) / 255.0f;
	}

} // namespace newton::graphics

#endif // GRAPHICS_COLOR_HPP
