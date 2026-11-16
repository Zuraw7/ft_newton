#include "graphics/ColliderRenderer.hpp"

#include <glad/gl.h>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/quaternion_float.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/quaternion.hpp>

#include <cmath>
#include <memory>
#include <string>
#include <vector>

#include "common/Logger.hpp"
#include "graphics/Camera.hpp"

namespace
{
	const char *cClassName = "ColliderRenderer";

	constexpr int SPHERE_SEGMENTS = 32; ///< Segments of each circle of the sphere wireframe
	constexpr int PLANE_DIVISIONS = 10; ///< Cells along each side of the plane grid
	constexpr float PLANE_SIZE = 5.0f; ///< Half of the side of the plane grid, in meters (the plane itself is infinite)

	constexpr glm::vec3 BOX_COLOR{1.0f, 0.85f, 0.0f};   ///< Yellow.
	constexpr glm::vec3 SPHERE_COLOR{0.2f, 1.0f, 0.2f}; ///< Green.
	constexpr glm::vec3 PLANE_COLOR{1.0f, 0.2f, 0.2f};  ///< Red.

	newton::common::Logger &logger = newton::common::Logger::getInstance();

	std::vector<glm::vec3> boxLines();
	std::vector<glm::vec3> sphereLines();
	std::vector<glm::vec3> planeLines();

} // unnamed namespace

namespace newton::graphics
{
	/**
	 * @brief Creates the collider renderer together with its shader and the line meshes of the shapes.
	 *
	 * Requires a current OpenGL context. The shader must provide the uModel, uView, uProjection and uColor uniforms.
	 *
	 * @param vertex Path to the vertex shader source, relative to the working directory.
	 * @param fragment Path to the fragment shader source, relative to the working directory.
	 * @return The collider renderer, or nullptr if the shader could not be created.
	 */
	std::unique_ptr<ColliderRenderer> ColliderRenderer::create(const std::string &vertex, const std::string &fragment)
	{
		auto shader = Shader::create(vertex, fragment);
		if (!shader) {
			logger.printf(common::LogLevel::ERROR, "[%s] Failed to create renderer: shader unavailable\n", cClassName);
			return nullptr;
		}
		return std::unique_ptr<ColliderRenderer>(new ColliderRenderer(std::move(shader)));
	}

	/**
	 * @brief Takes ownership of an already created shader and builds the line meshes. Called by @ref create().
	 */
	ColliderRenderer::ColliderRenderer(std::unique_ptr<Shader> shader)
		: m_shader(std::move(shader))
		, m_visible(false)
		, m_box(boxLines())
		, m_sphere(sphereLines())
		, m_plane(planeLines())
	{
		logger.printf(common::LogLevel::INFO, "[%s] Renderer initialized successfully\n", cClassName);
	}

	/**
	 * @brief Shows or hides the colliders. Hidden by default.
	 *
	 * @param enabled True shows the colliders.
	 */
	void ColliderRenderer::visible(bool enabled)
	{
		m_visible = enabled;
	}

	/**
	 * @return True if the draw methods draw anything.
	 */
	bool ColliderRenderer::isVisible() const
	{
		return m_visible;
	}

	/**
	 * @brief Binds the shader and sets the camera matrices for the draw calls of this frame.
	 *
	 * Call once per frame, after the scene, before the draw methods. No other shader may be bound until the last
	 * draw call, because the draw methods only set the per-shape uniforms.
	 *
	 * @param camera Camera the frame is seen from.
	 */
	void ColliderRenderer::beginFrame(Camera &camera)
	{
		if (!m_visible)
			return;

		m_shader->bind();
		m_shader->setMat4("uView", camera.getView());
		m_shader->setMat4("uProjection", camera.getProjection());
	}

	/**
	 * @brief Draws the edges of a box.
	 *
	 * @pre @ref beginFrame() was called in this frame.
	 *
	 * @param position Center of the box.
	 * @param rotation Rotation of the body.
	 * @param halfExtents Half of the size along each local axis, as in the box collider.
	 */
	void ColliderRenderer::drawBox(const glm::vec3 &position, const glm::quat &rotation, const glm::vec3 &halfExtents)
	{
		if (!m_visible)
			return;

		glm::mat4 model{1.0f};
		model = glm::translate(model, position);
		model = model * glm::mat4_cast(rotation);
		model = glm::scale(model, halfExtents);

		m_shader->setMat4("uModel", model);
		m_shader->setVec3("uColor", BOX_COLOR);

		m_box.draw();
	}

	/**
	 * @brief Draws a sphere as three circles, one around each local axis.
	 *
	 * @pre @ref beginFrame() was called in this frame.
	 *
	 * @param position Center of the sphere.
	 * @param rotation Rotation of the body. It does not change the shape, it only turns the circles so the
	 *        rotation of the sphere can be seen.
	 * @param radius Radius, as in the sphere collider.
	 */
	void ColliderRenderer::drawSphere(const glm::vec3 &position, const glm::quat &rotation, float radius)
	{
		if (!m_visible)
			return;

		glm::mat4 model{1.0f};
		model = glm::translate(model, position);
		model = model * glm::mat4_cast(rotation);
		model = glm::scale(model, glm::vec3(radius));

		m_shader->setMat4("uModel", model);
		m_shader->setVec3("uColor", SPHERE_COLOR);

		m_sphere.draw();
	}

	/**
	 * @brief Draws a part of a plane as a square grid, with an arrow showing its normal.
	 *
	 * The plane collider is infinite, so the grid has a fixed size and only marks where the plane is.
	 * The arrow points to the side bodies are pushed to.
	 *
	 * @pre @ref beginFrame() was called in this frame.
	 *
	 * @param position Any point on the plane, the grid is centered on it.
	 * @param rotation Rotation of the body, turning the local +Y axis into the plane normal.
	 */
	void ColliderRenderer::drawPlane(const glm::vec3 &position, const glm::quat &rotation)
	{
		if (!m_visible)
			return;

		glm::mat4 model{1.0f};
		model = glm::translate(model, position);
		model = model * glm::mat4_cast(rotation);
		model = glm::scale(model, glm::vec3(PLANE_SIZE));

		m_shader->setMat4("uModel", model);
		m_shader->setVec3("uColor", PLANE_COLOR);

		m_plane.draw();
	}

} // namespace newton::graphics

namespace newton::graphics
{
	/**
	 * @brief Uploads the vertices to the GPU and sets up the position attribute.
	 *
	 * @param vertices Pairs of segment ends.
	 */
	ColliderRenderer::LineMesh::LineMesh(const std::vector<glm::vec3> &vertices)
		: VAO()
		, VBO(vertices)
		, vertexCount(vertices.size())
	{
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), nullptr);
		VAO.unbind();
	}

	/**
	 * @brief Draws all segments with the currently bound shader.
	 */
	void ColliderRenderer::LineMesh::draw() const
	{
		VAO.bind();
		glDrawArrays(GL_LINES, 0, static_cast<int>(vertexCount));
		VAO.unbind();
	}

} // namespace newton::graphics

namespace
{
	/**
	 * @brief Builds the 12 edges of a box from -1 to 1 on each axis, as pairs of vertices.
	 *
	 * Scaled by the half extents, it matches a box collider.
	 */
	std::vector<glm::vec3> boxLines()
	{
		glm::vec3 corners[] = {
			{-1.0f, -1.0f, -1.0f}, {1.0f, -1.0f, -1.0f}, {1.0f, -1.0f, 1.0f}, {-1.0f, -1.0f, 1.0f},
			{-1.0f, 1.0f, -1.0f},  {1.0f, 1.0f, -1.0f},  {1.0f, 1.0f, 1.0f},  {-1.0f, 1.0f, 1.0f},
		};

		constexpr int edges[][2] = {
			{0, 1}, {1, 2}, {2, 3}, {3, 0}, // bottom
			{4, 5}, {5, 6}, {6, 7}, {7, 4}, // top
			{0, 4}, {1, 5}, {2, 6}, {3, 7}, // vertical
		};

		std::vector<glm::vec3> vertices;
		vertices.reserve(std::size(edges) * 2);

		for (const auto &edge : edges) {
			vertices.push_back(corners[edge[0]]);
			vertices.push_back(corners[edge[1]]);
		}
		return vertices;
	}

	/**
	 * @brief Appends a circle of radius 1 around the origin, in the plane spanned by two axes.
	 *
	 * @param vertices Vector the segments are added to, as pairs of vertices for `GL_LINES`.
	 * @param u First axis of the plane, unit length.
	 * @param v Second axis of the plane, unit length and perpendicular to u.
	 */
	void appendCircle(std::vector<glm::vec3> &vertices, const glm::vec3 &u, const glm::vec3 &v)
	{
		const float step = glm::two_pi<float>() / static_cast<float>(SPHERE_SEGMENTS);

		for (int i = 0; i < SPHERE_SEGMENTS; i++) {
			const float start = static_cast<float>(i) * step;
			const float end = static_cast<float>(i + 1) * step;

			vertices.push_back(std::cos(start) * u + std::sin(start) * v);
			vertices.push_back(std::cos(end) * u + std::sin(end) * v);
		}
	}

	/**
	 * @brief Builds three circles of radius 1 in the XY, XZ and YZ planes, as pairs of vertices.
	 *
	 * Scaled by the radius, it matches a sphere collider.
	 */
	std::vector<glm::vec3> sphereLines()
	{
		constexpr glm::vec3 x{1.0f, 0.0f, 0.0f};
		constexpr glm::vec3 y{0.0f, 1.0f, 0.0f};
		constexpr glm::vec3 z{0.0f, 0.0f, 1.0f};

		std::vector<glm::vec3> vertices;
		vertices.reserve(3 * SPHERE_SEGMENTS * 2);

		appendCircle(vertices, x, y);
		appendCircle(vertices, x, z);
		appendCircle(vertices, y, z);
		return vertices;
	}

	/**
	 * @brief Builds a square grid from -1 to 1 on the XZ plane and an arrow along +Y, as pairs of vertices.
	 *
	 * The plane collider is infinite, so the grid only marks where it is. Scaled uniformly by the shown size and
	 * rotated so that +Y matches the normal, the arrow points to the side of the plane that pushes bodies away.
	 * The arrow is a fraction of the grid, so it keeps its proportions at any size.
	 */
	std::vector<glm::vec3> planeLines()
	{
		constexpr float arrowLength = 0.3f;
		constexpr float headLength = 0.08f;

		std::vector<glm::vec3> vertices;
		vertices.reserve((PLANE_DIVISIONS + 1) * 4 + 6);

		// Lines parallel to X and to Z, crossing the whole grid
		for (int i = 0; i <= PLANE_DIVISIONS; i++) {
			const float t = -1.0f + 2.0f * static_cast<float>(i) / static_cast<float>(PLANE_DIVISIONS);

			vertices.emplace_back(-1.0f, 0.0f, t);
			vertices.emplace_back(1.0f, 0.0f, t);
			vertices.emplace_back(t, 0.0f, -1.0f);
			vertices.emplace_back(t, 0.0f, 1.0f);
		}

		// Arrow shaft along the normal and two head segments, in the XY plane
		const glm::vec3 tip{0.0f, arrowLength, 0.0f};
		vertices.emplace_back(0.0f, 0.0f, 0.0f);
		vertices.push_back(tip);
		vertices.push_back(tip);
		vertices.emplace_back(headLength, arrowLength - headLength, 0.0f);
		vertices.push_back(tip);
		vertices.emplace_back(-headLength, arrowLength - headLength, 0.0f);

		return vertices;
	}

} // unnamed namespace
