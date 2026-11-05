#include "controllers/CameraController.hpp"

#include "common/Logger.hpp"
#include "graphics/Camera.hpp"
#include "input/InputManager.hpp"

namespace
{
	const char *cClassName = "CameraController";

} // unnamed namespace

namespace newton::controllers
{
	/**
	 * @brief Creates a controller bound to the given camera.
	 * @param camera Camera to control. Must outlive this controller.
	 */
	CameraController::CameraController(graphics::Camera &camera)
		: m_camera(&camera)
		, m_logger(common::Logger::getInstance())
	{
		m_logger.printf(common::LogLevel::INFO, "[%s] Camera controller initialized successfully\n", cClassName);
	}

	/**
	 * @brief Binds the controller to another camera.
	 *
	 * @param camera Camera to control. Must outlive this controller.
	 */
	void CameraController::switchCamera(graphics::Camera &camera)
	{
		if (&camera == m_camera) {
			m_logger.printf(common::LogLevel::DEBUG, "[%s] Given camera is already bound\n", cClassName);
			return;
		}

		m_camera = &camera;

		m_logger.printf(common::LogLevel::INFO, "[%s] Camera switched successfully\n", cClassName);
	}

	/**
	 * @brief Applies this frame's input to the bound camera.
	 *
	 * Must be called after events are polled and before @ref input::InputManager::update(),
	 * which resets the per-frame input state (mouse offsets, pressed keys).
	 *
	 * @param deltaTime Time since the last frame, in seconds. Scales movement only;
	 *                  rotation and zoom use raw mouse offsets.
	 */
	void CameraController::update(const double deltaTime)
	{
		moveCamera(deltaTime);
		rotateCamera();
	}

	/**
	 * @brief Moves the camera according to held movement keys.
	 *
	 * @param deltaTime Time since the last frame, in seconds. Keeps speed independent of FPS.
	 */
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

	/**
	 * @brief Rotates and zooms the camera using mouse offsets accumulated this frame.
	 *
	 * Offsets are distances, not speeds, so they are not scaled by delta time.
	 * The camera is only updated when there was actual movement or scrolling.
	 */
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
