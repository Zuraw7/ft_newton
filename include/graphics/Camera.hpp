#ifndef GRAPHICS_CAMERA_HPP
#define GRAPHICS_CAMERA_HPP

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

#include "common/Logger.hpp"

namespace newton::graphics
{

	/**
	 * @brief Free-flying perspective camera controlled by @ref m_yaw and @ref m_pitch.
	 *
	 * Orientation is stored as yaw and pitch angles, from which the direction,
	 * right and up vectors are derived. Provides view and projection matrices for rendering.
	 *
	 * The camera only stores the view state.
	 *
	 * Non-copyable and non-movable.
	 */
	class Camera
	{
		public:
			Camera(const glm::vec3 &position, const glm::vec3 &target, const glm::vec3 &worldUp);
			~Camera() = default;

			Camera() = delete;
			Camera(const Camera &) = delete;
			Camera(Camera &&) = delete;

			Camera &operator=(const Camera &) = delete;
			Camera &operator=(Camera &&) = delete;

			void lookAt(const glm::vec3 &target);

			const glm::mat4 &getView();
			const glm::mat4 &getProjection() const;
			const glm::vec3 &getPosition() const;
			const glm::vec3 &getForward() const;
			const glm::vec3 &getRight() const;
			const glm::vec3 &getWorldUp() const;
			float getFOV() const;

			void setAspectRatio(float aspect);

			void setPosition(const glm::vec3 &position);
			void move(const glm::vec3 &offset);
			void rotate(float yawDegrees, float pitchDegrees);
			void zoom(float fovDelta);

		private:
			glm::mat4 calculateView() const;
			void updateVectors();
			void updateProjection();

			common::Logger &m_logger;

			float m_aspect; ///< Width divided by height, used by the projection.

			// Vectors
			glm::vec3 m_position; ///< Position in world space.
			glm::vec3 m_worldUp; ///< Normalized world up axis, used for vertical movement and to build the camera axes.

			glm::vec3 m_direction; ///< Normalized forward vector, derived from @ref m_yaw and @ref m_pitch.
			glm::vec3 m_right;     ///< Normalized right vector, derived from @ref m_yaw and @ref m_pitch.
			glm::vec3 m_up;        ///< Normalized up vector, derived from @ref m_yaw and @ref m_pitch.

			// Matrices
			glm::mat4 m_view;       ///< Cached view matrix, refreshed in @ref getView().
			glm::mat4 m_projection; ///< Projection matrix, refreshed when the FOV or the aspect ratio changes.

			float m_yaw;   ///< Horizontal angle in degrees, -90 looks along -Z.
			float m_pitch; ///< Vertical angle in degrees, clamped to [-89, 89].
			float m_fov;   ///< Vertical field of view in degrees.
	};

} // namespace newton::graphics

#endif // GRAPHICS_CAMERA_HPP
