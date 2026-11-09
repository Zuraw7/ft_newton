#include "ui/ImGuiLayer.hpp"

#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "common/Logger.hpp"

namespace
{
	const char *cClassName = "ImGuiLayer";
	newton::common::Logger &logger = newton::common::Logger::getInstance();
} // unnamed namespace

namespace newton::ui
{
	/**
	 * @brief Creates the ImGui context and initializes the GLFW and OpenGL 3 backends.
	 *
	 * Requires a current OpenGL 3.3 context. Create after @ref input::InputManager::init(): the GLFW backend
	 * installs its own callbacks and chains the ones already set, callbacks set later would replace it.
	 * Backend failures are logged, not reported to the caller.
	 *
	 * @param window Window ImGui reads input from. Must outlive this object.
	 */
	ImGuiLayer::ImGuiLayer(GLFWwindow *window)
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();

		ImGuiIO &io = ImGui::GetIO();
		io.IniFilename = nullptr;

		// ImGui would set the cursor every frame, which stutters on Wayland and fights InputManager's cursor modes
		io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;

		if (!ImGui_ImplGlfw_InitForOpenGL(window, true)) {
			logger.printf(common::LogLevel::ERROR, "[%s] Failed to initialize ImGui GLFW backend\n", cClassName);
		}

		if (!ImGui_ImplOpenGL3_Init("#version 330")) {
			logger.printf(common::LogLevel::ERROR, "[%s] Failed to initialize ImGui OpenGL3 backend\n", cClassName);
		}

		ImGui::StyleColorsDark();
		ImGui::GetStyle().SeparatorTextAlign = {0.5f, 0.5f};

		logger.printf(common::LogLevel::INFO, "[%s] ImGui initialized successfully\n", cClassName);
	}

	ImGuiLayer::~ImGuiLayer()
	{
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}

	/**
	 * @brief Starts a new ImGui frame.
	 *
	 * Call once per frame, after events are polled and before any ImGui window is drawn.
	 * Every call must be followed by @ref endFrame() in the same frame.
	 */
	void ImGuiLayer::beginFrame()
	{
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
	}

	/**
	 * @brief Finishes the ImGui frame and draws it into the current framebuffer.
	 *
	 * Call after the scene is drawn and before swapping buffers, so ImGui windows end up on top.
	 * The OpenGL state changed while drawing (shader, VAO, blending, polygon mode, ...) is restored afterwards.
	 */
	void ImGuiLayer::endFrame()
	{
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}

	/**
	 * @brief Tells whether ImGui uses the mouse: the cursor is over an ImGui window or a widget is being dragged.
	 *
	 * The value is updated by @ref beginFrame(). Read before it, it describes the previous frame, which is
	 * accurate enough to decide whether game code should ignore mouse input.
	 * Stays false while the cursor is disabled.
	 *
	 * @return True if mouse input should not be handled by the game this frame.
	 */
	bool ImGuiLayer::wantsMouse() const
	{
		return ImGui::GetIO().WantCaptureMouse;
	}

	/**
	 * @brief Tells whether ImGui uses the keyboard, e.g. a value is being typed into a field.
	 *
	 * Like @ref wantsMouse(), read before @ref beginFrame() it describes the previous frame.
	 *
	 * @return True if keyboard input should not be handled by the game this frame.
	 */
	bool ImGuiLayer::wantsKeyboard() const
	{
		return ImGui::GetIO().WantCaptureKeyboard;
	}

} // namespace newton::ui
