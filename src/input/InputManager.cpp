#include "../../include/input/InputManager.hpp"

#include <GLFW/glfw3.h>
#include <cstddef>

#include "../../include/input/utils.hpp"
#include "../../include/common/Logger.hpp"

namespace {
    const char *cClassName = "Input Manager";
}

namespace newton::input
{
        common::Logger& InputManager::m_logger = common::Logger::getInstance();

        std::array<bool, GLFW_KEY_LAST + 1> InputManager::m_currentKey;
        std::array<bool, GLFW_KEY_LAST + 1> InputManager::m_previousKey;

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
            glfwSetCursorPosCallback(window, cursorPosCallback);
            glfwSetScrollCallback(window, scrollCallback);

		    m_logger.printf(common::LogLevel::INFO, "[%s] Input manager initialized successfully\n", cClassName);
        }

        void InputManager::processInput(GLFWwindow *window, double deltaTime)
        {
            if (isKeyPressed(GLFW_KEY_ESCAPE))
                glfwSetWindowShouldClose(window, true);
            moveCamera(window, deltaTime);
            rotateCamera(window);
            polygonModeManagement(window);
            update();
        }

        void InputManager::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
        {
            (void) window;
            (void) scancode;
            (void) mods;

            if (key < 0 || key > GLFW_KEY_LAST)
                return;

            const auto index = static_cast<std::size_t>(key);

            if (action == GLFW_PRESS) {
                m_currentKey[index] = true;
                m_logger.printf(common::LogLevel::INFO, "[%s] Key pressed: [%s] [%d]\n", cClassName, keyToString(key).c_str(), key);
            }

            if (action == GLFW_RELEASE) {
                m_currentKey[index] = false;
                m_logger.printf(common::LogLevel::INFO, "[%s] Key released: [%s] [%d]\n", cClassName, keyToString(key).c_str(), key);
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

            m_mouseOffsetX = xpos - m_mouseLastX;
            m_mouseOffsetY = ypos - m_mouseLastY;

            m_mouseLastX = xpos;
            m_mouseLastY = ypos;
        }

        void InputManager::scrollCallback(GLFWwindow *window, double xoffset, double yoffset)
        {
            (void) window;
            (void) xoffset;

            m_scrollOffsetY = yoffset;
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

        void InputManager::update()
        {
            m_previousKey = m_currentKey;
            resetMouseOffsets();
        }

        void InputManager::resetMouseOffsets()
        {
            m_mouseOffsetX = 0.0;
            m_mouseOffsetY = 0.0;
            m_scrollOffsetY = 0.0;
        }

} // namespace newton::input
