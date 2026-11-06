#include "controllers/CameraController.hpp"

#include <GLFW/glfw3.h>

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
	 * Movement and rotation work only while the right mouse button is held.
	 * The cursor is disabled on press and restored on release. Zoom is always active.
	 *
	 * Must be called after events are polled and before @ref input::InputManager::update(),
	 * which resets the per-frame input state (mouse offsets, pressed keys).
	 *
	 * @param deltaTime Time since the last frame, in seconds. Scales movement only;
	 *                  rotation and zoom use raw mouse offsets.
	 */
	void CameraController::update(const double deltaTime)
	{
		using input::InputManager;
		constexpr int RMB = GLFW_MOUSE_BUTTON_RIGHT;

		if (InputManager::isButtonPressed(RMB))
			InputManager::setCursorDisabled(true);
		else if (InputManager::isButtonReleased(RMB))
			InputManager::setCursorDisabled(false);

		if (InputManager::isButtonHeld(RMB)) {
			moveCamera(deltaTime);
			rotateCamera();
		}

		zoomCamera();
	}

	/**
	 * @brief Moves the camera according to held movement keys. Ctrl doubles the speed.
	 *
	 * @param deltaTime Time since the last frame, in seconds. Keeps speed independent of FPS.
	 */
	void CameraController::moveCamera(double deltaTime)
	{
		float multiplier = input::InputManager::isKeyHeld(GLFW_KEY_LEFT_CONTROL) ||
		                           input::InputManager::isKeyHeld(GLFW_KEY_RIGHT_CONTROL)
		                       ? 2.0f
		                       : 1.0f;

		if (input::InputManager::isKeyHeld(GLFW_KEY_W))
			m_camera->updateCameraPos(graphics::CameraDirection::FORWARD, deltaTime, multiplier);
		if (input::InputManager::isKeyHeld(GLFW_KEY_S))
			m_camera->updateCameraPos(graphics::CameraDirection::BACKWARD, deltaTime, multiplier);
		if (input::InputManager::isKeyHeld(GLFW_KEY_A))
			m_camera->updateCameraPos(graphics::CameraDirection::LEFT, deltaTime, multiplier);
		if (input::InputManager::isKeyHeld(GLFW_KEY_D))
			m_camera->updateCameraPos(graphics::CameraDirection::RIGHT, deltaTime, multiplier);
		if (input::InputManager::isKeyHeld(GLFW_KEY_SPACE))
			m_camera->updateCameraPos(graphics::CameraDirection::UP, deltaTime, multiplier);
		if (input::InputManager::isKeyHeld(GLFW_KEY_LEFT_SHIFT))
			m_camera->updateCameraPos(graphics::CameraDirection::DOWN, deltaTime, multiplier);
	}

	/**
	 * @brief Rotates the camera using mouse offsets accumulated this frame.
	 *
	 * Offsets are distances, not speeds, so they are not scaled by delta time.
	 * The camera is only updated when there was actual movement.
	 */
	void CameraController::rotateCamera()
	{
		const double x = input::InputManager::getMouseOffsetX();
		const double y = input::InputManager::getMouseOffsetY();
		if (x != 0 || y != 0)
			m_camera->updateCameraDirection(x, y);
	}

	/**
	 * @brief Zooms the camera using the scroll offset accumulated this frame.
	 *
	 * The offset is a distance, not a speed, so it is not scaled by delta time.
	 * The camera is only updated when there was actual scrolling.
	 */
	void CameraController::zoomCamera()
	{
		double scrollOffsetY = input::InputManager::getScrollOffsetY();

		if (scrollOffsetY != 0)
			m_camera->updateCameraZoom(scrollOffsetY);
	}

} // namespace newton::controllers
