#include "graphics/Renderer.hpp"

#include <glad/gl.h>
#include <memory>

#include "common/Logger.hpp"
#include "graphics/Camera.hpp"
#include "graphics/Mesh.hpp"
#include "graphics/Shader.hpp"

namespace
{
	const char *cClassName = "Renderer";

} // unnamed namespace

namespace newton::graphics
{
	common::Logger &Renderer::m_logger = common::Logger::getInstance();

	/**
	 * @brief Creates the renderer together with its shader.
	 *
	 * Requires a current OpenGL context. The shader must provide the uModel, uView, uProjection
	 * and uColor uniforms.
	 *
	 * @param vertex Path to the vertex shader source, relative to the working directory.
	 * @param fragment Path to the fragment shader source, relative to the working directory.
	 * @return The renderer, or nullptr if the shader could not be created.
	 */
	std::unique_ptr<Renderer> Renderer::create(const std::string &vertex, const std::string &fragment)
	{
		auto shader = Shader::create(vertex, fragment);
		if (!shader) {
			m_logger.printf(common::LogLevel::ERROR, "[%s] Failed to create renderer: shader unavailable\n",
			                cClassName);
			return nullptr;
		}
		return std::unique_ptr<Renderer>(new Renderer(std::move(shader)));
	}

	/**
	 * @brief Takes ownership of an already created shader. Called by @ref create().
	 */
	Renderer::Renderer(std::unique_ptr<Shader> shader)
		: m_shader(std::move(shader))
		, m_polygonMode(false)
	{
		m_logger.printf(common::LogLevel::INFO, "[%s] Renderer initialized successfully\n", cClassName);
	}

	/**
	 * @brief Starts a frame: clears the buffers, binds the shader and sets the camera matrices.
	 *
	 * Must be called once per frame, before any @ref draw().
	 *
	 * @param camera Camera the frame is seen from.
	 */
	void Renderer::beginFrame(Camera &camera)
	{
		clear();

		m_shader->bind();

		m_shader->setMat4("uView", camera.getView());
		m_shader->setMat4("uProjection", camera.getProjection());
	}

	/**
	 * @brief Draws a mesh with the given transform and color.
	 *
	 * Valid only after @ref beginFrame() in the same frame, with no other shader bound in between.
	 *
	 * @param mesh Mesh to draw.
	 * @param model Model matrix, from model space to world space.
	 * @param color Color in the [0, 1] range.
	 */
	void Renderer::draw(const Mesh &mesh, const glm::mat4 &model, const glm::vec3 &color)
	{
		m_shader->setMat4("uModel", model);
		m_shader->setVec3("uColor", color);
		mesh.draw();
	}

	/**
	 * @brief Sets the color used by @ref clear(). Components are in the [0, 1] range.
	 */
	void Renderer::setBackgroundColor(const float red, const float green, const float blue, const float alpha)
	{
		glClearColor(red, green, blue, alpha);
	}

	/**
	 * @brief Switches between filled and wireframe drawing.
	 *
	 * Sets the global OpenGL polygon mode immediately, so it also affects anything drawn outside this renderer.
	 */
	void Renderer::togglePolygonMode()
	{
		glPolygonMode(GL_FRONT_AND_BACK, m_polygonMode ? GL_LINE : GL_FILL);
		m_polygonMode = !m_polygonMode;
	}

	/**
	 * @brief Clears the color and depth buffers. Called by @ref beginFrame().
	 */
	void Renderer::clear() const
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

} // namespace newton::graphics
