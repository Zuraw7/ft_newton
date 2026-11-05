#ifndef INPUT_MANAGER_HPP
#define INPUT_MANAGER_HPP

#include <GLFW/glfw3.h>

#include <array>

#include "../../include/common/Logger.hpp"

namespace newton::input
{
	class InputManager
	{
		public:
			InputManager() = delete;

			// Management
			static void init(GLFWwindow *window);
			static void update();

			// Callbacks
			static void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);

			static void mouseButtonCallback(GLFWwindow *window, int button, int action, int mods);
			static void cursorPosCallback(GLFWwindow *window, double xpos, double ypos);
			static void scrollCallback(GLFWwindow *window, double xoffset, double yoffset);

			// Key state
			static bool isKeyPressed(int key);
			static bool isKeyHeld(int key);
			static bool isKeyReleased(int key);

			// Button state
			static bool isButtonPressed(int button);
			static bool isButtonHeld(int button);
			static bool isButtonReleased(int button);

			// Mouse getters
			static double getMouseOffsetX();
			static double getMouseOffsetY();
			static double getScrollOffsetY();

		private:
			static void resetMouseOffsets();

			static common::Logger &m_logger;

			// Keyboard state
			using KeyStateArray = std::array<bool, GLFW_KEY_LAST + 1>;
			static KeyStateArray m_currentKey;
			static KeyStateArray m_previousKey;

			// Mouse state
			using ButtonStateArray = std::array<bool, GLFW_MOUSE_BUTTON_LAST + 1>;
			static ButtonStateArray m_currentButton;
			static ButtonStateArray m_previousButton;

			static double m_mouseLastX, m_mouseLastY;
			static bool m_firstMouse;
			static double m_mouseOffsetX, m_mouseOffsetY;
			static double m_scrollOffsetY;
	};

} // namespace newton::input

#endif // INPUT_MANAGER_HPP
