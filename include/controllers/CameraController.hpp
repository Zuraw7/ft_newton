#ifndef CONTROLLERS_CAMERA_CONTROLLER_HPP
#define CONTROLLERS_CAMERA_CONTROLLER_HPP

#include "../common/Logger.hpp"
#include "../graphics/Camera.hpp"

namespace newton::controllers
{
	// TODO: documentation (include that Camera MUST outlive this controller,
	// 	that means be created before and removed/destroyed after)
	class CameraController
	{
		public:
			explicit CameraController(graphics::Camera &camera);

			void switchCamera(graphics::Camera &camera);
			void update(double deltaTime);

		private:
			void moveCamera(double deltaTime);
			void rotateCamera();

			graphics::Camera *m_camera;
			common::Logger &m_logger;
	};

} // namespace newton::controllers

#endif // CONTROLLERS_CAMERA_CONTROLLER_HPP
