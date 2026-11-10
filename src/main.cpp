#include <GLFW/glfw3.h>

#include <memory>
#include <variant>

#include "common/Logger.hpp"
#include "controllers/CameraController.hpp"
#include "geometry/Primitives.hpp"
#include "graphics/Camera.hpp"
#include "graphics/ColliderRenderer.hpp"
#include "graphics/Color.hpp"
#include "graphics/Context.hpp"
#include "graphics/GLError.hpp"
#include "graphics/GridRenderer.hpp"
#include "graphics/Mesh.hpp"
#include "graphics/Renderer.hpp"
#include "input/InputManager.hpp"
#include "physics/Collider.hpp"
#include "physics/PhysicsWorld.hpp"
#include "physics/RigidBody.hpp"
#include "scene/Scene.hpp"
#include "scene/SceneObject.hpp"
#include "ui/CameraPanel.hpp"
#include "ui/DisplayPanel.hpp"
#include "ui/ImGuiLayer.hpp"
#include "ui/ObjectCreatorPanel.hpp"
#include "ui/PhysicsPanel.hpp"
#include "ui/StatsOverlay.hpp"

using namespace newton;

static void handleGameInput(graphics::Context &context, graphics::Renderer &renderer);

namespace
{
	/**
	 * @brief Draws one collider with the matching graphics::ColliderRenderer method.
	 */
	struct ColliderDrawer
	{
			newton::graphics::ColliderRenderer &renderer;
			const glm::vec3 &position;

			void operator()(const newton::physics::Collider::Sphere &sphere) const
			{
				renderer.drawSphere(position, sphere.radius);
			}

			void operator()(const newton::physics::Collider::Box &box) const
			{
				renderer.drawBox(position, box.halfExtents);
			}

			void operator()(const newton::physics::Collider::Plane &plane) const
			{
				renderer.drawPlane(position, plane.normal);
			}
	};

} // unnamed namespace

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

	std::unique_ptr<graphics::ColliderRenderer> colliderRenderer =
		graphics::ColliderRenderer::create("assets/shaders/line.vert", "assets/shaders/line.frag");
	if (!colliderRenderer)
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
	physics::PhysicsWorld world;

	// Create object creator panel
	ui::ObjectCreatorPanel objectCreatorPanel({{"Cube", &cubeMesh, {physics::Collider::Box{}}},
	                                           {"Sphere", &sphereMesh, {physics::Collider::Sphere{}}},
	                                           {"Plane", &planeMesh, {physics::Collider::Plane{}}}});

	// Game loop
	while (context->shouldClose() == false) {
		context->pollEvents();

		if (!imguiLayer.wantsKeyboard())
			handleGameInput(*context, *renderer);

		context->update();
		mainCamera.setAspectRatio(context->aspectRatio());
		cameraController.update(context->deltaTime(), imguiLayer.wantsMouse());
		input::InputManager::update();

		world.update(context->deltaTime());
		for (scene::SceneObject &object : scene.objects())
			if (object.bodyIndex)
				object.transform.position = world.body(*object.bodyIndex).position;

		imguiLayer.beginFrame();

		ui::drawStatsOverlay(context->FPS(), scene.size(), world.size());

		float panelTop = 0;
		panelTop = ui::drawDisplayPanel(*context, *gridRenderer, *colliderRenderer, panelTop);
		panelTop = ui::drawCameraPanel(cameraController, panelTop);
		panelTop = ui::drawPhysicsPanel(world, panelTop);
		objectCreatorPanel.draw(scene, world, panelTop);

		renderer->beginFrame(mainCamera);

		for (const scene::SceneObject &object : scene.objects())
			renderer->draw(*object.mesh, object.transform.matrix(), object.color);

		gridRenderer->draw(mainCamera);

		colliderRenderer->beginFrame(mainCamera);
		for (const physics::RigidBody &body : world.bodies())
			std::visit(ColliderDrawer{*colliderRenderer, body.position}, body.collider.shape);

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
