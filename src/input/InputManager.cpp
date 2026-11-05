#include "../../include/input/InputManager.hpp"

#include <GLFW/glfw3.h>
#include <cstddef>

#include "../../include/common/Logger.hpp"
#include "../../include/input/utils.hpp"

namespace
{
	const char *cClassName = "Input Manager";

} // unnamed namespace

namespace newton::input
{
	common::Logger &InputManager::m_logger = common::Logger::getInstance();

	InputManager::KeyStateArray InputManager::m_currentKey;
	InputManager::KeyStateArray InputManager::m_previousKey;
	InputManager::ButtonStateArray InputManager::m_currentButton;
	InputManager::ButtonStateArray InputManager::m_previousButton;

	double InputManager::m_mouseLastX = 0.0;
	double InputManager::m_mouseLastY = 0.0;
	bool InputManager::m_firstMouse = true;
	double InputManager::m_mouseOffsetX;
	double InputManager::m_mouseOffsetY;
	double InputManager::m_scrollOffsetY;

	/**
	 * @brief Registers input callbacks on the window.
	 *
	 * Call once, after the window is created and before the main loop.
	 *
	 * @param window Window to receive input from.
	 */
	void InputManager::init(GLFWwindow *window)
	{
		m_logger.printf(common::LogLevel::INFO, "[%s] Initializing input manager\n", cClassName);

		// Keyboard
		glfwSetKeyCallback(window, keyCallback);

		// Mouse
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		glfwSetMouseButtonCallback(window, mouseButtonCallback);
		glfwSetCursorPosCallback(window, cursorPosCallback);
		glfwSetScrollCallback(window, scrollCallback);

		m_logger.printf(common::LogLevel::INFO, "[%s] Input manager initialized successfully\n", cClassName);
	}

	/**
	 * @brief Ends the input frame.
	 *
	 * Stores the current key and button state as previous and resets mouse offsets.
	 * Call once per frame, after everything that reads input.
	 */
	void InputManager::update()
	{
		m_previousKey = m_currentKey;
		m_previousButton = m_currentButton;
		resetMouseOffsets();
	}

	/**
	 * @brief GLFW key callback, updates the current key state.
	 */
	void InputManager::keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
	{
		(void) window;
		(void) mods;

		if (key < 0 || key > GLFW_KEY_LAST) {
			m_logger.printf(common::LogLevel::DEBUG, "[%s] Unknown key, scancode: [%d]\n", cClassName, scancode);
			return;
		}

		const auto index = static_cast<std::size_t>(key);

		if (action == GLFW_PRESS) {
			m_currentKey[index] = true;
			m_logger.printf(common::LogLevel::DEBUG, "[%s] Key pressed: [%s] [%d]\n", cClassName,
			                keyToString(key).c_str(), key);
		}

		if (action == GLFW_RELEASE) {
			m_currentKey[index] = false;
			m_logger.printf(common::LogLevel::DEBUG, "[%s] Key released: [%s] [%d]\n", cClassName,
			                keyToString(key).c_str(), key);
		}
	}

	/**
	 * @brief GLFW mouse button callback, updates the current button state.
	 */
	void InputManager::mouseButtonCallback(GLFWwindow *window, int button, int action, int mods)
	{
		(void) window;
		(void) mods;

		if (button < GLFW_MOUSE_BUTTON_1 || button > GLFW_MOUSE_BUTTON_LAST) {
			m_logger.printf(common::LogLevel::WARN, "[%s] Unknown mouse button: [%d]\n", cClassName, button);
			return;
		}

		const auto index = static_cast<std::size_t>(button);

		if (action == GLFW_PRESS) {
			m_currentButton[index] = true;
			m_logger.printf(common::LogLevel::DEBUG, "[%s] Button pressed: [%s] [%d]\n", cClassName,
			                buttonToString(button).c_str(), button);
		}

		if (action == GLFW_RELEASE) {
			m_currentButton[index] = false;
			m_logger.printf(common::LogLevel::DEBUG, "[%s] Button released: [%s] [%d]\n", cClassName,
			                buttonToString(button).c_str(), button);
		}
	}

	/**
	 * @brief GLFW cursor callback, accumulates cursor movement.
	 */
	void InputManager::cursorPosCallback(GLFWwindow *window, double xpos, double ypos)
	{
		(void) window;

		if (m_firstMouse) {
			m_mouseLastX = xpos;
			m_mouseLastY = ypos;
			m_firstMouse = false;
		}

		m_mouseOffsetX += xpos - m_mouseLastX;
		m_mouseOffsetY += ypos - m_mouseLastY;

		m_mouseLastX = xpos;
		m_mouseLastY = ypos;
	}

	/**
	 * @brief GLFW scroll callback, accumulates vertical scroll.
	 */
	void InputManager::scrollCallback(GLFWwindow *window, double xoffset, double yoffset)
	{
		(void) window;
		(void) xoffset;

		m_scrollOffsetY += yoffset;
	}

	/**
	 * @brief Checks if the key went down this frame.
	 * @param key GLFW key code (GLFW_KEY_*).
	 * @return True only in the frame of the press. False for out-of-range codes.
	 */
	bool InputManager::isKeyPressed(int key)
	{
		if (key < 0 || key > GLFW_KEY_LAST)
			return false;

		const auto index = static_cast<std::size_t>(key);

		return m_currentKey[index] && !m_previousKey[index];
	}

	/**
	 * @brief Checks if the key is down.
	 * @param key GLFW key code (GLFW_KEY_*).
	 * @return True in every frame while the key is down, including the press frame.
	 */
	bool InputManager::isKeyHeld(int key)
	{
		if (key < 0 || key > GLFW_KEY_LAST)
			return false;
		return m_currentKey[static_cast<std::size_t>(key)];
	}

	/**
	 * @brief Checks if the key went up this frame.
	 * @param key GLFW key code (GLFW_KEY_*).
	 * @return True only in the frame of the release. False for out-of-range codes.
	 */
	bool InputManager::isKeyReleased(int key)
	{
		if (key < 0 || key > GLFW_KEY_LAST)
			return false;

		const auto index = static_cast<std::size_t>(key);

		return !m_currentKey[index] && m_previousKey[index];
	}

	/**
	 * @brief Checks if the mouse button went down this frame.
	 * @param button GLFW mouse button code (GLFW_MOUSE_BUTTON_*).
	 * @return True only in the frame of the press. False for out-of-range codes.
	 */
	bool InputManager::isButtonPressed(int button)
	{
		if (button < GLFW_MOUSE_BUTTON_1 || button > GLFW_MOUSE_BUTTON_LAST)
			return false;

		const auto index = static_cast<std::size_t>(button);

		return m_currentButton[index] && !m_previousButton[index];
	}

	/**
	 * @brief Checks if the mouse button is down.
	 * @param button GLFW mouse button code (GLFW_MOUSE_BUTTON_*).
	 * @return True in every frame while the button is down, including the press frame.
	 */
	bool InputManager::isButtonHeld(int button)
	{
		if (button < GLFW_MOUSE_BUTTON_1 || button > GLFW_MOUSE_BUTTON_LAST)
			return false;

		const auto index = static_cast<std::size_t>(button);

		return m_currentButton[index];
	}

	/**
	 * @brief Checks if the mouse button went up this frame.
	 * @param button GLFW mouse button code (GLFW_MOUSE_BUTTON_*).
	 * @return True only in the frame of the release. False for out-of-range codes.
	 */
	bool InputManager::isButtonReleased(int button)
	{
		if (button < GLFW_MOUSE_BUTTON_1 || button > GLFW_MOUSE_BUTTON_LAST)
			return false;

		const auto index = static_cast<std::size_t>(button);

		return !m_currentButton[index] && m_previousButton[index];
	}

	/**
	 * @return Horizontal cursor movement since the last @ref update(), positive to the right.
	 */
	double InputManager::getMouseOffsetX()
	{
		return m_mouseOffsetX;
	}

	/**
	 * @return Vertical cursor movement since the last @ref update(), positive downwards (screen coordinates).
	 */
	double InputManager::getMouseOffsetY()
	{
		return m_mouseOffsetY;
	}

	/**
	 * @return Vertical scroll since the last @ref update(), positive when scrolling up.
	 */
	double InputManager::getScrollOffsetY()
	{
		return m_scrollOffsetY;
	}

	/**
	 * @brief Clears cursor and scroll offsets accumulated during the frame.
	 */
	void InputManager::resetMouseOffsets()
	{
		m_mouseOffsetX = 0.0;
		m_mouseOffsetY = 0.0;
		m_scrollOffsetY = 0.0;
	}

} // namespace newton::input
