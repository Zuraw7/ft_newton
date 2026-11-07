#include <GLFW/glfw3.h>

#include <glm/gtc/matrix_transform.hpp>
#include <memory>

#include "common/Logger.hpp"
#include "controllers/CameraController.hpp"
#include "geometry/Primitives.hpp"
#include "graphics/Camera.hpp"
#include "graphics/Color.hpp"
#include "graphics/Context.hpp"
#include "graphics/GLError.hpp"
#include "graphics/Mesh.hpp"
#include "graphics/Renderer.hpp"
#include "graphics/Resolution.hpp"
#include "input/InputManager.hpp"
#include "ui/DisplayPanel.hpp"
#include "ui/ImGuiLayer.hpp"
#include "ui/StatsOverlay.hpp"

using namespace newton;

static void handleGameInput(graphics::Context &context, graphics::Renderer &renderer);

int main()
{
	// Global logger configuration
	common::Logger &logger = common::Logger::getInstance();
	logger.setLogLevel(common::LogLevel::INFO);

	// Context setup
	graphics::Resolution res = graphics::FHD;

	auto context = graphics::Context::create(res.width, res.height, "ft_newton");
	if (!context)
		return -1;
	input::InputManager::init(context->window());

	ui::ImGuiLayer imguiLayer(context->window());

	// Renderer creation
	std::unique_ptr<graphics::Renderer> renderer =
		graphics::Renderer::create("assets/shaders/basic.vert", "assets/shaders/basic.frag");
	if (!renderer)
		return -1;
	renderer->setBackgroundColor(graphics::rgb(77, 189, 255));

	// Camera creation
	graphics::Camera mainCamera({0, 4, 10}, {0, 0, 0}, {0, 1, 0});
	controllers::CameraController cameraController(mainCamera);

	// Objects creation
	graphics::Mesh cubeMesh(geometry::makeCube());
	glm::mat4 cubeModel(1.0f);

	graphics::Mesh planeMesh(geometry::makePlane());
	glm::mat4 planeModel(1.0f);
	planeModel = glm::translate(planeModel, glm::vec3(0.0f, -0.5f, 0.0f));
	planeModel = glm::scale(planeModel, glm::vec3(10.0f, 1.0f, 10.0f));

	graphics::Mesh sphereMesh(geometry::makeSphere());
	glm::mat4 sphereModel(1.0f);
	sphereModel = glm::translate(sphereModel, glm::vec3(1.0f, 0.0f, 1.0f));

	glm::vec3 gray(graphics::rgb(154, 150, 159));
	glm::vec3 turquoise(graphics::rgb(0x4dbd00));

	// Game loop
	while (context->shouldClose() == false) {
		context->pollEvents();

		handleGameInput(*context, *renderer);

		context->update();
		cameraController.update(context->deltaTime(), imguiLayer.wantsMouse());
		input::InputManager::update();

		imguiLayer.beginFrame();
		ui::drawStatsOverlay(context->FPS());
		ui::drawDisplayPanel(*context);

		renderer->beginFrame(mainCamera);
		renderer->draw(cubeMesh, cubeModel, gray);
		renderer->draw(sphereMesh, sphereModel, gray);
		renderer->draw(planeMesh, planeModel, turquoise);

		imguiLayer.endFrame();

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
