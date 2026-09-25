// CTLab.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <memory>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>

#include <axisPlane.h>
#include <controls.h>
#include <volume.h>
#include <window.h>

std::unique_ptr<Context> context;

static void glfw_error_callback(int error, const char* description)
{
	std::cout << "Glfw Error " << error << " : " << description << "\n";
}

int main()
{
	glfwSetErrorCallback(glfw_error_callback);

	if (!glfwInit())
		return -1;

	const char* glsl_version = "#version 330";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

	int window_width = 1024;
	int window_height = 768;

	Window window(window_width, window_height, Controls::key_callback, Controls::mouse_callback, Controls::mouse_button_callback, Controls::scroll_callback);

	std::cout << "Window Width: " << window.GetWidth() << '\n';
	std::cout << "Window Height: " << window.GetHeight() << '\n';

	context = std::make_unique<Context>(window.GetHandler());

	glfwSetInputMode(window.GetHandler(), GLFW_STICKY_KEYS, GL_FALSE);
	glfwSwapInterval(1); // Enable vsync

	glewExperimental = true;
	if (glewInit() != GLEW_OK)
	{
		std::cout << "Failed to initlize GLEW" << "\n";
		return 0;
	}

	Camera cameraGlobal(window.GetHandler(), 3.0);

	Axes3d axes3d(1.2, 1.2, -1.2);
	axes3d.Setup();

	Volume volume(512, 512, 512, &cameraGlobal);
	volume.Setup();

	//Axis
	AxisPlane planeXY(glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec4(0.0f, 1.0f, 0.0f, 0.1f), 2);
	planeXY.Setup();
	//Coronal
	AxisPlane planeXZ(glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec4(0.0f, 0.0f, 1.0f, 0.1f), 2);
	planeXZ.Setup();
	//Sagittal
	AxisPlane planeYZ(glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec4(1.0f, 0.0f, 0.0f, 0.1f), 2);
	planeYZ.Setup();

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO const& io = ImGui::GetIO(); (void)io;

	ImGui::StyleColorsDark();

	ImGui_ImplGlfw_InitForOpenGL(window.GetHandler(), true);
	ImGui_ImplOpenGL3_Init(glsl_version);

	auto backgroundColor = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);

	glDisable(GL_DEPTH_TEST);

	int saveFrameColorClicked = 0;

	while (!glfwWindowShouldClose(window.GetHandler()))
	{
		glFrontFace(GL_CW);
		glfwPollEvents();

		if (context->view == View::Global && saveFrameColorClicked == 0)
		{
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();

			{
				ImGui::Begin("ControlWindow");
				ImGui::ColorEdit3("clear color", (float*)&backgroundColor);
				ImGui::Text("X %.3f, Y %0.3f", context->rotateX, context->rotateY);
				ImGui::Text("X %.3f, Y %0.3f", 0, 0);
				ImGui::SliderFloat("zoffset", &context->zOffset, -50.0f, 50.0f, "zOffset = %.3f");
				ImGui::SliderFloat("lat_shift", &context->latShift, -1000.0f, 1000.0f, "latShift = %.3f");
				ImGui::SliderFloat("vert_shift", &context->vertShift, -1000.0f, 1000.0f, "vertShift = %.3f");
				ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
				ImGui::Separator();
				ImGui::Separator();
				ImGui::InputText("Screenshot name", context->screenShotName, 255);
				ImGui::InputText("Scale percent", context->screenShotScalePercent, 4);
				ImGui::Checkbox("Save to size", &context->screenshotSaveToSize); ImGui::SameLine();
				ImGui::PushItemWidth(40);
				ImGui::InputText("W", context->saveWidth, 4); ImGui::SameLine();
				ImGui::InputText("H", context->saveHeight, 4); ImGui::SameLine();
				if (ImGui::Button("Save"))
					saveFrameColorClicked++;
				ImGui::Separator();
				ImGui::PushItemWidth(ImGui::GetWindowWidth());
				ImGui::Checkbox("Use perspective", &context->usePerspectiveProjection);
				ImGui::Text("Perspective");
				ImGui::SliderFloat("slider float", &context->perspectiveFovyRadians, 0.1f, 3.0f, "perspective angle = %.3f");
				ImGui::Text("Orthographic");
				ImGui::PushItemWidth(40);
				ImGui::InputText("L", context->orthoLeft, 4); ImGui::SameLine();
				ImGui::InputText("R", context->orthoRight, 4); ImGui::SameLine();
				ImGui::InputText("B", context->orthoBottom, 4); ImGui::SameLine();
				ImGui::InputText("T", context->orthoTop, 4); ImGui::SameLine();
				ImGui::InputText("N", context->orthoNear, 5); ImGui::SameLine();
				ImGui::InputText("F", context->orthoFar, 5);
				ImGui::PushItemWidth(ImGui::GetWindowWidth());
				ImGui::Separator();
				ImGui::Separator();
				ImGui::End();
			}

			ImGui::Render();
		}

		int display_w, display_h;
		glfwGetFramebufferSize(window.GetHandler(), &display_w, &display_h);
		glViewport(0, 0, display_w, display_h);
		glClearColor(backgroundColor.x * backgroundColor.w, backgroundColor.y * backgroundColor.w, backgroundColor.z * backgroundColor.w, backgroundColor.w);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		if (context->view == View::Global && saveFrameColorClicked == 0)
		{
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		}

		if (!ImGui::GetIO().WantCaptureMouse)
		{
			cameraGlobal.rotateX(context->rotateX);
			cameraGlobal.rotateY(context->rotateY);
			cameraGlobal.setOffsetX(context->latShift);
			cameraGlobal.setOffsetY(context->vertShift);

			if (context->usePerspectiveProjection)
			{
				cameraGlobal.computeViewProjectionMatrices(Controls::moveback, Controls::moveforward);
			}
			else
			{
				cameraGlobal.computeViewProjectionMatrices(context->GetOrthoLeft(),
					context->GetOrthoRight(),
					context->GetOrthoBottom(),
					context->GetOrthoTop(),
					context->GetOrthoNear(),
					context->GetOrthoFar());
			}
		}

		glm::mat4 projection_matrix;
		glm::mat4 view_matrix;

		projection_matrix = cameraGlobal.getProjectionMatrix();
		view_matrix = cameraGlobal.getViewMatrix();

		Controls::moveback = false;
		Controls::moveforward = false;

		glUseProgram(axes3d.GetProgramId());
		axes3d.UpdateModel(view_matrix);
		axes3d.SetProjection(projection_matrix);
		axes3d.Draw();

		//glDisable(GL_DEPTH_TEST); // optional: try disabling depth for translucent volume
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDisable(GL_DEPTH_TEST);

		glUseProgram(volume.GetProgramId());
		volume.UpdateModel(view_matrix);
		volume.SetProjection(projection_matrix);
		volume.Draw();

		glUseProgram(planeXY.GetProgramId());
		planeXY.UpdateModel(view_matrix);
		planeXY.SetProjection(projection_matrix);
		planeXY.Draw();

		planeXZ.UpdateModel(view_matrix);
		planeXZ.SetProjection(projection_matrix);
		planeXZ.Draw();

		planeYZ.UpdateModel(view_matrix);
		planeYZ.SetProjection(projection_matrix);
		planeYZ.Draw();



		if (saveFrameColorClicked > 2)
		{
			context->SaveImage();
			saveFrameColorClicked = 0;
		}
		else if (saveFrameColorClicked > 0)
			saveFrameColorClicked++;

		glfwSwapBuffers(window.GetHandler());

		context->rotateY = 0.0f;
		context->rotateX = 0.0f;
	}

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	window.Destroy();
	window.~Window();

	glfwTerminate();

	return 0;

}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
