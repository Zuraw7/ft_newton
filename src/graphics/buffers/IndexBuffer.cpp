#include "graphics/buffers/IndexBuffer.hpp"

#include <glad/gl.h>
#include "common/Logger.hpp"
#include "graphics/GLError.hpp"

namespace
{
	const char *cClassName = "IndexBuffer";
	newton::common::Logger &logger = newton::common::Logger::getInstance();

} // unnamed namespace

namespace newton::graphics
{

	/**
	 * @brief Creates the buffer, binds it to GL_ELEMENT_ARRAY_BUFFER and uploads the indices.
	 *
	 * The buffer stays bound after construction, so it attaches to the currently bound VAO.
	 *
	 * @param indices Pointer to the indices.
	 * @param count Number of indices, not bytes.
	 */
	IndexBuffer::IndexBuffer(const unsigned int *indices, std::size_t count)
		: m_id(0)
	{
		glGenBuffers(1, &m_id);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_id);

		const std::size_t bytes = count * sizeof(unsigned int);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLsizeiptr>(bytes), indices, GL_STATIC_DRAW);

		graphics::checkGLError("IndexBuffer::IndexBuffer");

		logger.printf(common::LogLevel::DEBUG, "[%s] EBO [%u] created, size: %zu bytes\n", cClassName, m_id, bytes);
	}

	/**
	 * @brief Same as the pointer version, for a vector of indices.
	 */
	IndexBuffer::IndexBuffer(const std::vector<unsigned int> &indices)
		: IndexBuffer(indices.data(), indices.size())
	{}

	IndexBuffer::~IndexBuffer()
	{
		if (m_id)
			glDeleteBuffers(1, &m_id);
	}

	IndexBuffer::IndexBuffer(IndexBuffer &&other) noexcept
		: m_id(other.m_id)
	{
		other.m_id = 0;
	}

	IndexBuffer &IndexBuffer::operator=(IndexBuffer &&other) noexcept
	{
		if (this == &other)
			return *this;

		if (m_id)
			glDeleteBuffers(1, &m_id);

		m_id = other.m_id;
		other.m_id = 0;
		return *this;
	}

	/**
	 * @brief Binds the buffer to GL_ELEMENT_ARRAY_BUFFER, attaching it to the currently bound VAO.
	 */
	void IndexBuffer::bind() const
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_id);
	}

	/**
	 * @brief Unbinds any buffer from GL_ELEMENT_ARRAY_BUFFER.
	 *
	 * @warning While a VAO is bound this detaches the index buffer from that VAO.
	 * Unbind the VAO first.
	 */
	void IndexBuffer::unbind() const
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

} // namespace newton::graphics
