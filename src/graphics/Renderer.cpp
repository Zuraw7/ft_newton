#include "../../include/graphics/Renderer.hpp"

#include <glad/gl.h>

#include "../../include/common/Logger.hpp"

namespace
{
	const char *cClassName = "Renderer";

} // unnamed namespace

namespace newton::graphics
{
	Renderer::Renderer()
		: m_logger(common::Logger::getInstance())
		, m_polygonMode(false)
	{
		m_logger.printf(common::LogLevel::INFO, "[%s] Renderer initialized successfully\n", cClassName);
	}

	void Renderer::setBackgroundColor(const float red, const float green, const float blue, const float alpha)
	{
		glClearColor(red, green, blue, alpha);
	}

	void Renderer::clear() const
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void Renderer::togglePolygonMode()
	{
		// TODO: apply with glPolygonMode in draw() once rendering is implemented
		m_polygonMode = !m_polygonMode;
	}

} // namespace newton::graphics
