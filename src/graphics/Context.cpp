#include <glad/gl.h>

#include "graphics/Context.hpp"

#include <GLFW/glfw3.h>

#include <chrono>
#include <memory>
#include <thread>

#include "common/Logger.hpp"
#include "graphics/GLError.hpp"
#include "graphics/Resolution.hpp"

namespace
{
	const char *cClassName = "Context";

	const GLFWvidmode *queryVideoMode();

	constexpr int FALLBACK_FRAME_LIMIT = 60;
	constexpr float FALLBACK_ASPECT_RATIO = 16.0f / 9.0f;
	constexpr bool DEFAULT_SYNC = false;

	using newton::graphics::Resolution;

	/// Window sizes to start with when none is given, from the largest.
	constexpr Resolution cStartupResolutions[] = {newton::graphics::UHD, newton::graphics::QHD, newton::graphics::FHD,
	                                              newton::graphics::HDP, newton::graphics::HD};

	Resolution startupResolution(const GLFWvidmode *mode);

} // unnamed namespace

namespace newton::graphics
{
	common::Logger &Context::m_logger = common::Logger::getInstance();

	/**
	 * @brief Creates the window and loads OpenGL.
	 *
	 * On failure everything created so far is released.
	 *
	 * @param width Window width in screen coordinates. With 0 or less the size is chosen as in @ref create(const char
	 * *).
	 * @param height Window height in screen coordinates. With 0 or less the size is chosen as in @ref create(const char
	 * *).
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
	 * @brief Creates the window with the largest 16:9 preset smaller than the primary monitor in both dimensions.
	 *
	 * The window leaves room for its title bar and desktop panels, e.g. 1920 x 1080 on a 2560 x 1440 monitor.
	 * If the monitor cannot be read or all presets are too large, the size is 960 x 540.
	 *
	 * @param title Window title.
	 * @return The context, or nullptr on failure.
	 */
	std::unique_ptr<Context> Context::create(const char *title)
	{
		return create(0, 0, title);
	}

	/**
	 * @brief Takes ownership of an already created window. Called by @ref create().
	 */
	Context::Context(GLFWwindow *window)
		: m_window(window)
		, m_monitorRefreshRate(0)
		, m_monitorResolution(0, 0)
		, m_windowedResolution(0, 0)
		, m_windowedX(0)
		, m_windowedY(0)
		, m_windowedMaximized(false)
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
		const GLFWvidmode *mode = queryVideoMode();

		if (mode) {
			m_monitorRefreshRate = mode->refreshRate;
			m_monitorResolution = Resolution(mode->width, mode->height);
			m_frameLimit = m_monitorRefreshRate > 0 ? m_monitorRefreshRate : FALLBACK_FRAME_LIMIT;

			m_logger.printf(common::LogLevel::INFO, "[%s] Primary monitor: %dx%d, %d Hz\n", cClassName, mode->width,
			                mode->height, mode->refreshRate);
		} else {
			m_frameLimit = FALLBACK_FRAME_LIMIT;
			m_logger.printf(common::LogLevel::WARN, "[%s] Failed to read primary monitor, using frame limit %d\n",
			                cClassName, FALLBACK_FRAME_LIMIT);
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
	 * @brief Requests a new window size.
	 *
	 * The window system may apply it later or limit it, so @ref resolution() may not return this value
	 * right away. The OpenGL viewport follows the size through the framebuffer callback.
	 *
	 * @param res New window size in screen coordinates.
	 */
	void Context::setResolution(Resolution res)
	{
		if (res == resolution())
			return;

		glfwSetWindowSize(m_window, res.width, res.height);

		m_logger.printf(common::LogLevel::INFO, "[%s] Window size set to %s\n", cClassName, res.toString().c_str());
	}

	/**
	 * @brief Switches between fullscreen on the primary monitor and a regular window.
	 *
	 * Fullscreen uses the current video mode of the monitor, so its resolution and refresh rate do not change.
	 * Entering stores the window size, maximized state and position, leaving restores them. Wayland does not
	 * expose the window position, so there the system places the window. Does nothing if the state is already
	 * set. Without a monitor it logs a warning and stays windowed. The viewport follows through the framebuffer
	 * callback, the aspect ratio changes too, see @ref aspectRatio().
	 *
	 * @param enabled True enters fullscreen, false returns to the window.
	 */
	void Context::setFullscreen(bool enabled)
	{
		if (enabled == isFullscreen())
			return;

		if (enabled) {
			GLFWmonitor *monitor = glfwGetPrimaryMonitor();
			const GLFWvidmode *mode = queryVideoMode();
			if (!monitor || !mode) {
				m_logger.printf(common::LogLevel::WARN, "[%s] Cannot enter fullscreen: no monitor\n", cClassName);
				return;
			}

			m_windowedMaximized = glfwGetWindowAttrib(m_window, GLFW_MAXIMIZED) != 0;
			m_windowedResolution = resolution();

			if (glfwGetPlatform() != GLFW_PLATFORM_WAYLAND)
				glfwGetWindowPos(m_window, &m_windowedX, &m_windowedY);

			glfwSetWindowMonitor(m_window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
		} else {
			glfwSetWindowMonitor(m_window, nullptr, m_windowedX, m_windowedY, m_windowedResolution.width,
			                     m_windowedResolution.height, GLFW_DONT_CARE);
			if (m_windowedMaximized)
				glfwMaximizeWindow(m_window);
		}

		m_logger.printf(common::LogLevel::INFO, "[%s] Fullscreen %s\n", cClassName, enabled ? "ON" : "OFF");
	}

	/**
	 * @return True if the window is currently fullscreen.
	 */
	bool Context::isFullscreen() const
	{
		return glfwGetWindowMonitor(m_window) != nullptr;
	}

	/**
	 * @return Current window size in screen coordinates.
	 */
	Resolution Context::resolution() const
	{
		int width = 0;
		int height = 0;
		glfwGetWindowSize(m_window, &width, &height);

		return {width, height};
	}

	/**
	 * @brief Size of the primary monitor, read once when the context was created.
	 *
	 * With several monitors the window may be on another one, its size is not detected.
	 *
	 * @return Monitor size, or 0 x 0 if it could not be read.
	 */
	Resolution Context::monitorResolution() const
	{
		return m_monitorResolution;
	}

	/**
	 * @brief Width divided by height of the framebuffer, the value a camera projection needs.
	 *
	 * Reads the current size on every call, so it follows window resizing. Pass it to
	 * @ref Camera::setAspectRatio() once per frame.
	 *
	 * @return The aspect ratio, or 16:9 if the framebuffer has no area, e.g. in a minimized window.
	 */
	float Context::aspectRatio() const
	{
		int width = 0;
		int height = 0;
		glfwGetFramebufferSize(m_window, &width, &height);

		if (width == 0 || height == 0)
			return FALLBACK_ASPECT_RATIO;

		return static_cast<float>(width) / static_cast<float>(height);
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
	GLFWwindow *Context::setupGLFW(int width, int height, const char *title)
	{
		if (!glfwInit()) {
			m_logger.printf(common::LogLevel::ERROR, "[%s] Failed to initialize GLFW\n", cClassName);
			return nullptr;
		}

		// No size given: choose it from the primary monitor
		if (width <= 0 || height <= 0) {
			const Resolution startup = startupResolution(queryVideoMode());
			width = startup.width;
			height = startup.height;
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
	 * @brief Reads the current video mode of the primary monitor: size and refresh rate. Requires initialized GLFW.
	 *
	 * The refresh rate is an integer, so e.g. 59.94 Hz may be reported as 59 or 60.
	 * The result is owned by GLFW and may become invalid, e.g. when the monitor is disconnected,
	 * so copy the values instead of storing the pointer.
	 *
	 * @return The video mode, or nullptr if there is no monitor or its video mode is unavailable.
	 */
	const GLFWvidmode *queryVideoMode()
	{
		GLFWmonitor *monitor = glfwGetPrimaryMonitor();
		return monitor ? glfwGetVideoMode(monitor) : nullptr;
	}

	/**
	 * @brief Chooses the window size used when none is given.
	 *
	 * @param mode Video mode of the monitor, may be nullptr.
	 * @return The first of @ref cStartupResolutions smaller than the monitor, or qHD if there is none.
	 */
	Resolution startupResolution(const GLFWvidmode *mode)
	{
		if (!mode)
			return newton::graphics::qHD;

		const Resolution monitor(mode->width, mode->height);
		for (const Resolution &resolution : cStartupResolutions)
			if (resolution.isSmallerThan(monitor))
				return resolution;

		return newton::graphics::qHD;
	}

} // unnamed namespace
