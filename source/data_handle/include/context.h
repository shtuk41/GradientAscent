#pragma once

#include <numbers>
#include <vector>

#include <camera.h>
#include <axes3d.h>
#include <GLFW/glfw3.h>



struct Context
{
	//infrmation
	float rotateY;
	float rotateX;
	View view;

	std::unique_ptr<Camera> cameraSensor;
	float zOffset = 0.0f;
	float previousZOffset = 0.0f;
	float latShift = 0.0f;
	float vertShift = 0.0f;

	//screenshot saving
	char screenShotName[256];
	char screenShotScalePercent[4];
	bool screenshotSaveToSize = false;
	char saveWidth[5];
	char saveHeight[5];
	int listbox_item_current_last;
	std::vector<std::unique_ptr<char[]>> fov_items;
	bool startSavingAll;
	char outputDirectoryPath[256];
	char overlayViewPercent[4];

	//projection settings
	float perspectiveFovyRadians = std::numbers::pi_v<float> / 2;
	bool usePerspectiveProjection = false;
	char orthoLeft[5];
	char orthoRight[5];
	char orthoBottom[5];
	char orthoTop[5];
	char orthoNear[5];
	char orthoFar[5];

	Context(GLFWwindow* window);
	void SaveImage();
	void ReadPathPlan();
	int sync_current_fov_number();
	bool updateListBoxCurrentLast(int fov_selected);
	void saveAllScreenshotsBW(float percent_scale);
	float GetScreenshotScalePercent();
	void SetScreenshotScalePercent(const std::string &percent);
	void SetUsePerspectiveProjection(const std::string& use_perspective_projection);
	void SetPerspetiveAngle(const std::string& perspective_angle);
	void SetOutputDirectoryPath(const std::string& output_directory_path);
	float GetOverlayViewPercent();
	void SetOverlayViewPercent(const std::string& overlay_view_percent);

	int GetSaveWidth();
	int GetSaveHeight();
	float GetOrthoLeft();
	float GetOrthoRight();
	float GetOrthoBottom();
	float GetOrthoTop();
	float GetOrthoNear();
	float GetOrthoFar();
};

