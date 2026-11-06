#ifndef INPUT_MANAGER_HPP
#define INPUT_MANAGER_HPP

#include <GLFW/glfw3.h>

#include <array>

#include "common/Logger.hpp"

namespace newton::input
{
	/**
	 * @brief Static store of keyboard and mouse state, fed by GLFW callbacks.
	 *
	 * Per-frame contract:
	 * 1. GLFW callbacks update the current state during event polling.
	 * 2. Game code queries the state (@ref isKeyPressed(), @ref getMouseOffsetX(), ...).
	 * 3. @ref update() ends the input frame - must be called after all queries.
	 *
	 * Pressed and released are true only in the single frame the state changed,
	 * held is true in every frame while the key or button is down.
	 */
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

			// Mouse setters
			static void setCursorDisabled(bool disabled);

		private:
			static void resetMouseOffsets();

			static common::Logger &m_logger;
			static GLFWwindow *m_window;

			// Keyboard state
			using KeyStateArray = std::array<bool, GLFW_KEY_LAST + 1>;
			static KeyStateArray m_currentKey;  ///< Key state from the latest polled events.
			static KeyStateArray m_previousKey; ///< Key state from the previous frame.

			// Mouse state
			using ButtonStateArray = std::array<bool, GLFW_MOUSE_BUTTON_LAST + 1>;
			static ButtonStateArray m_currentButton;  ///< Button state from the latest polled events.
			static ButtonStateArray m_previousButton; ///< Button state from the previous frame.

			static double m_mouseLastX;    ///< Cursor X from the last cursor event.
			static double m_mouseLastY;    ///< Cursor Y from the last cursor event.
			static bool m_firstMouse;      ///< Skips the jump at start and after a cursor mode change.
			static double m_mouseOffsetX;  ///< Cursor X movement accumulated since the last @ref update().
			static double m_mouseOffsetY;  ///< Cursor Y movement accumulated since the last @ref update().
			static double m_scrollOffsetY; ///< Vertical scroll accumulated since the last @ref update().
	};

} // namespace newton::input

#endif // INPUT_MANAGER_HPP
