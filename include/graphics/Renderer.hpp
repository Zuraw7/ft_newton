#ifndef GRAPHICS_RENDERER_HPP
#define GRAPHICS_RENDERER_HPP

namespace newton::graphics
{
	class Renderer
	{
		public:
			void setBackgroundColor(float red, float green, float blue, float alpha);
			void clear() const;
			void togglePolygonMode();

		private:
			bool m_polygonMode = false;
	};

} // namespace newton::graphics

#endif // GRAPHICS_RENDERER_HPP
