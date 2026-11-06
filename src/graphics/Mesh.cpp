#include "graphics/Mesh.hpp"

#include <glad/gl.h>

#include <cstddef>
#include "common/Logger.hpp"
#include "graphics/GLError.hpp"

namespace
{
	const char *cClassName = "Mesh";
	newton::common::Logger &logger = newton::common::Logger::getInstance();

} // unnamed namespace

namespace newton::graphics
{
	/**
	 * @brief Uploads the vertices and indices to the GPU and sets up the vertex attributes.
	 *
	 * The data is copied to the GPU, so the vectors can be discarded afterwards.
	 *
	 * @param vertices Vertex data.
	 * @param indices Triangle indices into @p vertices, counter-clockwise for front faces.
	 */
	Mesh::Mesh(const std::vector<Vertex> &vertices, const std::vector<unsigned int> &indices)
		: m_VAO()
		, m_VBO(vertices)
		, m_EBO(indices)
		, m_indexCount(static_cast<int>(indices.size()))
	{
		m_VAO.bind();
		m_VBO.bind();
		m_EBO.bind();

		setupMesh();

		m_VAO.unbind();
		m_VBO.unbind();
		m_EBO.unbind();

		graphics::checkGLError("Mesh::Mesh");

		logger.printf(common::LogLevel::DEBUG, "[%s] Mesh created: %zu vertices, %d indices\n", cClassName,
		              vertices.size(), m_indexCount);
	}

	/**
	 * @brief Draws the mesh as triangles with the currently bound shader.
	 */
	void Mesh::draw() const
	{
		m_VAO.bind();
		glDrawElements(GL_TRIANGLES, m_indexCount, GL_UNSIGNED_INT, nullptr);
		m_VAO.unbind();
	}

	/**
	 * @brief Describes the @ref Vertex layout to OpenGL.
	 *
	 * Must be called with @ref m_VAO and @ref m_VBO bound, so the attributes are recorded in the VAO
	 * and read from the VBO.
	 */
	void Mesh::setupMesh()
	{
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), nullptr);

		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
		                      reinterpret_cast<void *>(offsetof(Vertex, normal)));

		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
		                      reinterpret_cast<void *>(offsetof(Vertex, texCoords)));
	}

} // namespace newton::graphics
