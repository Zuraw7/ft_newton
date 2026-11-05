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

	void InputManager::update()
	{
		m_previousKey = m_currentKey;
		m_previousButton = m_currentButton;
		resetMouseOffsets();
	}

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

	void InputManager::scrollCallback(GLFWwindow *window, double xoffset, double yoffset)
	{
		(void) window;
		(void) xoffset;

		m_scrollOffsetY += yoffset;
	}

	bool InputManager::isKeyPressed(int key)
	{
		if (key < 0 || key > GLFW_KEY_LAST)
			return false;

		const auto index = static_cast<std::size_t>(key);

		return m_currentKey[index] && !m_previousKey[index];
	}

	bool InputManager::isKeyHeld(int key)
	{
		if (key < 0 || key > GLFW_KEY_LAST)
			return false;
		return m_currentKey[static_cast<std::size_t>(key)];
	}

	bool InputManager::isKeyReleased(int key)
	{
		if (key < 0 || key > GLFW_KEY_LAST)
			return false;

		const auto index = static_cast<std::size_t>(key);

		return !m_currentKey[index] && m_previousKey[index];
	}

	bool InputManager::isButtonPressed(int button)
	{
		if (button < GLFW_MOUSE_BUTTON_1 || button > GLFW_MOUSE_BUTTON_LAST)
			return false;

		const auto index = static_cast<std::size_t>(button);

		return m_currentButton[index] && !m_previousButton[index];
	}

	bool InputManager::isButtonHeld(int button)
	{
		if (button < GLFW_MOUSE_BUTTON_1 || button > GLFW_MOUSE_BUTTON_LAST)
			return false;

		const auto index = static_cast<std::size_t>(button);

		return m_currentButton[index];
	}

	bool InputManager::isButtonReleased(int button)
	{
		if (button < GLFW_MOUSE_BUTTON_1 || button > GLFW_MOUSE_BUTTON_LAST)
			return false;

		const auto index = static_cast<std::size_t>(button);

		return !m_currentButton[index] && m_previousButton[index];
	}

	double InputManager::getMouseOffsetX()
	{
		return m_mouseOffsetX;
	}

	double InputManager::getMouseOffsetY()
	{
		return m_mouseOffsetY;
	}

	double InputManager::getScrollOffsetY()
	{
		return m_scrollOffsetY;
	}

	void InputManager::resetMouseOffsets()
	{
		m_mouseOffsetX = 0.0;
		m_mouseOffsetY = 0.0;
		m_scrollOffsetY = 0.0;
	}

} // namespace newton::input
