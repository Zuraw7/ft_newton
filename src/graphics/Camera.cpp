#include "../../include/graphics/Camera.hpp"

#include <algorithm>
#include <cmath>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>

#include "../../include/common/Logger.hpp"
#include "../../include/graphics/resolution.hpp"

namespace
{
	const char *cClassName = "Camera";
	constexpr float NEAR = 0.1f;
	constexpr float FAR = 450.0f;
	constexpr float SENSITIVITY = 0.1f;

} // unnamed namespace

namespace newton::graphics
{
	/**
	 * @brief Creates a camera looking along -Z.
	 * @param position Starting position in world space.
	 * @param target Initial direction comes from the default @ref m_yaw and @ref m_pitch.
	 * @param worldUp World up axis, expected (0, 1, 0). @ref m_yaw and @ref m_pitch assume a Y-up world.
	 *                A zero vector or one parallel to the view direction makes the camera axes NaN.
	 */
	Camera::Camera(const glm::vec3 &position, const glm::vec3 &target, const glm::vec3 &worldUp)
		: m_logger(common::Logger::getInstance())
		, m_camPosition(position)
		, m_targetPos(target)
		, m_worldUp(worldUp)
		, m_yaw(-90.0f)
		, m_pitch(0.0f)
		, m_speed(20.0f)
		, m_fov(60.0f)
	{
		m_logger.printf(common::LogLevel::INFO, "[%s] Initializing camera\n", cClassName);

		updateCameraVectors();
		m_view = lookAt();
		m_projection = glm::perspective(glm::radians(m_fov), Resolution::aspect, NEAR, FAR);

		m_logger.printf(common::LogLevel::INFO, "[%s] Camera initialized successfully\n", cClassName);
	}

	/**
	 * @brief Recalculates and returns the view matrix.
	 *
	 * Cached matrix is refreshed on every call.
	 */
	const glm::mat4 &Camera::getCamView()
	{
		m_view = lookAt();
		return m_view;
	}

	/**
	 * @return Perspective projection with a fixed aspect ratio.
	 */
	const glm::mat4 &Camera::getCamProjection() const
	{
		return m_projection;
	}

	const glm::vec3 &Camera::getPosition() const
	{
		return m_camPosition;
	}

	/**
	 * @brief Rotates the camera by a mouse offset.
	 *
	 * Pitch is clamped to [-89, 89] degrees.
	 *
	 * @param dx Horizontal offset in pixels, positive turns right.
	 * @param dy Vertical offset in pixels, positive looks down (screen coordinates).
	 */
	void Camera::updateCameraDirection(double dx, double dy)
	{
		m_yaw += static_cast<float>(dx) * SENSITIVITY;
		m_pitch -= static_cast<float>(dy) * SENSITIVITY;

		if (m_pitch > 89.0f)
			m_pitch = 89.0f;
		else if (m_pitch < -89.0f)
			m_pitch = -89.0f;
		updateCameraVectors();
	}

	/**
	 * @brief Moves the camera in the given direction.
	 *
	 * Forward and backward follow the pitch, so the camera flies where it looks.
	 *
	 * @param dir Direction of movement.
	 * @param deltaTime Time since the last frame, in seconds.
	 */
	void Camera::updateCameraPos(const CameraDirection dir, const double deltaTime)
	{
		float velocity = static_cast<float>(deltaTime) * m_speed;

		switch (dir) {
			case CameraDirection::FORWARD : m_camPosition += m_camDirection * velocity; break;
			case CameraDirection::BACKWARD : m_camPosition -= m_camDirection * velocity; break;
			case CameraDirection::RIGHT : m_camPosition += m_camRight * velocity; break;
			case CameraDirection::LEFT : m_camPosition -= m_camRight * velocity; break;
			case CameraDirection::UP : m_camPosition += m_worldUp * velocity; break;
			case CameraDirection::DOWN : m_camPosition -= m_worldUp * velocity; break;
			case CameraDirection::NONE : break;
		}
	}

	/**
	 * @brief Zooms by changing the vertical FOV, clamped to [20, 90] degrees.
	 * @param dy Scroll offset, positive zooms in.
	 */
	void Camera::updateCameraZoom(double dy)
	{
		m_fov -= static_cast<float>(dy);
		m_fov = std::clamp(m_fov, 20.0f, 90.0f);

		m_projection = glm::perspective(glm::radians(m_fov), Resolution::aspect, NEAR, FAR);
	}

	/**
	 * @brief Recalculates @ref m_camDirection, @ref m_camRight and @ref m_camUp from @ref m_yaw and @ref m_pitch.
	 */
	void Camera::updateCameraVectors()
	{
		glm::vec3 direction;
		direction.x = std::cos(glm::radians(m_yaw)) * std::cos(glm::radians(m_pitch));
		direction.y = std::sin(glm::radians(m_pitch));
		direction.z = std::sin(glm::radians(m_yaw)) * std::cos(glm::radians(m_pitch));

		m_camDirection = glm::normalize(direction);
		m_camRight = glm::normalize(glm::cross(m_camDirection, m_worldUp));
		m_camUp = glm::normalize(glm::cross(m_camRight, m_camDirection));
	}

	void Camera::setCameraPosition(glm::vec3 newPos)
	{
		m_camPosition = newPos;
	}

	/**
	 * @brief Builds the view matrix from the current position and direction.
	 */
	glm::mat4 Camera::lookAt() const
	{
		return glm::lookAt(m_camPosition, m_camPosition + m_camDirection, m_worldUp);
	}

} // namespace newton::graphics
