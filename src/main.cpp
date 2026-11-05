#include <GLFW/glfw3.h>

#include "common/Logger.hpp"
#include "controllers/CameraController.hpp"
#include "graphics/Camera.hpp"
#include "graphics/Context.hpp"
#include "graphics/Renderer.hpp"
#include "graphics/Resolution.hpp"
#include "input/InputManager.hpp"

using namespace newton;

static void handleGameInput(graphics::Renderer &renderer);

int main()
{
	// Global logger configuration
	common::Logger &logger = common::Logger::getInstance();
	logger.setLogLevel(common::LogLevel::DEBUG);

	// Setup
	graphics::Resolution res = graphics::FHD;

	if (!graphics::Context::setup(res.width, res.height, "ft_newton"))
		return -1;
	input::InputManager::init(graphics::Context::window());
	graphics::Renderer renderer;
	graphics::Camera mainCamera({0, 0, 3}, {0, 0, 0}, {0, 1, 0});
	controllers::CameraController cameraController(mainCamera);

	renderer.setBackgroundColor(0.07f, 0.11f, 0.18f, 1.0f);
	// Game loop
	while (graphics::Context::shouldClose() == false) {
		graphics::Context::pollEvents();

		handleGameInput(renderer);

		graphics::Context::update();

		cameraController.update(graphics::Context::deltaTime());

		input::InputManager::update();

		renderer.clear();
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
