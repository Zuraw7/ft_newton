#ifndef GRAPHICS_CAMERA_HPP
#define GRAPHICS_CAMERA_HPP

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

#include "common/Logger.hpp"

namespace newton::graphics
{

	/**
	 * @brief Movement direction.
	 */
	enum CameraDirection {
		NONE = 0,
		FORWARD,
		BACKWARD,
		LEFT,
		RIGHT,
		UP,
		DOWN
	};

	/**
	 * @brief Free-flying perspective camera controlled by @ref m_yaw and @ref m_pitch.
	 *
	 * Orientation is stored as yaw and pitch angles, from which the direction,
	 * right and up vectors are derived. Provides view and projection matrices for rendering.
	 * Non-copyable and non-movable.
	 */
	class Camera
	{
		public:
			Camera() = delete;
			Camera(const glm::vec3 &position, const glm::vec3 &target, const glm::vec3 &worldUp);
			Camera(Camera &) = delete;
			Camera(Camera &&) = delete;
			~Camera() = default;

			Camera &operator=(const Camera &) = delete;
			Camera &operator=(const Camera &&) = delete;

			const glm::mat4 &getCamView();
			const glm::mat4 &getCamProjection() const;
			const glm::vec3 &getPosition() const;

			void updateCameraDirection(double dx, double dy);
			void updateCameraPos(CameraDirection dir, double deltaTime, float multiplier);
			void updateCameraZoom(double dy);
			void updateCameraVectors();

			void setCameraPosition(glm::vec3 newPos);

		private:
			glm::mat4 lookAt() const;

			common::Logger &m_logger;

			// Vectors
			glm::vec3 m_camPosition;
			[[maybe_unused]] glm::vec3 m_targetPos; ///< Initial direction comes from @ref m_yaw and @ref m_pitch.
			glm::vec3 m_worldUp; ///< World up axis, used for vertical movement and to build the camera axes.

			glm::vec3 m_camDirection; ///< Normalized forward vector, derived from @ref m_yaw and @ref m_pitch.
			glm::vec3 m_camRight;     ///< Normalized right vector, derived from @ref m_yaw and @ref m_pitch.
			glm::vec3 m_camUp;        ///< Normalized up vector, derived from @ref m_yaw and @ref m_pitch.

			// Matrices
			glm::mat4 m_view;       ///< Cached view matrix, refreshed in @ref getCamView().
			glm::mat4 m_projection; ///< Projection matrix, refreshed on zoom.

			float m_yaw;   ///< Horizontal angle in degrees, -90 looks along -Z.
			float m_pitch; ///< Vertical angle in degrees, clamped to [-89, 89].
			float m_speed; ///< Movement speed in world units per second.
			float m_fov;   ///< Vertical field of view in degrees.
	};

} // namespace newton::graphics

#endif // GRAPHICS_CAMERA_HPP
