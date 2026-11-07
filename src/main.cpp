#include <GLFW/glfw3.h>

#include <glm/gtc/matrix_transform.hpp>
#include <memory>

#include "common/Logger.hpp"
#include "controllers/CameraController.hpp"
#include "geometry/Primitives.hpp"
#include "graphics/Camera.hpp"
#include "graphics/Context.hpp"
#include "graphics/GLError.hpp"
#include "graphics/Mesh.hpp"
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

	// Context setup
	graphics::Resolution res = graphics::FHD;

	auto context = graphics::Context::create(res.width, res.height, "ft_newton");
	if (!context)
		return -1;
	input::InputManager::init(context->window());

	// Renderer creation
	std::unique_ptr<graphics::Renderer> renderer =
		graphics::Renderer::create("assets/shaders/basic.vert", "assets/shaders/basic.frag");
	if (!renderer)
		return -1;
	renderer->setBackgroundColor(0.07f, 0.11f, 0.18f, 1.0f);

	// Camera creation
	graphics::Camera mainCamera({0, 0, 3}, {0, 0, 0}, {0, 1, 0});
	controllers::CameraController cameraController(mainCamera);

	// Objects creation
	graphics::Mesh cubeMesh(geometry::makeCube());
	glm::mat4 cubeModel(1.0f);
	glm::vec3 cubeColor(0.3f, 0.12f, 0.44f);

	graphics::Mesh planeMesh(geometry::makePlane());
	glm::mat4 planeModel(1.0f);
	planeModel = glm::translate(planeModel, glm::vec3(0.0f, -0.5f, 0.0f));
	planeModel = glm::scale(planeModel, glm::vec3(10.0f, 1.0f, 10.0f));
	glm::vec3 planeColor(0.8f, 0.1f, 0.74f);

	graphics::Mesh sphereMesh(geometry::makeSphere());
	glm::mat4 sphereModel(1.0f);
	sphereModel = glm::translate(sphereModel, glm::vec3(1.0f, 0.0f, 1.0f));
	glm::vec3 sphereColor(0.5f, 0.5f, 0.5f);

	// Game loop
	while (context->shouldClose() == false) {
		context->pollEvents();

		handleGameInput(*context, *renderer);

		context->update();
		cameraController.update(context->deltaTime());
		input::InputManager::update();

		renderer->beginFrame(mainCamera);
		renderer->draw(cubeMesh, cubeModel, cubeColor);
		renderer->draw(sphereMesh, sphereModel, sphereColor);
		renderer->draw(planeMesh, planeModel, planeColor);

		graphics::checkGLError("frame");

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
