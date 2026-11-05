#include <GLFW/glfw3.h>

#include "../include/common/Logger.hpp"
#include "../include/controllers/CameraController.hpp"
#include "../include/graphics/Camera.hpp"
#include "../include/graphics/Context.hpp"
#include "../include/graphics/Renderer.hpp"
#include "../include/graphics/resolution.hpp"
#include "../include/input/InputManager.hpp"

using namespace newton;

static void handleGameInput(graphics::Renderer &renderer);

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
	graphics::Renderer renderer;
	graphics::Camera mainCamera({0, 0, 0}, {0, 0, 0}, {0, 0, 0});
	controllers::CameraController cameraController(mainCamera);

	// Game loop
	while (graphics::Context::shouldClose() == false) {
		graphics::Context::pollEvents();

		handleGameInput(renderer);

		graphics::Context::update();

		cameraController.update(graphics::Context::deltaTime());

		input::InputManager::update();

		graphics::Context::swapBuffers();
	}

	graphics::Context::cleanup();

	return 0;
}

void handleGameInput(graphics::Renderer &renderer)
{
	if (input::InputManager::isKeyPressed(GLFW_KEY_ESCAPE))
		graphics::Context::requestClose();

	if (input::InputManager::isKeyPressed(GLFW_KEY_P))
		renderer.togglePolygonMode();
}
