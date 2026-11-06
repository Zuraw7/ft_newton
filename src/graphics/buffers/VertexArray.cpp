#include "graphics/buffers/VertexArray.hpp"

#include <glad/gl.h>
#include "common/Logger.hpp"
#include "graphics/GLError.hpp"

namespace
{
	const char *cClassName = "VertexArray";
	newton::common::Logger &logger = newton::common::Logger::getInstance();

} // unnamed namespace

namespace newton::graphics
{

	VertexArray::VertexArray()
		: m_id(0)
	{
		glGenVertexArrays(1, &m_id);
		glBindVertexArray(m_id);

		graphics::checkGLError("VertexArray::VertexArray");

		logger.printf(common::LogLevel::DEBUG, "[%s] VAO [%u] created\n", cClassName, m_id);
	}

	VertexArray::~VertexArray()
	{
		if (m_id)
			glDeleteVertexArrays(1, &m_id);
	}

	VertexArray::VertexArray(VertexArray &&other) noexcept
		: m_id(other.m_id)
	{
		other.m_id = 0;
	}

	VertexArray &VertexArray::operator=(VertexArray &&other) noexcept
	{
		if (this == &other)
			return *this;

		if (m_id)
			glDeleteVertexArrays(1, &m_id);

		m_id = other.m_id;
		other.m_id = 0;
		return *this;
	}

	void VertexArray::bind() const
	{
		glBindVertexArray(m_id);
	}

	void VertexArray::unbind() const
	{
		glBindVertexArray(0);
	}

} // namespace newton::graphics
