#ifndef CONTROLLERS_CAMERA_CONTROLLER_HPP
#define CONTROLLERS_CAMERA_CONTROLLER_HPP

#include "common/Logger.hpp"
#include "graphics/Camera.hpp"

namespace newton::controllers
{
	/**
	 * @brief Translates keyboard and mouse input into movement of a bound camera.
	 *
	 * Controls:
	 * - Right mouse button (hold) - enables movement and rotation, hides the cursor
	 *   - W / S / A / D - move forward / backward / left / right
	 *   - Space / Left Shift - move up / down
	 *   - Ctrl - move faster
	 *   - Mouse movement - rotate
	 * - Scroll - zoom (changes FOV), always active
	 *
	 * The controller does not own the camera, it only stores a pointer to it.
	 * The bound camera must outlive the controller: create it before the controller
	 * and destroy it after.
	 */
	class CameraController
	{
		public:
			/**
			 * @brief Tunable movement parameters.
			 */
			struct Settings
			{
					float speed = 20.0f;         ///< Movement speed in world units per second.
					float fastMultiplier = 2.0f; ///< Speed multiplier.
					float sensitivity = 0.1f;    ///< Rotation in degrees per pixel of mouse movement.
			};

			explicit CameraController(graphics::Camera &camera);

			void switchCamera(graphics::Camera &camera);
			void update(double deltaTime);

		private:
			void moveCamera(double deltaTime);
			void rotateCamera();
			void zoomCamera();

			graphics::Camera *m_camera; ///< Currently controlled camera. Never null, set only from references.
			common::Logger &m_logger;

			Settings m_settings; ///< Movement parameters, defaults from @ref Settings.
	};

} // namespace newton::controllers

#endif // CONTROLLERS_CAMERA_CONTROLLER_HPP
