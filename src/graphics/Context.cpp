#include <glad/gl.h>

#include "graphics/Context.hpp"

#include <GLFW/glfw3.h>

#include <chrono>
#include <memory>
#include <thread>

#include "common/Logger.hpp"
#include "graphics/GLError.hpp"

namespace
{
	const char *cClassName = "Context";

	int queryMonitorRefreshRate();

	constexpr int FALLBACK_FRAME_LIMIT = 144;
	constexpr bool DEFAULT_SYNC = false;

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
		m_logger.printf(common::LogLevel::INFO, "[%s] VSync %s, FPS limit %d\n", cClassName,
		                context->isVSync() ? "ON" : "OFF", context->frameLimit());

		return context;
	}

	/**
	 * @brief Takes ownership of an already created window. Called by @ref create().
	 */
	Context::Context(GLFWwindow *window)
		: m_window(window)
		, m_monitorRefreshRate(0)
		, m_lastFrameTime(0.0)
		, m_deltaTime(0.0)
		, m_frameLimit(0)
		, m_nextFrameTime(0.0)
		, m_vsyncOn(DEFAULT_SYNC)
		, m_frameCount(0)
		, m_fpsLastTime(0.0)
		, m_currentFPS(0)
		, m_avgFPS(0.0)
	{
		m_monitorRefreshRate = queryMonitorRefreshRate();
		m_frameLimit = m_monitorRefreshRate > 0 ? m_monitorRefreshRate : FALLBACK_FRAME_LIMIT;

		if (m_monitorRefreshRate == 0) {
			m_logger.printf(common::LogLevel::WARN,
			                "[%s] Failed to get primary monitor refresh rate, using fallback value: %d\n", cClassName,
			                FALLBACK_FRAME_LIMIT);
		} else {
			m_logger.printf(common::LogLevel::INFO, "[%s] Monitor refresh rate: %d Hz\n", cClassName,
			                m_monitorRefreshRate);
		}
	}

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

	/**
	 * @brief Shows the rendered frame, first waiting so that the frame rate does not exceed @ref frameLimit().
	 *
	 * Every frame is scheduled one budget (1 / limit) after the previous scheduled time, not after the
	 * actual swap, so oversleeping in one frame shortens the wait in the next and the average hits the limit.
	 * A frame that is already late is shown immediately and the schedule restarts from now, without catching up.
	 * The wait blocks the calling thread, so it limits the whole main loop, not only drawing.
	 */
	void Context::swapBuffers()
	{
		if (m_frameLimit > 0) {
			const double frameBudget = 1.0 / m_frameLimit;
			m_nextFrameTime += frameBudget;

			double now = glfwGetTime();
			if (m_nextFrameTime < now)
				m_nextFrameTime = now; // fell behind (slow frame, window drag), don't try to catch up
			else
				while (glfwGetTime() < m_nextFrameTime)
					std::this_thread::sleep_for(std::chrono::microseconds(100));
		}

		glfwSwapBuffers(m_window);
	}

	GLFWwindow *Context::window()
	{
		return m_window;
	}

	/**
	 * @brief Sets the maximum frame rate enforced by @ref swapBuffers(). Takes effect from the next swap.
	 *
	 * With vsync enabled the swap already waits for the monitor, so a limit above its refresh rate has no effect.
	 *
	 * @param fps Max frames per second, 0 or less disables the limit.
	 */
	void Context::setFrameLimit(int fps)
	{
		if (fps == m_frameLimit)
			return;

		m_frameLimit = fps;

		if (m_frameLimit > 0)
			m_logger.printf(common::LogLevel::INFO, "[%s] FPS limit set to %d\n", cClassName, m_frameLimit);
		else
			m_logger.printf(common::LogLevel::INFO, "[%s] FPS limit disabled\n", cClassName);
	}

	/**
	 * @return Max frames per second, 0 or less means unlimited.
	 */
	int Context::frameLimit() const
	{
		return m_frameLimit;
	}

	/**
	 * @brief Refresh rate of the primary monitor, read once when the context was created.
	 *
	 * @return Refresh rate in Hz, or 0 if it could not be read.
	 */
	int Context::monitorRefreshRate() const
	{
		return m_monitorRefreshRate;
	}

	/**
	 * @brief Enables or disables waiting for the monitor refresh on buffer swap. Off by default.
	 *
	 * Applies to the current OpenGL context, which is this window's context. The frame limit from
	 * @ref setFrameLimit() still applies on top, but a limit above the refresh rate then has no effect.
	 *
	 * @param enabled True to sync swaps to the monitor refresh.
	 */
	void Context::setVSync(bool enabled)
	{
		if (enabled == m_vsyncOn)
			return;

		glfwSwapInterval(enabled ? 1 : 0);

		m_vsyncOn = enabled;

		const char *mode = m_vsyncOn ? "ON" : "OFF";
		m_logger.printf(common::LogLevel::INFO, "[%s] VSync is now %s\n", cClassName, mode);
	}

	/**
	 * @return True if buffer swaps wait for the monitor refresh.
	 */
	bool Context::isVSync() const
	{
		return m_vsyncOn;
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

		if (!window) {
			if (glfwGetError(nullptr) == GLFW_VERSION_UNAVAILABLE)
				m_logger.printf(common::LogLevel::ERROR, "[%s] OpenGL 3.3 not supported by the driver\n", cClassName);
			glfwTerminate();
			m_logger.printf(common::LogLevel::ERROR, "[%s] Failed to create GLFW window\n", cClassName);
			return nullptr;
		}

		glfwMakeContextCurrent(window);

		glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

		glfwSetWindowAspectRatio(window, 16, 9);

		glfwSwapInterval(DEFAULT_SYNC ? 1 : 0);

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
		if (gladLoadGL(glfwGetProcAddress) == 0) {
			m_logger.printf(common::LogLevel::ERROR, "[%s] Failed to initialize OpenGL context\n", cClassName);
			return false;
		}

		const char *vendor = reinterpret_cast<const char *>(glGetString(GL_VENDOR));
		const char *renderer = reinterpret_cast<const char *>(glGetString(GL_RENDERER));
		const char *version = reinterpret_cast<const char *>(glGetString(GL_VERSION));
		const char *shadingLangVers = reinterpret_cast<const char *>(glGetString(GL_SHADING_LANGUAGE_VERSION));

		m_logger.printf(common::LogLevel::INFO, "[%s] OpenGL vendor: %s\n", cClassName, vendor);
		m_logger.printf(common::LogLevel::INFO, "[%s] OpenGL renderer: %s\n", cClassName, renderer);
		m_logger.printf(common::LogLevel::INFO, "[%s] OpenGL version: %s\n", cClassName, version);
		m_logger.printf(common::LogLevel::INFO, "[%s] OpenGL GLSL: %s\n", cClassName, shadingLangVers);

		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);

		glEnable(GL_MULTISAMPLE);

		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		glFrontFace(GL_CCW);

		checkGLError("Context::setupGLAD");

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

namespace
{
	/**
	 * @brief Reads the refresh rate of the primary monitor's current video mode. Requires initialized GLFW.
	 *
	 * The value is an integer, so e.g. 59.94 Hz may be reported as 59 or 60.
	 *
	 * @return Refresh rate in Hz, or 0 if there is no monitor or its video mode is unavailable.
	 */
	int queryMonitorRefreshRate()
	{
		GLFWmonitor *monitor = glfwGetPrimaryMonitor();
		if (!monitor)
			return 0;

		const GLFWvidmode *mode = glfwGetVideoMode(monitor);
		return mode ? mode->refreshRate : 0;
	}

} // unnamed namespace
