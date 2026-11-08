#ifndef GRAPHICS_GRIDRENDERER_HPP
#define GRAPHICS_GRIDRENDERER_HPP

#include <memory>

#include "graphics/Mesh.hpp"
#include "graphics/Shader.hpp"

namespace newton::graphics
{
	class Camera;

	/**
	 * @brief Draws an infinite-looking reference grid on the XZ plane (y = 0), with its own shader.
	 *
	 * The geometry is one large quad that follows the camera. The lines exist only in the fragment shader,
	 * which computes them from the world position, so they stay fixed in the world and keep a constant width
	 * on screen: thin lines every 1 unit, stronger every 10, the X axis in red and the Z axis in blue.
	 * The grid fades out with the distance from the camera and is visible from both sides.
	 *
	 * Created only through @ref create(), so an existing object always holds a valid shader.
	 * Methods call OpenGL directly, so they can be used only after a successful @ref Context::create().
	 * Non-copyable.
	 */
	class GridRenderer
	{
		public:
			static std::unique_ptr<GridRenderer> create(const std::string &vertex, const std::string &fragment);
			~GridRenderer() = default;

			GridRenderer(const GridRenderer &) = delete;
			GridRenderer &operator=(const GridRenderer &) = delete;

			void visible(bool enabled);
			bool isVisible() const;

			void draw(Camera &camera);

		private:
			explicit GridRenderer(std::unique_ptr<Shader> shader);

			std::unique_ptr<Shader> m_shader; ///< Shader computing the grid lines. Never null.
			Mesh m_quad;                      ///< Plane scaled up by the model matrix, grid lines are drawn on it.

			bool m_visible; ///< False makes draw() do nothing.
	};

} // namespace newton::graphics

#endif // GRAPHICS_GRIDRENDERER_HPP
