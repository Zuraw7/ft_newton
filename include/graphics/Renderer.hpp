#ifndef GRAPHICS_RENDERER_HPP
#define GRAPHICS_RENDERER_HPP

#include "../common/Logger.hpp"

namespace newton::graphics
{
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
			bool m_polygonMode;
	};

} // namespace newton::graphics

#endif // GRAPHICS_RENDERER_HPP
