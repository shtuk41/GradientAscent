#include <GL/glew.h>

#define GLM_ENABLE_EXPERIMENTAL

#include <glm/glm.hpp>
#include <glm/gtx/vector_angle.hpp>

#include <cstring>
#include <memory>
#include <sstream>
#include <string>

#include <context.h>
#include <optionsreader.h>

Context::Context(GLFWwindow* window) : startSavingAll(false)
{
	orientation = { 1.0f, 0.0f, 0.0f, 0.0f };
	cameraSensor = std::make_unique<Camera>(window);
	std::strncpy(screenShotName, "notdefinedfilename.png",sizeof(screenShotName)-1);
	std::strncpy(screenShotScalePercent, "0", sizeof(screenShotScalePercent)-1);
	std::strncpy(saveWidth, "7920",sizeof(saveWidth)-1);
	std::strncpy(saveHeight, "6004",sizeof(saveHeight)-1);
	orthoLeft = -10000;
	orthoRight = 10000;
	orthoBottom = -10000;
	orthoTop = 10000;
	orthoNear = -10000;
	orthoFar = 30000;
}

float Context::GetScreenshotScalePercent()
{
	std::stringstream iss(screenShotScalePercent);
	int percentInteger;
	iss >> percentInteger;
	bool valid = iss.eof() && !iss.fail() && percentInteger > 0 && percentInteger <= 100;

	return valid ? static_cast<float>(percentInteger) : 100.0f;
}

void Context::SetScreenshotScalePercent(const std::string &percent)
{
	memset(screenShotScalePercent, 0, 4);
	size_t nc = percent.length() > 3 ? 3 : percent.length();

	percent.copy(screenShotScalePercent, nc);
}

float Context::GetOverlayViewPercent()
{
	std::stringstream iss(overlayViewPercent);
	int percentInteger;
	iss >> percentInteger;
	bool valid = iss.eof() && !iss.fail() && percentInteger > 0 && percentInteger <= 100;

	return valid ? static_cast<float>(percentInteger) : 100.0f;
}

void Context::SetOverlayViewPercent(const std::string& ovp)
{
	memset(overlayViewPercent, 0, 4);
	size_t nc = ovp.length() > 3 ? 3 : ovp.length();

	ovp.copy(overlayViewPercent, nc);
}

void Context::SetOutputDirectoryPath(const std::string& output_directory_path)
{
	memset(outputDirectoryPath, 0, 256);
	output_directory_path.copy(outputDirectoryPath, 255);
}

int Context::GetSaveWidth()
{
	std::stringstream iss(saveWidth);
	int widthInteger;
	iss >> widthInteger;
	bool valid = iss.eof() && !iss.fail() && widthInteger > 0 && widthInteger < 10000;

	return valid ? widthInteger : 7920;
}

int Context::GetSaveHeight()
{
	std::stringstream iss(saveHeight);
	int heightInteger;
	iss >> heightInteger;
	bool valid = iss.eof() && !iss.fail() && heightInteger > 0 && heightInteger <= 10000;

	return valid ? heightInteger : 6004;
}

void Context::SaveImage()
{
	std::string tmp = screenShotName;
	
	if (tmp.length() > 4 && (tmp.find(".bmp") >= 0 || tmp.find(".png") >= 0))
	{
		if (screenshotSaveToSize)
		{
			int width = GetSaveWidth();
			int height = GetSaveHeight();

		}
		else
		{
			float percent = GetScreenshotScalePercent();
		}
	}
	else
	{
		if (screenshotSaveToSize)
		{
			int width = GetSaveWidth();
			int height = GetSaveHeight();

		}
		else
		{
			float percent = GetScreenshotScalePercent();
		}
	}
}

int Context::sync_current_fov_number()
{
	return listbox_item_current_last;
}

bool Context::updateListBoxCurrentLast(int fov_selected)
{
	bool updated = fov_selected != listbox_item_current_last;

	if (updated)
	{
		int fovNumber = std::atoi(fov_items[fov_selected].get());
		listbox_item_current_last = fov_selected;
	}
	
	return updated;
}

void Context::saveAllScreenshotsBW(float percent_scale)
{
	startSavingAll = true;
}

float Context::GetOrthoLeft()
{
	return orthoLeft;
}

float Context::GetOrthoRight()
{
	return orthoRight;
}

float Context::GetOrthoBottom()
{
	return orthoBottom;
}

float Context::GetOrthoTop()
{
	return orthoTop;
}

float Context::GetOrthoNear()
{
	return orthoNear;
}

float Context::GetOrthoFar()
{
	return orthoFar;
}




