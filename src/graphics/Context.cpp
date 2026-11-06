#include <glad/gl.h>

#include "graphics/Context.hpp"

#include <GLFW/glfw3.h>

#include <memory>

#include "common/Logger.hpp"

namespace
{
	const char *cClassName = "Context";

} // unnamed namespace

namespace newton::graphics
{
	common::Logger &Context::m_logger = common::Logger::getInstance();

	/**
	 * @brief Creates the window and loads OpenGL.
	 *
	 * On failure everything created so far is released.
	 *
	 * @param width Window width in screen coordinates.
	 * @param height Window height in screen coordinates.
	 * @param title Window title.
	 * @return The context, or nullptr on failure.
	 */
	std::unique_ptr<Context> Context::create(const int width, const int height, const char *title)
	{
		m_logger.printf(common::LogLevel::INFO, "[%s] Initializing graphics context\n", cClassName);

		glfwSetErrorCallback(errorCallback);

		GLFWwindow *window = setupGLFW(width, height, title);
		if (!window)
			return nullptr;

		if (!setupGLAD()) {
			glfwDestroyWindow(window);
			glfwTerminate();
			return nullptr;
		}

		auto context = std::unique_ptr<Context>(new Context(window));

		m_logger.printf(common::LogLevel::INFO, "[%s] Graphics context initialized successfully\n", cClassName);

		return context;
	}

	/**
	 * @brief Takes ownership of an already created window. Called by @ref create().
	 */
	Context::Context(GLFWwindow *window)
		: m_window(window)
		, m_lastFrameTime(0.0)
		, m_deltaTime(0.0)
		, m_frameCount(0)
		, m_fpsLastTime(0.0)
		, m_currentFPS(0)
		, m_avgFPS(0.0)
	{}

	/**
	 * @brief Destroys the window and terminates GLFW.
	 */
	Context::~Context()
	{
		m_logger.printf(common::LogLevel::INFO, "[%s] Cleaning up graphics context\n", cClassName);

		glfwDestroyWindow(m_window);
		glfwTerminate();

		m_logger.printf(common::LogLevel::INFO, "[%s] Graphics context cleaned up successfully\n", cClassName);
	}

	bool Context::shouldClose()
	{
		return glfwWindowShouldClose(m_window);
	}

	/**
	 * @brief Marks the window to close. The current frame still finishes,
	 * @ref shouldClose() returns true from the next check.
	 */
	void Context::requestClose()
	{
		glfwSetWindowShouldClose(m_window, GLFW_TRUE);
	}

	/**
	 * @brief Updates @ref m_deltaTime, @ref m_currentFPS and @ref m_avgFPS. Call once per frame.
	 */
	void Context::update()
	{
		calculateDelta();
		calculateFPS();
		calculateAvgFPS();
	}

	/**
	 * @brief Processes pending window events, which triggers the input callbacks.
	 */
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

	/**
	 * @return Time between the last two @ref update() calls, in seconds.
	 */
	double Context::deltaTime()
	{
		return m_deltaTime;
	}

	/**
	 * @return Frames counted in the last full second, refreshed once per second.
	 */
	int Context::FPS()
	{
		return m_currentFPS;
	}

	double Context::avgFPS()
	{
		return m_avgFPS;
	}

	/**
	 * @brief GLFW callback, logs every GLFW error. Registered before glfwInit, so it also catches init errors.
	 */
	void Context::errorCallback(const int code, const char *description)
	{
		m_logger.printf(common::LogLevel::ERROR, "[%s] GLFW error 0x%X: %s\n", cClassName,
		                static_cast<unsigned int>(code), description);
	}

	/**
	 * @brief Initializes GLFW and creates the window with its OpenGL context.
	 *
	 * Terminates GLFW itself if the window cannot be created.
	 *
	 * @return The created window, or nullptr on failure.
	 */
	GLFWwindow *Context::setupGLFW(const int width, const int height, const char *title)
	{
		if (!glfwInit()) {
			m_logger.printf(common::LogLevel::ERROR, "[%s] Failed to initialize GLFW\n", cClassName);
			return nullptr;
		}

		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_SAMPLES, 4);

		GLFWwindow *window = glfwCreateWindow(width, height, title, nullptr, nullptr);

		if (glfwGetError(nullptr) == GLFW_VERSION_UNAVAILABLE)
			 m_logger.printf(common::LogLevel::ERROR, "[%s] OpenGL 3.3 not supported by the driver\n", cClassName);

		if (!window) {
			glfwTerminate();
			m_logger.printf(common::LogLevel::ERROR, "[%s] Failed to create GLFW window\n", cClassName);
			return nullptr;
		}

		glfwMakeContextCurrent(window);

		glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

		glfwSetWindowAspectRatio(window, 16, 9);

		glfwSwapInterval(0);

		return window;
	}

	/**
	 * @brief Loads OpenGL functions and sets the global render state.
	 *
	 * Requires a current OpenGL context, so it must run after @ref setupGLFW().
	 *
	 * @return True on success.
	 */
	bool Context::setupGLAD()
	{
		int version = gladLoadGL(glfwGetProcAddress);

		if (version == 0) {
			m_logger.printf(common::LogLevel::ERROR, "[%s] Failed to initialize OpenGL context\n", cClassName);
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

	/**
	 * @brief GLFW callback, resizes the viewport to the framebuffer.
	 */
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

	/**
	 * @brief Counts frames and stores the count once every full second.
	 */
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

	/**
	 * @brief Updates the smoothed FPS with an exponential moving average.
	 */
	void Context::calculateAvgFPS()
	{
		double currentFPS = 1.0 / m_deltaTime;
		m_avgFPS = 0.9 * m_avgFPS + 0.1 * currentFPS;
	}

} // namespace newton::graphics
