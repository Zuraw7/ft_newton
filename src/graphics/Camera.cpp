#include "graphics/Camera.hpp"

#include <algorithm>
#include <cmath>

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/geometric.hpp>
#include <glm/trigonometric.hpp>

#include "common/Logger.hpp"

namespace
{
	const char *cClassName = "Camera";
	constexpr float NEAR = 0.1f;
	constexpr float FAR = 450.0f;
	constexpr float MAX_PITCH = 89.0f;
	constexpr float DEFAULT_FOV = 60.0f;
	constexpr float MAX_FOV = 90.0f;
	constexpr float MIN_FOV = 10.0f;

} // unnamed namespace

namespace newton::graphics
{
	/**
	 * @brief Creates a camera at the given position, looking at the target.
	 * @param position Starting position in world space.
	 * @param target Point to look at. If equal to the position, the camera looks along -Z.
	 * @param worldUp World up axis, expected (0, 1, 0). @ref m_yaw and @ref m_pitch assume a Y-up world.
	 *                A zero vector or one parallel to the view direction makes the camera axes NaN.
	 */
	Camera::Camera(const glm::vec3 &position, const glm::vec3 &target, const glm::vec3 &worldUp)
		: m_logger(common::Logger::getInstance())
		, m_aspect(16.0f / 9.0f)
		, m_startingPos(position)
		, m_startingTarget(target)
		, m_position(position)
		, m_worldUp(glm::normalize(worldUp))
		, m_yaw(-90.0f)
		, m_pitch(0.0f)
		, m_fov(DEFAULT_FOV)
	{
		m_logger.printf(common::LogLevel::INFO, "[%s] Initializing camera\n", cClassName);

		updateVectors();
		lookAt(target);

		m_view = calculateView();
		updateProjection();

		m_logger.printf(common::LogLevel::INFO, "[%s] Camera initialized successfully\n", cClassName);
	}

	/**
	 * @brief Turns the camera towards a point by recalculating @ref m_yaw and @ref m_pitch.
	 *
	 * Pitch is clamped to [-89, 89] degrees, so a target straight above or below is looked at almost vertically.
	 *
	 * @param target Point in world space. Ignored if equal to the camera position.
	 */
	void Camera::lookAt(const glm::vec3 &target)
	{
		if (target == m_position)
			return;

		const glm::vec3 dir = glm::normalize(target - m_position);
		m_yaw = glm::degrees(std::atan2(dir.z, dir.x));
		m_pitch = glm::degrees(std::asin(dir.y));

		m_pitch = std::clamp(m_pitch, -MAX_PITCH, MAX_PITCH);
		updateVectors();
	}

	/**
	 * @brief Recalculates and returns the view matrix.
	 *
	 * Cached matrix is refreshed on every call.
	 */
	const glm::mat4 &Camera::getView()
	{
		m_view = calculateView();
		return m_view;
	}

	/**
	 * @return Perspective projection for the current FOV and aspect ratio.
	 */
	const glm::mat4 &Camera::getProjection() const
	{
		return m_projection;
	}

	const glm::vec3 &Camera::getPosition() const
	{
		return m_position;
	}

	const glm::vec3 &Camera::getForward() const
	{
		return m_direction;
	}

	const glm::vec3 &Camera::getRight() const
	{
		return m_right;
	}

	const glm::vec3 &Camera::getWorldUp() const
	{
		return m_worldUp;
	}

	/**
	 * @return Vertical field of view in degrees.
	 */
	float Camera::getFOV() const
	{
		return m_fov;
	}

	/**
	 * @brief Sets the aspect ratio of the projection. Starts as 16:9.
	 *
	 * Without it the image stretches when the window size changes.
	 *
	 * @param aspect Width divided by height, must be greater than 0.
	 */
	void Camera::setAspectRatio(float aspect)
	{
		if (m_aspect == aspect)
			return;

		m_aspect = aspect;
		updateProjection();
	}

	void Camera::setPosition(const glm::vec3 &position)
	{
		m_position = position;
	}

	void Camera::move(const glm::vec3 &offset)
	{
		m_position += offset;
	}

	/**
	 * @brief Adds the given angles to @ref m_yaw and @ref m_pitch.
	 *
	 * Pitch is clamped to [-89, 89] degrees.
	 *
	 * @param yawDegrees Horizontal rotation, positive turns right.
	 * @param pitchDegrees Vertical rotation, positive looks up.
	 */
	void Camera::rotate(float yawDegrees, float pitchDegrees)
	{
		m_yaw += yawDegrees;
		m_pitch += pitchDegrees;

		m_pitch = std::clamp(m_pitch, -MAX_PITCH, MAX_PITCH);
		updateVectors();
	}

	/**
	 * @brief Zooms by changing the vertical FOV, clamped to [10, 90] degrees.
	 * @param fovDelta Change of FOV in degrees, negative zooms in.
	 */
	void Camera::zoom(float fovDelta)
	{
		m_fov += fovDelta;
		m_fov = std::clamp(m_fov, MIN_FOV, MAX_FOV);

		updateProjection();
	}

	/**
	 * @brief Restores the position and direction from the constructor and the default FOV.
	 *
	 * The aspect ratio stays as it is, because it follows the window size. If the starting target equals
	 * the starting position, the direction does not change (see @ref lookAt()).
	 */
	void Camera::reset()
	{
		m_position = m_startingPos;
		lookAt(m_startingTarget);

		m_fov = DEFAULT_FOV;
		updateProjection();
	}

	/**
	 * @brief Builds the view matrix from the current position and direction.
	 */
	glm::mat4 Camera::calculateView() const
	{
		return glm::lookAt(m_position, m_position + m_direction, m_worldUp);
	}

	/**
	 * @brief Recalculates @ref m_direction, @ref m_right and @ref m_up from @ref m_yaw and @ref m_pitch.
	 */
	void Camera::updateVectors()
	{
		glm::vec3 direction;
		direction.x = std::cos(glm::radians(m_yaw)) * std::cos(glm::radians(m_pitch));
		direction.y = std::sin(glm::radians(m_pitch));
		direction.z = std::sin(glm::radians(m_yaw)) * std::cos(glm::radians(m_pitch));

		m_direction = glm::normalize(direction);
		m_right = glm::normalize(glm::cross(m_direction, m_worldUp));
		m_up = glm::normalize(glm::cross(m_right, m_direction));
	}

	/**
	 * @brief Recalculates @ref m_projection from @ref m_fov and @ref m_aspect.
	 */
	void Camera::updateProjection()
	{
		m_projection = glm::perspective(glm::radians(m_fov), m_aspect, NEAR, FAR);
	}

} // namespace newton::graphics
