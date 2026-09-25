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

Context::Context(GLFWwindow* window) : rotateX(0.0f), rotateY(0.0f), view(View::Global), startSavingAll(false)
{
	cameraSensor = std::make_unique<Camera>(window);
	std::strncpy(screenShotName, "notdefinedfilename.png",sizeof(screenShotName)-1);
	std::strncpy(screenShotScalePercent, "0", sizeof(screenShotScalePercent)-1);
	std::strncpy(saveWidth, "7920",sizeof(saveWidth)-1);
	std::strncpy(saveHeight, "6004",sizeof(saveHeight)-1);
	std::strncpy(orthoLeft, "-26",sizeof(orthoLeft)-1);
	std::strncpy(orthoRight, "26",sizeof(orthoRight)-1);
	std::strncpy(orthoBottom, "-20",sizeof(orthoBottom)-1);
	std::strncpy(orthoTop, "20",sizeof(orthoTop)-1);
	std::strncpy(orthoNear, "1",sizeof(orthoNear)-1);
	std::strncpy(orthoFar, "9999",sizeof(orthoFar)-1);
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

void Context::SetUsePerspectiveProjection(const std::string& use_perspetive_projection)
{
	usePerspectiveProjection = use_perspetive_projection.compare("true") == 0;
}

void Context::SetPerspetiveAngle(const std::string& perspetive_angle)
{
	std::stringstream iss(perspetive_angle);
	float angle;
	iss >> angle;
	bool valid = iss.eof() && !iss.fail() && angle > 0 && angle <= std::numbers::pi_v<float>;

	perspectiveFovyRadians = valid ? static_cast<float>(angle) : std::numbers::pi_v<float> / 2;
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
	std::stringstream iss(orthoLeft);
	int orthoLeftInteger;
	iss >> orthoLeftInteger;
	bool valid = iss.eof() && !iss.fail() && orthoLeftInteger >= -100 && orthoLeftInteger <= 100;

	return valid ? static_cast<float>(orthoLeftInteger) : -26.0f;
}

float Context::GetOrthoRight()
{
	std::stringstream iss(orthoRight);
	int orthoRightInteger;
	iss >> orthoRightInteger;
	bool valid = iss.eof() && !iss.fail() && orthoRightInteger >= -100 && orthoRightInteger <= 100;

	return valid ? static_cast<float>(orthoRightInteger) : 26.0f;
}

float Context::GetOrthoBottom()
{
	std::stringstream iss(orthoBottom);
	int orthoBottomInteger;
	iss >> orthoBottomInteger;
	bool valid = iss.eof() && !iss.fail() && orthoBottomInteger >= -100 && orthoBottomInteger <= 100;

	return valid ? static_cast<float>(orthoBottomInteger) : -20.0f;
}

float Context::GetOrthoTop()
{
	std::stringstream iss(orthoTop);
	int orthoTopInteger;
	iss >> orthoTopInteger;
	bool valid = iss.eof() && !iss.fail() && orthoTopInteger >= -100 && orthoTopInteger <= 100;

	return valid ? static_cast<float>(orthoTopInteger) : 20.0f;
}

float Context::GetOrthoNear()
{
	std::stringstream iss(orthoNear);
	int orthoNearInteger;
	iss >> orthoNearInteger;
	bool valid = iss.eof() && !iss.fail() && orthoNearInteger >= -10000 && orthoNearInteger <= 10000;

	return valid ? static_cast<float>(orthoNearInteger) : 1.0f;
}

float Context::GetOrthoFar()
{
	std::stringstream iss(orthoFar);
	int orthoFarInteger;
	iss >> orthoFarInteger;
	bool valid = iss.eof() && !iss.fail() && orthoFarInteger >=-10000  && orthoFarInteger <= 10000;

	return valid ? static_cast<float>(orthoFarInteger) : 9999.0f;
}




