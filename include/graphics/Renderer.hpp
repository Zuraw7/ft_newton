#ifndef GRAPHICS_RENDERER_HPP
#define GRAPHICS_RENDERER_HPP

#include <memory>

#include "common/Logger.hpp"
#include "graphics/Camera.hpp"
#include "graphics/Mesh.hpp"
#include "graphics/Shader.hpp"

namespace newton::graphics
{
	/**
	 * @brief Draws meshes with its own shader, using the camera matrices of the current frame.
	 *
	 * Created only through @ref create(), so an existing object always holds a valid shader.
	 * Every frame starts with @ref beginFrame(), followed by any number of @ref draw() calls.
	 * Methods call OpenGL directly, so they can be used only after a successful @ref Context::create().
	 * Non-copyable.
	 */
	class Renderer
	{
		public:
			static std::unique_ptr<Renderer> create(const std::string &vertex, const std::string &fragment);
			~Renderer() = default;

			Renderer(const Renderer &) = delete;
			Renderer &operator=(const Renderer &) = delete;

			void beginFrame(Camera &camera);
			void draw(const Mesh &mesh, const glm::mat4 &model, const glm::vec3 &color);

			void setBackgroundColor(float red, float green, float blue, float alpha);
			void togglePolygonMode();

		private:
			explicit Renderer(std::unique_ptr<Shader> shader);

			void clear() const;

			static common::Logger &m_logger; ///< Shared logger instance.

			std::unique_ptr<Shader> m_shader; ///< Shader used by every draw. Never null.

			bool m_polygonMode; ///< True draws wireframes.
	};

} // namespace newton::graphics

#endif // GRAPHICS_RENDERER_HPP
