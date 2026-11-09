#ifndef UI_IMGUILAYER_HPP
#define UI_IMGUILAYER_HPP

#include <GLFW/glfw3.h>

namespace newton::ui
{
	/**
	 * @brief Owns the Dear ImGui context and its GLFW and OpenGL 3 backends.
	 *
	 * Handles only the ImGui lifetime and frame, the windows themselves are drawn by separate functions
	 * between @ref beginFrame() and @ref endFrame(). ImGui has one global context, so only one instance
	 * may exist at a time. Must be destroyed before the window and OpenGL context it was created with.
	 *
	 * Window layout is not saved between runs (no imgui.ini), and ImGui never changes the OS cursor,
	 * cursor modes are controlled only by @ref input::InputManager. Non-copyable.
	 */
	class ImGuiLayer
	{
		public:
			explicit ImGuiLayer(GLFWwindow *window);
			~ImGuiLayer();

			ImGuiLayer(const ImGuiLayer &) = delete;
			ImGuiLayer &operator=(const ImGuiLayer &) = delete;

			void beginFrame();
			void endFrame();

			bool wantsMouse() const;
			bool wantsKeyboard() const;
	};

} // namespace newton::ui

#endif // UI_IMGUILAYER_HPP
