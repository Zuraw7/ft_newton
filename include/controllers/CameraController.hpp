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
	 * - Scroll - zoom (changes FOV)
	 *
	 * While the mouse is blocked (see @ref update()), e.g. over a UI window, the right button does not start
	 * movement and scrolling does not zoom. Movement already started continues until the button is released.
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
					float speed = 20.0f;          ///< Movement speed in world units per second.
					float fastMultiplier = 2.0f;  ///< Speed multiplier.
					float sensitivity = 0.1f;     ///< Rotation in degrees per pixel of mouse movement.
					float zoomSensitivity = 1.0f; ///< Change of FOV in degrees per scroll step.
			};

			explicit CameraController(graphics::Camera &camera);

			void switchCamera(graphics::Camera &camera);
			void update(double deltaTime, bool mouseBlocked);

			void setSettings(const Settings &settings);
			const Settings &settings() const;

			float fov() const;

		private:
			void moveCamera(double deltaTime);
			void rotateCamera();
			void zoomCamera();

			graphics::Camera *m_camera; ///< Currently controlled camera. Never null, set only from references.
			common::Logger &m_logger;

			Settings m_settings; ///< Movement parameters, defaults from @ref Settings.

			bool m_active; ///< True while the right mouse button drag controls the camera.
	};

} // namespace newton::controllers

#endif // CONTROLLERS_CAMERA_CONTROLLER_HPP
