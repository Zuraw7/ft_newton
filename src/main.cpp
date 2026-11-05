#include "common/Logger.hpp"
#include "graphics/Camera.hpp"
#include "graphics/Context.hpp"
#include "graphics/resolution.hpp"
#include "input/InputManager.hpp"

using namespace newton;

graphics::Camera gCamera(
	{0,0,0},
	{0,0,0},
	{0,0,0}
);

int main() {

	common::Logger &logger = common::Logger::getInstance();
	logger.setLogLevel(common::LogLevel::DEBUG);

	Resolution res = FHD;

	if (!graphics::Context::setup(res.width, res.height, "ft_newton")) {
		return -1;
	}

	input::InputManager::init(graphics::Context::window());


	while (graphics::Context::shouldClose() == false) {

		graphics::Context::update();
	}

	graphics::Context::cleanup();

	return 0;
}
