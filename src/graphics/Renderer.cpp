#include "graphics/Renderer.hpp"

#include <glad/gl.h>

#include "common/Logger.hpp"

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

	/**
	 * @brief Sets the color used by @ref clear(). Components are in the [0, 1] range.
	 */
	void Renderer::setBackgroundColor(const float red, const float green, const float blue, const float alpha)
	{
		glClearColor(red, green, blue, alpha);
	}

	/**
	 * @brief Clears the color and depth buffers. Call before drawing the frame.
	 */
	void Renderer::clear() const
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	/**
	 * @brief Switches between filled and wireframe drawing.
	 */
	void Renderer::togglePolygonMode()
	{
		// TODO: apply with glPolygonMode in draw() once rendering is implemented
		m_polygonMode = !m_polygonMode;
	}

} // namespace newton::graphics
