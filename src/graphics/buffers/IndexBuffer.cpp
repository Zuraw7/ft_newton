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

	void IndexBuffer::bind() const
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_id);
	}

	void IndexBuffer::unbind() const
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

} // namespace newton::graphics
