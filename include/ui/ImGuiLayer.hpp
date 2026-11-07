#ifndef UI_IMGUILAYER_HPP
#define UI_IMGUILAYER_HPP

#include <GLFW/glfw3.h>

namespace newton::ui
{
	class ImGuiLayer
	{
		public:
			explicit ImGuiLayer(GLFWwindow *window);
			~ImGuiLayer();

			ImGuiLayer(const ImGuiLayer &) = delete;
			ImGuiLayer &operator=(const ImGuiLayer &) = delete;

			void beginFrame();
			void endFrame();
	};

} // namespace newton::ui

#endif // UI_IMGUILAYER_HPP
