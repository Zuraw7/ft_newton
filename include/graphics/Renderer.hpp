#ifndef GRAPHICS_RENDERER_HPP
#define GRAPHICS_RENDERER_HPP

#include "../common/Logger.hpp"

namespace newton::graphics
{
	/**
	 * @brief Owns the drawing state of a frame.
	 *
	 * Methods call OpenGL directly, so they can be used only after a successful @ref Context::setup().
	 */
	class Renderer
	{
		public:
			Renderer();
			~Renderer() = default;

			void setBackgroundColor(float red, float green, float blue, float alpha);
			void clear() const;
			void togglePolygonMode();

		private:
			common::Logger &m_logger;
			bool m_polygonMode; ///< True draws wireframes.
	};

} // namespace newton::graphics

#endif // GRAPHICS_RENDERER_HPP
