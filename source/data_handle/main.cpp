// CTLab.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <exception>
#include <format>
#include <fstream>
#include <iostream>
#include <memory>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <gdal.h>
#include "gdal_priv.h"
#include "cpl_conv.h"

#include <axisPlane.h>
#include <controls.h>
#include <volume.h>
#include <window.h>
#include <activity_stream.h>
#include <geo_data_handler.h>

std::unique_ptr<Context> context;

static void glfw_error_callback(int error, const char* description)
{
	std::cout << "Glfw Error " << error << " : " << description << "\n";
}

int main()
{
	GDALAllRegister();

	fs::path testgpxpath(R"(D:\Files\GradientAscent\data\Afternoon_Ride.gpx)");

	try
	{
		gpx track(testgpxpath);

		std::string trackName = testgpxpath.stem().string();;

		auto [minlat, maxlat, minlon, maxlon] = track.getMinMaxLatLon();
		std::cout << std::format("{}, {}, {}, {}\n", minlat, maxlat, minlon, maxlon);
		ot_data_download(trackName + ".tif", minlat, maxlat, minlon, maxlon, 0.01f);
		fs::path geofilepath(trackName + ".tif");
		tiff_data_handler tiffHandler(geofilepath);

		auto trackpoints = track.getTrackpoints();
		std::cout << std::format("number of points: {}\n", trackpoints.size());

		std::vector<std::tuple<int, int, float>> routeData;
		routeData.reserve(trackpoints.size());

		for (auto& t : trackpoints)
		{
			std::cout << std::format("Lat {}, Lon {}, Elevation {} at time {}\n", t.lat, t.lon, t.elevation, t.time);

			float elevation;
				
			GEO_DATA_ERROR error = tiffHandler.getElevation(t.lat, t.lon, elevation);

			if (error == GEO_DATA_ERROR::GEO_DATA_NO_ERROR)
			{
				float difference = t.elevation - elevation;

				std::cout << std::format("{}tiff elevation {}, difference is {} {}\n", GREEN, elevation, difference, RESET);
				tiffHandler.putTreckPoint(t.lat, t.lon);

				int row, col;

				GEO_DATA_ERROR pixelError = tiffHandler.getPixelCoordinate(t.lat, t.lon, row, col);

				if (pixelError == GEO_DATA_ERROR::GEO_DATA_NO_ERROR)
				{
					routeData.push_back({ row, col, elevation});
				}
			}
			else
			{
				std::cout << std::format("{}tiff elevation is nan{}\n", RED, RESET);
			}
		}

		std::cout << std::format("Saved points: {} vs reserved {}\n", routeData.size(), routeData.capacity());
	}
	catch (std::exception& e)
	{
		std::cout << std::format("Global exception: {}", e.what());
		return 0;
	}

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

		//glUseProgram(planeXY.GetProgramId());
		//planeXY.UpdateModel(view_matrix);
		//planeXY.SetProjection(projection_matrix);
		//planeXY.Draw();

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
