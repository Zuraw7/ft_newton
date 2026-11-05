#include <GLFW/glfw3.h>

#include "common/Logger.hpp"
#include "controllers/CameraController.hpp"
#include "graphics/Camera.hpp"
#include "graphics/Context.hpp"
#include "graphics/Renderer.hpp"
#include "graphics/Resolution.hpp"
#include "input/InputManager.hpp"

using namespace newton;

static void handleGameInput(graphics::Context &context, graphics::Renderer &renderer);

int main()
{
	// Global logger configuration
	common::Logger &logger = common::Logger::getInstance();
	logger.setLogLevel(common::LogLevel::DEBUG);

	// Setup
	graphics::Resolution res = graphics::FHD;

	auto context = graphics::Context::create(res.width, res.height, "ft_newton");
	if (!context)
		return -1;
	input::InputManager::init(context->window());
	graphics::Renderer renderer;
	graphics::Camera mainCamera({0, 0, 3}, {0, 0, 0}, {0, 1, 0});
	controllers::CameraController cameraController(mainCamera);

	renderer.setBackgroundColor(0.07f, 0.11f, 0.18f, 1.0f);
	// Game loop
	while (context->shouldClose() == false) {
		context->pollEvents();

		handleGameInput(*context, renderer);

		context->update();

		cameraController.update(context->deltaTime());

		input::InputManager::update();

		renderer.clear();
		context->swapBuffers();
	}

	return 0;
}

void handleGameInput(graphics::Context &context, graphics::Renderer &renderer)
{
	if (input::InputManager::isKeyPressed(GLFW_KEY_ESCAPE))
		context.requestClose();

	if (input::InputManager::isKeyPressed(GLFW_KEY_P))
		renderer.togglePolygonMode();
}
