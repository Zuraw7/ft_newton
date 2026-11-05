#ifndef GRAPHICS_CONTEXT_HPP
#define GRAPHICS_CONTEXT_HPP

#include <GLFW/glfw3.h>

#include "common/Logger.hpp"

namespace newton::graphics
{
	/**
	 * @brief Static owner of the window, the OpenGL context and frame timing.
	 *
	 * Creates an OpenGL context. The window keeps a 16:9 aspect ratio. @ref setup()
	 * must succeed before any other call.
	 */
	class Context
	{
		public:
			Context() = delete;

			static bool setup(int width, int height, const char *title);
			static void cleanup();
			static bool shouldClose();
			static void requestClose();
			static void update();
			static void pollEvents();
			static void swapBuffers();
			static GLFWwindow *window();

			static double deltaTime();
			[[deprecated("Use avgFPS() for more stable, smoothed value.")]] static int FPS();
			static double avgFPS();

		private:
			static bool setupGLFW(int width, int height, const char *title);
			static void cleanupGLFW();
			static bool setupGLAD();

			static void framebufferSizeCallback(GLFWwindow *window, const int width, const int height);

			static void calculateDelta();
			static void calculateFPS();
			static void calculateAvgFPS();

			static common::Logger &m_logger;

			static GLFWwindow *m_window;   ///< Null until @ref setup() succeeds.
			static double m_lastFrameTime; ///< GLFW time of the previous @ref update(), in seconds.
			static double m_deltaTime;     ///< Time between the last two @ref update() calls, in seconds.

			static int m_frameCount;     ///< Frames counted in the current one-second window.
			static double m_fpsLastTime; ///< GLFW time when the current one-second window started.
			static int m_currentFPS;     ///< Frames counted in the last full second.
			static double m_avgFPS;      ///< Smoothed FPS.
	};

} // namespace newton::graphics

#endif // GRAPHICS_CONTEXT_HPP
