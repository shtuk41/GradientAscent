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

#include <routePreview.h>
#include <controls.h>
#include <volume.h>
#include <window.h>
#include <activity_stream.h>
#include <geo_data_handler.h>
#include <utilities.h>

std::unique_ptr<Context> context;

static void glfw_error_callback(int error, const char* description)
{
	std::cout << "Glfw Error " << error << " : " << description << "\n";
}

int main()
{
	GDALAllRegister();

	//fs::path testgpxpath(R"(D:\Files\GradientAscent\data\Afternoon_Ride.gpx)");
	fs::path testgpxpath(R"(D:\Files\GradientAscent\data\Afternoon_Ride_09272026.gpx)");

	std::vector<std::tuple<int, int, float, float>> routeData;

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

		routeData.reserve(trackpoints.size());

		trackpoint tp = trackpoints.front();

		float lastvalidElevation = std::numeric_limits<float>::quiet_NaN();;
		//int line = 0;
		//std::ofstream elevationFile("elevation.csv", std::ios::trunc);

		for (auto& t : trackpoints)
		{
			//std::cout << std::format("Lat {}, Lon {}, Elevation {} at time {}\n", t.lat, t.lon, t.elevation, t.time);

			float elevation;
				
			GEO_DATA_ERROR error = tiffHandler.getElevation(t.lat, t.lon, elevation);

			if (error == GEO_DATA_ERROR::GEO_DATA_NO_ERROR)
			{
				lastvalidElevation = elevation;
			}
			else if (std::isnan(lastvalidElevation))
			{
				continue;
			}
			else
			{
				elevation = lastvalidElevation;
			}

			//std::string writeLine =  std::format("{},{},\n", line, elevation);
			//elevationFile.write(writeLine.c_str(), writeLine.length());
			//line += 1;

			float difference = t.elevation - elevation;

			//std::cout << std::format("{}tiff elevation {}, difference is {} {}\n", GREEN, elevation, difference, RESET);
			tiffHandler.putTreckPoint(t.lat, t.lon);

			int row, col;

			GEO_DATA_ERROR pixelError = tiffHandler.getPixelCoordinate(t.lat, t.lon, row, col);

			if (pixelError == GEO_DATA_ERROR::GEO_DATA_NO_ERROR)
			{
				float gradient;
				computeGradient(tp, t, gradient);
				routeData.push_back({ row, col, elevation, gradient });
			}

			tp = t;
		}

		std::cout << std::format("Saved points: {} vs reserved {}\n", routeData.size(), routeData.capacity());
	}
	catch (std::exception& e)
	{
		std::cout << std::format("Global exception: {}", e.what());
		return 0;
	}

	float minimumGraident = getMinRouteGradient(routeData);
	float maximumGradient = getMaxRouteGradient(routeData);

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

	glm::vec3 current_pos = glm::vec3(0.0f, 0.0f, 1000.0f );
	cameraGlobal.setPosition(current_pos);

	Axes3d axes3d(1.2, 1.2, -1.2);
	axes3d.Setup();

	RoutePreview routePreview(routeData);
	routePreview.Setup();

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO const& io = ImGui::GetIO(); (void)io;

	ImGui::StyleColorsDark();

	ImGui_ImplGlfw_InitForOpenGL(window.GetHandler(), true);
	ImGui_ImplOpenGL3_Init(glsl_version);

	auto backgroundColor = ImVec4(23.0f/255.0f, 20.0f/255.0f, 20.0f/255.0f, 1.0f);

	glDisable(GL_DEPTH_TEST);

	int saveFrameColorClicked = 0;

	while (!glfwWindowShouldClose(window.GetHandler()))
	{
		glFrontFace(GL_CW);
		glfwPollEvents();

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		{
			ImGui::Begin("ControlWindow");
			ImGui::ColorEdit3("clear color", (float*)&backgroundColor);
			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			ImGui::Separator();
			ImGui::Separator();
			ImGui::PushItemWidth(ImGui::GetWindowWidth());
			ImGui::PushItemWidth(40);
			ImGui::PushItemWidth(ImGui::GetWindowWidth());
			ImGui::Separator();
			ImGui::Separator();
			ImGui::Text("Minimum and maximum gradients %0.3f / %0.3f", minimumGraident, maximumGradient);
			ImGui::Separator();
			ImGui::Separator();
			ImGui::End();
		}

		ImGui::Render();

		int display_w, display_h;
		glfwGetFramebufferSize(window.GetHandler(), &display_w, &display_h);
		glViewport(0, 0, display_w, display_h);
		glClearColor(backgroundColor.x * backgroundColor.w, backgroundColor.y * backgroundColor.w, backgroundColor.z * backgroundColor.w, backgroundColor.w);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		if (!ImGui::GetIO().WantCaptureMouse)
		{
			cameraGlobal.setOffsetX(context->latShift);
			cameraGlobal.setOffsetY(context->vertShift);

			if (Controls::moveback || Controls::key_w)
			{
				context->orthoLeft += 10;
				context->orthoRight -= 10;
				context->orthoBottom += 10;
				context->orthoTop -= 10;
			}
			else if (Controls::moveforward || Controls::key_s)
			{
				context->orthoLeft -= 10;
				context->orthoRight += 10;
				context->orthoBottom -= 10;
				context->orthoTop += 10;
			}

			cameraGlobal.computeViewProjectionMatrices(context->GetOrthoLeft(),
				context->GetOrthoRight(),
				context->GetOrthoBottom(),
				context->GetOrthoTop(),
				context->GetOrthoNear(),
				context->GetOrthoFar(),
				context->orientation);
		}

		glm::mat4 projection_matrix;
		glm::mat4 view_matrix;

		projection_matrix = cameraGlobal.getProjectionMatrix();
		view_matrix = cameraGlobal.getViewMatrix();

		Controls::moveback = false;
		Controls::moveforward = false;
		Controls::key_w = false;
		Controls::key_s = false;

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

		glUseProgram(routePreview.GetProgramId());
		routePreview.UpdateModel(view_matrix);
		routePreview.SetProjection(projection_matrix);
		routePreview.Draw();

		glfwSwapBuffers(window.GetHandler());
	}

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	window.Destroy();
	window.~Window();

	glfwTerminate();

	return 0;

}
