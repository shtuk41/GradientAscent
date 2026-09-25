#pragma once

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

	static void mouse_callback(GLFWwindow* window, double xpos, double ypos)
	{
		if (rotateEnable)
		{
			double delta_x = xpos - previous_xpos;

			float ry = (float)(0.005f * delta_x);
			context->rotateY += ry;

			double delta_y = ypos - previous_ypos;

			float rx = (float)(0.005f * delta_y);
			context->rotateX += rx;

			previous_xpos = xpos;
			previous_ypos = ypos;
		}
	}

	static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
	{
		if (button == GLFW_MOUSE_BUTTON_LEFT)
		{
			if (context->view == View::Global)
			{
				if (action == GLFW_PRESS)
				{
					double xpos, ypos;

					glfwGetCursorPos(window, &xpos, &ypos);

					previous_xpos = xpos;
					previous_ypos = ypos;

					rotateEnable = true;
				}
				else if (action == GLFW_RELEASE)
				{
					rotateEnable = false;
				}
			}
			else
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
			context->rotateX = 0;
			context->rotateY = 0;
			break;
		case GLFW_KEY_A:
			if (context->view == View::Sensor)
			{
			}
			break;
		case GLFW_KEY_D:
			if (context->view == View::Sensor)
			{
			}
			break;
		case GLFW_KEY_C:
			if (context->view == View::Global)
			{
				context->view = View::Sensor;
			}
			else
			{
				context->view = View::Global;
			}
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
			if (context->view == View::Sensor)
			{
			}
			break;
		case GLFW_KEY_W:
			if (context->view == View::Sensor)
			{
			}
			break;
		case GLFW_KEY_UP:
			if (context->view == View::Sensor)
			{
			}
			break;
		case GLFW_KEY_DOWN:
			if (context->view == View::Sensor)
			{
			}
			break;
		case GLFW_KEY_LEFT:
			if (context->view == View::Sensor)
			{
			}
			break;
		case GLFW_KEY_RIGHT:
			if (context->view == View::Sensor)
			{
			}
			break;
		case GLFW_KEY_COMMA:
			if (context->view == View::Sensor)
			{
			}
			break;
		case GLFW_KEY_PERIOD:
			if (context->view == View::Sensor)
			{
			}
			break;
		case GLFW_KEY_PAGE_UP:
			if (context->view == View::Global)
			{
			}
			break;
		case GLFW_KEY_PAGE_DOWN:
			if (context->view == View::Global)
			{
			}
			break;
		}
	}
};

bool Controls::rotateEnable = false;
double Controls::previous_xpos = 0.0f;
double Controls::previous_ypos = 0.0f;
bool Controls::moveback = false;
bool Controls::moveforward = false;






