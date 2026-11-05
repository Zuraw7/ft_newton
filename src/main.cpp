#include "../include/common/Logger.hpp"
#include "../include/controllers/CameraController.hpp"
#include "../include/graphics/Camera.hpp"
#include "../include/graphics/Context.hpp"
#include "../include/graphics/resolution.hpp"
#include "../include/input/InputManager.hpp"

using namespace newton;

graphics::Camera gCamera({0, 0, 0}, {0, 0, 0}, {0, 0, 0});

int main()
{
	// Global logger configuration
	common::Logger &logger = common::Logger::getInstance();
	logger.setLogLevel(common::LogLevel::DEBUG);

	// Setup
	Resolution res = FHD;

	if (!graphics::Context::setup(res.width, res.height, "ft_newton"))
		return -1;
	input::InputManager::init(graphics::Context::window());
	graphics::Camera mainCamera({0, 0, 0}, {0, 0, 0}, {0, 0, 0});
	controllers::CameraController cameraController(mainCamera);

	// Game loop
	while (graphics::Context::shouldClose() == false) {
		graphics::Context::pollEvents();

		graphics::Context::update();

		cameraController.update(graphics::Context::deltaTime());

		input::InputManager::processInput(graphics::Context::window());

		graphics::Context::swapBuffers();
	}

	graphics::Context::cleanup();

	return 0;
}
