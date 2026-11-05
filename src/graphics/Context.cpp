#include <glad/gl.h>

#include "../../include/graphics/Context.hpp"

#include "../../include/common/Logger.hpp"

namespace
{
	const char *cClassName = "Context";

} // unnamed namespace

namespace newton::graphics
{
	common::Logger &Context::m_logger = common::Logger::getInstance();

	GLFWwindow *Context::m_window = nullptr;
	double Context::m_lastFrameTime = 0.0;
	double Context::m_deltaTime = 0.0;

	double Context::m_fpsLastTime = 0.0;
	int Context::m_frameCount = 0;
	int Context::m_currentFPS = 0;
	double Context::m_avgFPS = 0.0;

	bool Context::setup(const int width, const int height, const char *title)
	{
		m_logger.printf(common::LogLevel::INFO, "[%s] Initializing graphics context\n", cClassName);

		if (!setupGLFW(width, height, title))
			return false;
		if (!setupGLAD()) {
			cleanupGLFW();
			return false;
		}

		m_logger.printf(common::LogLevel::INFO, "[%s] Graphics context initialized successfully\n", cClassName);

		return true;
	}

	void Context::cleanup()
	{
		cleanupGLFW();
	}

	bool Context::shouldClose()
	{
		return glfwWindowShouldClose(m_window);
	}

	void Context::update()
	{
		calculateDelta();
		calculateFPS();
		calculateAvgFPS();
	}

	void Context::pollEvents()
	{
		glfwPollEvents();
	}

	void Context::swapBuffers()
	{
		glfwSwapBuffers(m_window);
	}

	GLFWwindow *Context::window()
	{
		return m_window;
	}

	double Context::deltaTime()
	{
		return m_deltaTime;
	}

	int Context::FPS()
	{
		return m_currentFPS;
	}

	double Context::avgFPS()
	{
		return m_avgFPS;
	}

	void Context::calculateAvgFPS()
	{
		double currentFPS = 1.0 / m_deltaTime;
		m_avgFPS = 0.9 * m_avgFPS + 0.1 * currentFPS;
	}

	bool Context::setupGLFW(const int width, const int height, const char *title)
	{
		if (!glfwInit()) {
			m_logger.printf(common::LogLevel::ERROR, "[%s] Failed to init GLFW\n", cClassName);
			return false;
		}

		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_SAMPLES, 4);

		m_window = glfwCreateWindow(width, height, title, nullptr, nullptr);
		if (!m_window) {
			glfwTerminate();
			m_logger.printf(common::LogLevel::ERROR, "[%s] Failed to create GLFW window\n", cClassName);
			return false;
		}

		glfwMakeContextCurrent(m_window);

		glfwSetFramebufferSizeCallback(m_window, framebufferSizeCallback);

		glfwSetWindowAspectRatio(m_window, 16, 9);

		glfwSwapInterval(0);
		return true;
	}

	void Context::cleanupGLFW()
	{
		glfwDestroyWindow(m_window);
		glfwTerminate();
	}

	bool Context::setupGLAD()
	{
		int version = gladLoadGL(glfwGetProcAddress);

		if (version == 0) {
			m_logger.printf(common::LogLevel::ERROR, "[%s] Failed to initialized OpenGL context\n", cClassName);
			return false;
		}

		m_logger.printf(common::LogLevel::INFO, "[%s] Loaded OpenGL %d.%d\n", cClassName, GLAD_VERSION_MAJOR(version),
		                GLAD_VERSION_MINOR(version));

		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);

		glEnable(GL_MULTISAMPLE);

		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		glFrontFace(GL_CCW);

		return true;
	}

	void Context::framebufferSizeCallback(GLFWwindow *window, const int width, const int height)
	{
		(void) window;
		glViewport(0, 0, width, height);
	}

	void Context::calculateDelta()
	{
		double currentTime = glfwGetTime();
		m_deltaTime = currentTime - m_lastFrameTime;
		m_lastFrameTime = currentTime;
	}

	void Context::calculateFPS()
	{
		m_frameCount++;
		double currentTime = glfwGetTime();
		if (currentTime - m_fpsLastTime >= 1.0) {
			m_currentFPS = m_frameCount;
			m_frameCount = 0;
			m_fpsLastTime = currentTime;
		}
	}

} // namespace newton::graphics
