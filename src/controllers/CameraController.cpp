#include "controllers/CameraController.hpp"

#include <GLFW/glfw3.h>

#include <algorithm>
#include <glm/geometric.hpp>
#include <glm/vec3.hpp>

#include "common/Logger.hpp"
#include "graphics/Camera.hpp"
#include "input/InputManager.hpp"

namespace
{
	const char *cClassName = "CameraController";
	constexpr float MIN_SPEED = 1.0f;
	constexpr float MIN_FAST_MULTIPLIER = 1.0f;
	constexpr float MIN_SENSITIVITY = 0.01f;
	constexpr float MIN_ZOOM_SENSITIVITY = 0.1f;

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
		, m_settings()
		, m_active(false)
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
	 * Movement and rotation work during a right mouse button drag. The drag
	 * starts only when the button is pressed while the mouse is not blocked,
	 * and then lasts until release. The cursor is disabled for the drag and
	 * restored on release. Zoom works whenever the mouse is not blocked.
	 *
	 * Must be called after events are polled and before @ref input::InputManager::update(),
	 * which resets the per-frame input state (mouse offsets, pressed keys).
	 *
	 * @param deltaTime Time since the last frame, in seconds. Scales movement only;
	 *                  rotation and zoom use raw mouse offsets.
	 * @param mouseBlocked True if the mouse is used by something else this frame, e.g. hovering a UI window
	 *                     (@ref ui::ImGuiLayer::wantsMouse()).
	 */
	void CameraController::update(double deltaTime, bool mouseBlocked)
	{
		using input::InputManager;
		constexpr int RMB = GLFW_MOUSE_BUTTON_RIGHT;

		if (InputManager::isButtonPressed(RMB) && !mouseBlocked) {
			m_active = true;
			InputManager::setCursorDisabled(true);
		} else if (InputManager::isButtonReleased(RMB) && m_active) {
			m_active = false;
			InputManager::setCursorDisabled(false);
		}

		if (m_active) {
			moveCamera(deltaTime);
			rotateCamera();
		}

		if (!mouseBlocked)
			zoomCamera();
	}

	/**
	 * @brief Replaces the movement parameters. They take effect on the next @ref update().
	 *
	 * Values below the minimums are raised to them, so movement, rotation and zoom cannot stop or get reversed:
	 * speed 1, fast multiplier 1, sensitivity 0.01, zoom sensitivity 0.1.
	 *
	 * @param settings New parameters, `{}` restores the defaults.
	 */
	void CameraController::setSettings(const CameraController::Settings &settings)
	{
		m_settings.speed = std::max(settings.speed, MIN_SPEED);
		m_settings.fastMultiplier = std::max(settings.fastMultiplier, MIN_FAST_MULTIPLIER);
		m_settings.sensitivity = std::max(settings.sensitivity, MIN_SENSITIVITY);
		m_settings.zoomSensitivity = std::max(settings.zoomSensitivity, MIN_ZOOM_SENSITIVITY);
	}

	const CameraController::Settings &CameraController::settings() const
	{
		return m_settings;
	}

	/**
	 * @return Vertical field of view of the bound camera, in degrees.
	 */
	float CameraController::fov() const
	{
		return m_camera->getFOV();
	}

	/**
	 * @brief Moves the camera according to held movement keys.
	 *
	 * Directions are summed and normalized. Ctrl multiplies the speed by
	 * @ref Settings::fastMultiplier.
	 *
	 * @param deltaTime Time since the last frame, in seconds. Keeps speed independent of FPS.
	 */
	void CameraController::moveCamera(double deltaTime)
	{
		using input::InputManager;

		bool fast = InputManager::isKeyHeld(GLFW_KEY_LEFT_CONTROL) || InputManager::isKeyHeld(GLFW_KEY_RIGHT_CONTROL);
		float multiplier = fast ? m_settings.fastMultiplier : 1.0f;

		glm::vec3 dir(0.0f);

		if (InputManager::isKeyHeld(GLFW_KEY_W))
			dir += m_camera->getForward();
		if (InputManager::isKeyHeld(GLFW_KEY_S))
			dir -= m_camera->getForward();
		if (InputManager::isKeyHeld(GLFW_KEY_A))
			dir -= m_camera->getRight();
		if (InputManager::isKeyHeld(GLFW_KEY_D))
			dir += m_camera->getRight();
		if (InputManager::isKeyHeld(GLFW_KEY_SPACE))
			dir += m_camera->getWorldUp();
		if (InputManager::isKeyHeld(GLFW_KEY_LEFT_SHIFT))
			dir -= m_camera->getWorldUp();

		if (glm::length(dir) > 0.0f)
			m_camera->move(glm::normalize(dir) * m_settings.speed * multiplier * static_cast<float>(deltaTime));
	}

	/**
	 * @brief Rotates the camera using mouse offsets accumulated this frame.
	 *
	 * Offsets are distances, not speeds, so they are not scaled by delta time.
	 * The camera is only updated when there was actual movement.
	 */
	void CameraController::rotateCamera()
	{
		const float dx = static_cast<float>(input::InputManager::getMouseOffsetX());
		const float dy = static_cast<float>(input::InputManager::getMouseOffsetY());

		if (dx != 0.0f || dy != 0.0f)
			m_camera->rotate(dx * m_settings.sensitivity, -dy * m_settings.sensitivity);
	}

	/**
	 * @brief Zooms the camera using the scroll offset accumulated this frame.
	 *
	 * The offset is a distance, not a speed, so it is not scaled by delta time.
	 * The camera is only updated when there was actual scrolling.
	 */
	void CameraController::zoomCamera()
	{
		const float scrollOffsetY = static_cast<float>(input::InputManager::getScrollOffsetY());

		if (scrollOffsetY != 0.0f)
			m_camera->zoom(-scrollOffsetY * m_settings.zoomSensitivity);
	}

} // namespace newton::controllers
