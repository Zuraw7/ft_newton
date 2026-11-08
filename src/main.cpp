#include <GLFW/glfw3.h>

#include <memory>

#include "common/Logger.hpp"
#include "controllers/CameraController.hpp"
#include "geometry/Primitives.hpp"
#include "graphics/Camera.hpp"
#include "graphics/Color.hpp"
#include "graphics/Context.hpp"
#include "graphics/GLError.hpp"
#include "graphics/GridRenderer.hpp"
#include "graphics/Mesh.hpp"
#include "graphics/Renderer.hpp"
#include "input/InputManager.hpp"
#include "scene/Scene.hpp"
#include "scene/SceneObject.hpp"
#include "ui/CameraPanel.hpp"
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
	auto context = graphics::Context::create("ft_newton");
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

	std::unique_ptr<graphics::GridRenderer> gridRenderer =
		graphics::GridRenderer::create("assets/shaders/grid.vert", "assets/shaders/grid.frag");
	if (!gridRenderer)
		return -1;

	// Camera creation
	graphics::Camera mainCamera({0, 4, 10}, {0, 0, 0}, {0, 1, 0});
	controllers::CameraController cameraController(mainCamera);

	// Mesh creation
	graphics::Mesh cubeMesh(geometry::makeCube());
	graphics::Mesh sphereMesh(geometry::makeSphere());
	graphics::Mesh planeMesh(geometry::makePlane());

	// Scene creation
	scene::Scene scene;

	scene::SceneObject floor;
	floor.transform.position = {0.0f, 0.01f, 0.0f};
	floor.transform.scale = {20.0f, 1.0f, 20.0f};
	floor.mesh = &planeMesh;
	floor.color = graphics::rgb(223, 99, 46);
	scene.add(floor);

	scene::SceneObject redCube;
	redCube.transform.position = {1.0f, 0.5f, 1.0f};
	redCube.mesh = &cubeMesh;
	redCube.color = graphics::rgb(255, 0, 0);
	scene.add(redCube);

	scene::SceneObject blueCube;
	blueCube.transform.position = {-1.0f, 0.5f, -1.0f};
	blueCube.mesh = &cubeMesh;
	blueCube.color = graphics::rgb(0, 0, 255);
	scene.add(blueCube);

	scene::SceneObject greenSphere;
	greenSphere.transform.position = {0.0f, 0.5f, 0.0f};
	greenSphere.mesh = &sphereMesh;
	greenSphere.color = graphics::rgb(0, 255, 0);
	scene.add(greenSphere);

	// Game loop
	while (context->shouldClose() == false) {
		context->pollEvents();

		handleGameInput(*context, *renderer);

		context->update();
		mainCamera.setAspectRatio(context->aspectRatio());
		cameraController.update(context->deltaTime(), imguiLayer.wantsMouse());
		input::InputManager::update();

		imguiLayer.beginFrame();

		ui::drawStatsOverlay(context->FPS(), scene.size());

		float panelTop = 0;
		panelTop = ui::drawDisplayPanel(*context, *gridRenderer, panelTop);
		ui::drawCameraPanel(cameraController, panelTop);

		renderer->beginFrame(mainCamera);

		for (const scene::SceneObject &object : scene.objects())
			renderer->draw(*object.mesh, object.transform.matrix(), object.color);

		gridRenderer->draw(mainCamera);

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

	if (input::InputManager::isKeyPressed(GLFW_KEY_F11))
		context.setFullscreen(!context.isFullscreen());
}
