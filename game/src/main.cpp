#include <GLFW/glfw3.h>

#include <memory>

#include "newton/common/Logger.hpp"
#include "newton/controllers/CameraController.hpp"
#include "newton/geometry/Primitives.hpp"
#include "newton/graphics/Camera.hpp"
#include "newton/graphics/Color.hpp"
#include "newton/graphics/Context.hpp"
#include "newton/graphics/GLError.hpp"
#include "newton/graphics/GridRenderer.hpp"
#include "newton/graphics/Mesh.hpp"
#include "newton/graphics/MeshRenderer.hpp"
#include "newton/input/InputManager.hpp"
#include "newton/scene/Scene.hpp"
#include "newton/scene/SceneObject.hpp"
#include "newton/ui/CameraPanel.hpp"
#include "newton/ui/DisplayPanel.hpp"
#include "newton/ui/ImGuiLayer.hpp"
#include "newton/ui/ObjectCreatorPanel.hpp"
#include "newton/ui/StatsOverlay.hpp"

using namespace newton;

static void handleGameInput(graphics::Context &context, graphics::MeshRenderer &renderer);

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

	// Renderers creation
	std::unique_ptr<graphics::MeshRenderer> meshRenderer =
		graphics::MeshRenderer::create("assets/shaders/mesh.vert", "assets/shaders/mesh.frag");
	if (!meshRenderer)
		return -1;
	meshRenderer->setBackgroundColor(graphics::rgb(77, 189, 255));

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

	// Create object creator panel
	ui::ObjectCreatorPanel objectCreatorPanel({{"Cube", &cubeMesh}, {"Sphere", &sphereMesh}, {"Plane", &planeMesh}});

	// Game loop
	while (context->shouldClose() == false) {
		context->pollEvents();

		if (!imguiLayer.wantsKeyboard())
			handleGameInput(*context, *meshRenderer);

		context->update();
		mainCamera.setAspectRatio(context->aspectRatio());
		cameraController.update(context->deltaTime(), imguiLayer.wantsMouse());
		input::InputManager::update();

		imguiLayer.beginFrame();

		ui::drawStatsOverlay(context->FPS(), scene.size());

		float panelTop = 0;
		panelTop = ui::drawDisplayPanel(*context, *gridRenderer, panelTop);
		panelTop = ui::drawCameraPanel(cameraController, panelTop);
		objectCreatorPanel.draw(scene, panelTop);

		meshRenderer->beginFrame(mainCamera);

		for (const scene::SceneObject &object : scene.objects())
			meshRenderer->draw(*object.mesh, object.transform.matrix(), object.color);

		gridRenderer->draw(mainCamera);

		imguiLayer.endFrame();

		graphics::checkGLError("frame");

		context->swapBuffers();
	}

	return 0;
}

void handleGameInput(graphics::Context &context, graphics::MeshRenderer &renderer)
{
	if (input::InputManager::isKeyPressed(GLFW_KEY_ESCAPE))
		context.requestClose();

	if (input::InputManager::isKeyPressed(GLFW_KEY_P))
		renderer.togglePolygonMode();

	if (input::InputManager::isKeyPressed(GLFW_KEY_F11))
		context.setFullscreen(!context.isFullscreen());
}
