#ifndef GRAPHICS_CONTEXT_HPP
#define GRAPHICS_CONTEXT_HPP

#include <GLFW/glfw3.h>

#include <memory>

#include "common/Logger.hpp"

namespace newton::graphics
{
	/**
	 * @brief Owner of the window, the OpenGL context and frame timing.
	 *
	 * Created only through @ref create(), so an existing object always has a valid window.
	 * Only one instance may exist at a time, because the destructor terminates GLFW.
	 * The window keeps a 16:9 aspect ratio.
	 */
	class Context
	{
		public:
			static std::unique_ptr<Context> create(int width, int height, const char *title);
			~Context();

			Context(const Context &) = delete;
			Context &operator=(const Context &) = delete;

			bool shouldClose();
			void requestClose();
			void update();
			void pollEvents();
			void swapBuffers();
			GLFWwindow *window();

			double deltaTime();
			[[deprecated("Use avgFPS() for more stable, smoothed value.")]] int FPS();
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

			double m_lastFrameTime; ///< GLFW time of the previous @ref update(), in seconds.
			double m_deltaTime;     ///< Time between the last two @ref update() calls, in seconds.

			int m_frameCount;     ///< Frames counted in the current one-second window.
			double m_fpsLastTime; ///< GLFW time when the current one-second window started.
			int m_currentFPS;     ///< Frames counted in the last full second.
			double m_avgFPS;      ///< Smoothed FPS.
	};

} // namespace newton::graphics

#endif // GRAPHICS_CONTEXT_HPP
