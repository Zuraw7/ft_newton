#include "graphics/buffers/VertexBuffer.hpp"

#include <glad/gl.h>
#include <cstddef>
#include "common/Logger.hpp"
#include "graphics/GLError.hpp"

namespace
{
	const char *cClassName = "VertexBuffer";
	newton::common::Logger &logger = newton::common::Logger::getInstance();

} // unnamed namespace

namespace newton::graphics
{
	VertexBuffer::VertexBuffer(const void *data, std::size_t bytes)
		: m_id(0)
	{
		glGenBuffers(1, &m_id);
		glBindBuffer(GL_ARRAY_BUFFER, m_id);
		glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(bytes), data, GL_STATIC_DRAW);

		graphics::checkGLError("VertexBuffer::VertexBuffer");

		logger.printf(common::LogLevel::DEBUG, "[%s] VBO [%u] created, size: %zu bytes\n", cClassName, m_id, bytes);
	}

	VertexBuffer::~VertexBuffer()
	{
		if (m_id)
			glDeleteBuffers(1, &m_id);
	}

	VertexBuffer::VertexBuffer(VertexBuffer &&other) noexcept
		: m_id(other.m_id)
	{
		other.m_id = 0;
	}

	VertexBuffer &VertexBuffer::operator=(VertexBuffer &&other) noexcept
	{
		if (this == &other)
			return *this;

		if (m_id)
			glDeleteBuffers(1, &m_id);

		m_id = other.m_id;
		other.m_id = 0;
		return *this;
	}

	void VertexBuffer::bind() const
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_id);
	}

	void VertexBuffer::unbind() const
	{
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

} // namespace newton::graphics
