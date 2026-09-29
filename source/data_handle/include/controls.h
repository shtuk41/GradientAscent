#pragma once

#define GLM_ENABLE_EXPERIMENTAL

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#include <GLFW/glfw3.h>
#include <memory>
#include <context.h>

extern std::unique_ptr<Context> context;

class Controls
{
public:
	static double previous_xpos;
	static double previous_ypos;
	static bool rotateEnable;
	static bool moveback;
	static bool moveforward;
	static bool key_w;
	static bool key_s;

	static void mouse_callback(GLFWwindow* window, double xpos, double ypos)
	{
		if (rotateEnable)
		{
			double delta_x = xpos - previous_xpos;
			double delta_y = ypos - previous_ypos;

			float sensitivity = 0.005f;

			float angleX = (float)(delta_y * sensitivity); // Pitch
			float angleY = (float)(delta_x * sensitivity); // Yaw

			// 1. Extract BOTH local axes from the current orientation matrix
			glm::mat4 rotMat = glm::mat4_cast(context->orientation);
			glm::vec3 localRight = glm::vec3(rotMat[0]); // Local X axis
			glm::vec3 localUp = glm::vec3(rotMat[1]); // Local Y axis (the "new Y")

			// 2. Create incremental rotations around the camera's actual local axes
			glm::quat pitchRot = glm::angleAxis(angleX, localRight);
			glm::quat yawRot = glm::angleAxis(angleY, localUp);

			// 3. Apply local rotations 
			context->orientation = yawRot * pitchRot * context->orientation;
			context->orientation = glm::normalize(context->orientation);

			previous_xpos = xpos;
			previous_ypos = ypos;
		}
	}

	static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
	{
		if (button == GLFW_MOUSE_BUTTON_LEFT)
		{
			if (action == GLFW_PRESS)
			{
				glfwGetCursorPos(window, &previous_xpos, &previous_ypos);
				rotateEnable = true;
			}
			else if (action == GLFW_RELEASE)
			{
				rotateEnable = false;
			}
		}
	}

	static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
	{
		if (yoffset >= 1.0)
		{
			moveback = true;
			moveforward = false;
		}
		else if (yoffset <= 1.0)
		{
			moveback = false;
			moveforward = true;
		}
	}

	static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		if (action != GLFW_PRESS && action != GLFW_REPEAT)
			return;

		switch (key)
		{
		case GLFW_KEY_ESCAPE:
			glfwSetWindowShouldClose(window, GL_TRUE);
			break;
		case GLFW_KEY_SPACE:
			break;
		case GLFW_KEY_A:
			break;
		case GLFW_KEY_D:
			break;
		case GLFW_KEY_C:
			break;
		case GLFW_KEY_O:
			context->saveAllScreenshotsBW(10.0f);
			break;
		case GLFW_KEY_I:
			context->startSavingAll = false;
			break;
		case GLFW_KEY_U:
			context->startSavingAll = true;
		case GLFW_KEY_P:
			context->SaveImage();
			break;
		case GLFW_KEY_S:
			key_s = true;
			break;
		case GLFW_KEY_W:
			key_w = true;
			break;
		case GLFW_KEY_UP:
			break;
		case GLFW_KEY_DOWN:
			break;
		case GLFW_KEY_LEFT:
			break;
		case GLFW_KEY_RIGHT:
			break;
		case GLFW_KEY_COMMA:
			break;
		case GLFW_KEY_PERIOD:
			break;
		case GLFW_KEY_PAGE_UP:
			break;
		case GLFW_KEY_PAGE_DOWN:
			break;
		}
	}
};

bool Controls::rotateEnable = false;
double Controls::previous_xpos = 0.0f;
double Controls::previous_ypos = 0.0f;
bool Controls::moveback = false;
bool Controls::moveforward = false;
bool Controls::key_w = false;
bool Controls::key_s = false;






