#include "../../include/controllers/CameraController.hpp"

#include "../../include/common/Logger.hpp"
#include "../../include/graphics/Camera.hpp"
#include "../../include/input/InputManager.hpp"

namespace
{
	const char *cClassName = "CameraController";

} // unnamed namespace

namespace newton::controllers
{
	CameraController::CameraController(graphics::Camera &camera)
		: m_camera(&camera)
		, m_logger(common::Logger::getInstance())
	{
		m_logger.printf(common::LogLevel::INFO, "[%s] Camera controller initialized successfully\n", cClassName);
	}

	void CameraController::switchCamera(graphics::Camera &camera)
	{
		if (&camera == m_camera) {
			m_logger.printf(common::LogLevel::DEBUG, "[%s] Given camera is already bound\n", cClassName);
			return;
		}

		m_camera = &camera;

		m_logger.printf(common::LogLevel::INFO, "[%s] Camera switched successfully\n", cClassName);
	}

	// Must be called after glfwPollEvents and before InputManager::update,
	// which resets the per-frame input state
	void CameraController::update(const double deltaTime)
	{
		moveCamera(deltaTime);
		rotateCamera();
	}

	void CameraController::moveCamera(double deltaTime)
	{
		if (input::InputManager::isKeyHeld(GLFW_KEY_W))
			m_camera->updateCameraPos(graphics::CameraDirection::FORWARD, deltaTime);
		if (input::InputManager::isKeyHeld(GLFW_KEY_S))
			m_camera->updateCameraPos(graphics::CameraDirection::BACKWARD, deltaTime);
		if (input::InputManager::isKeyHeld(GLFW_KEY_A))
			m_camera->updateCameraPos(graphics::CameraDirection::LEFT, deltaTime);
		if (input::InputManager::isKeyHeld(GLFW_KEY_D))
			m_camera->updateCameraPos(graphics::CameraDirection::RIGHT, deltaTime);
		if (input::InputManager::isKeyHeld(GLFW_KEY_SPACE))
			m_camera->updateCameraPos(graphics::CameraDirection::UP, deltaTime);
		if (input::InputManager::isKeyHeld(GLFW_KEY_LEFT_SHIFT))
			m_camera->updateCameraPos(graphics::CameraDirection::DOWN, deltaTime);
	}

	void CameraController::rotateCamera()
	{
		const double x = input::InputManager::getMouseOffsetX();
		const double y = input::InputManager::getMouseOffsetY();
		if (x != 0 || y != 0)
			m_camera->updateCameraDirection(x, y);
		if (input::InputManager::getScrollOffsetY() != 0)
			m_camera->updateCameraZoom(input::InputManager::getScrollOffsetY());
	}

} // namespace newton::controllers
