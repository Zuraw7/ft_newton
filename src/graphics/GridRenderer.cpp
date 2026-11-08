#include "graphics/GridRenderer.hpp"

#include <glad/gl.h>
#include <glm/ext/matrix_transform.hpp>
#include <glm/vec3.hpp>

#include <memory>
#include <string>
#include <utility>

#include "common/Logger.hpp"
#include "geometry/Primitives.hpp"
#include "graphics/Camera.hpp"
#include "graphics/Mesh.hpp"
#include "graphics/Shader.hpp"

namespace
{
	const char *cClassName = "GridRenderer";
	newton::common::Logger &logger = newton::common::Logger::getInstance();

	constexpr float GRID_SIZE = 500.0f;       ///< Side of the quad following the camera, in world units.
	constexpr float FADE_START = 30.0f;       ///< Distance from the camera where the grid starts to fade out.
	constexpr float FADE_END = GRID_SIZE / 2; ///< Fully faded at the quad edge, so the edge is never visible.

} // unnamed namespace

namespace newton::graphics
{
	/**
	 * @brief Creates the grid renderer together with its shader.
	 *
	 * Requires a current OpenGL context. The shader must provide the uModel, uView, uProjection, uCameraPos,
	 * uFadeStart and uFadeEnd uniforms.
	 *
	 * @param vertex Path to the vertex shader source, relative to the working directory.
	 * @param fragment Path to the fragment shader source, relative to the working directory.
	 * @return The grid renderer, or nullptr if the shader could not be created.
	 */
	std::unique_ptr<GridRenderer> GridRenderer::create(const std::string &vertex, const std::string &fragment)
	{
		auto shader = Shader::create(vertex, fragment);
		if (!shader) {
			logger.printf(common::LogLevel::ERROR, "[%s] Failed to create renderer: shader unavailable\n", cClassName);
			return nullptr;
		}
		return std::unique_ptr<GridRenderer>(new GridRenderer(std::move(shader)));
	}

	/**
	 * @brief Takes ownership of an already created shader and creates the quad. Called by @ref create().
	 */
	GridRenderer::GridRenderer(std::unique_ptr<Shader> shader)
		: m_shader(std::move(shader))
		, m_quad(geometry::makePlane())
		, m_visible(true)
	{
		logger.printf(common::LogLevel::INFO, "[%s] Renderer initialized successfully\n", cClassName);
	}

	/**
	 * @brief Shows or hides the grid. A hidden grid makes @ref draw() do nothing. Visible by default.
	 *
	 * @param enabled True shows the grid.
	 */
	void GridRenderer::visible(bool enabled)
	{
		m_visible = enabled;
	}

	/**
	 * @return True if @ref draw() draws the grid.
	 */
	bool GridRenderer::isVisible() const
	{
		return m_visible;
	}

	/**
	 * @brief Draws the grid, if visible.
	 *
	 * The grid is transparent and blends with what is already on screen, so call it after opaque objects,
	 * once per frame. It binds its own shader.
	 *
	 * Enables blending and disables depth writes and face culling while drawing. Afterwards blending
	 * is off, depth writes are on and culling is on, whatever they were before.
	 *
	 * @param camera Camera the frame is seen from. The quad follows its position.
	 */
	void GridRenderer::draw(Camera &camera)
	{
		if (!m_visible)
			return;

		const glm::vec3 &cameraPos = camera.getPosition();

		glm::mat4 model(1.0f);
		model = glm::translate(model, glm::vec3(cameraPos.x, 0.0f, cameraPos.z));
		model = glm::scale(model, glm::vec3(GRID_SIZE, 1.0f, GRID_SIZE));

		m_shader->bind();
		m_shader->setMat4("uModel", model);
		m_shader->setMat4("uView", camera.getView());
		m_shader->setMat4("uProjection", camera.getProjection());
		m_shader->setVec3("uCameraPos", cameraPos);
		m_shader->setFloat("uFadeStart", FADE_START);
		m_shader->setFloat("uFadeEnd", FADE_END);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDepthMask(GL_FALSE);
		glDisable(GL_CULL_FACE);

		m_quad.draw();

		glDepthMask(GL_TRUE);
		glDisable(GL_BLEND);
		glEnable(GL_CULL_FACE);
	}

} // namespace newton::graphics
