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

		logger.printf(common::LogLevel::INFO, "[%s] ImGui initialized successfully\n", cClassName);
	}

	ImGuiLayer::~ImGuiLayer()
	{
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}

	void ImGuiLayer::beginFrame()
	{
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
	}

	void ImGuiLayer::endFrame()
	{
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}

} // namespace newton::ui
