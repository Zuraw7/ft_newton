#ifndef GRAPHICS_CONTEXT_HPP
#define GRAPHICS_CONTEXT_HPP

#include <GLFW/glfw3.h>

#include <memory>

#include "common/Logger.hpp"
#include "graphics/Resolution.hpp"

namespace newton::graphics
{
	/**
	 * @brief Owner of the window, the OpenGL context and frame timing.
	 *
	 * Created only through @ref create(), so an existing object always has a valid window.
	 * Only one instance may exist at a time, because the destructor terminates GLFW.
	 * The window can have any size and aspect ratio, see @ref aspectRatio().
	 */
	class Context
	{
		public:
			static std::unique_ptr<Context> create(int width, int height, const char *title);
			static std::unique_ptr<Context> create(const char *title);
			~Context();

			Context(const Context &) = delete;
			Context &operator=(const Context &) = delete;

			bool shouldClose();
			void requestClose();
			void update();
			void pollEvents();
			void swapBuffers();
			GLFWwindow *window();

			void setFrameLimit(int fps);
			int frameLimit() const;
			int monitorRefreshRate() const;

			void setVSync(bool enabled);
			bool isVSync() const;

			void setResolution(Resolution resolution);
			Resolution resolution() const;
			Resolution monitorResolution() const;
			float aspectRatio() const;
			void setFullscreen(bool enabled);
			bool isFullscreen() const;

			double deltaTime();
			int FPS();
			double avgFPS();

		private:
			explicit Context(GLFWwindow *window);

			static void errorCallback(int code, const char *description);
			static GLFWwindow *setupGLFW(int width, int height, const char *title);
			static bool setupGLAD();
			static void framebufferSizeCallback(GLFWwindow *window, const int width, const int height);

			void calculateDelta();
			void calculateFPS();
			void calculateAvgFPS();

			static common::Logger &m_logger;

			GLFWwindow *m_window;
			int m_monitorRefreshRate;        ///< Primary monitor refresh rate read at creation, in Hz. 0 if unknown.
			Resolution m_monitorResolution;  ///< Primary monitor size read at creation. 0 x 0 if unknown.
			Resolution m_windowedResolution; ///< Window size to restore when leaving fullscreen.
			int m_windowedX;                 ///< Window X position to restore. Unused on Wayland.
			int m_windowedY;                 ///< Window Y position to restore. Unused on Wayland.
			bool m_windowedMaximized;        ///< True if the window was maximized when entering fullscreen.

			double m_lastFrameTime; ///< GLFW time of the previous @ref update(), in seconds.
			double m_deltaTime;     ///< Time between the last two @ref update() calls, in seconds.

			int m_frameLimit;       ///< Max frames per second, 0 or less means unlimited.
			double m_nextFrameTime; ///< GLFW time the next buffer swap is scheduled for, in seconds.

			bool m_vsyncOn; ///< True if buffer swaps wait for the monitor refresh.

			int m_frameCount;     ///< Frames counted in the current one-second window.
			double m_fpsLastTime; ///< GLFW time when the current one-second window started.
			int m_currentFPS;     ///< Frames counted in the last full second.
			double m_avgFPS;      ///< Smoothed FPS.
	};

} // namespace newton::graphics

#endif // GRAPHICS_CONTEXT_HPP
