#include <camera.h>

Camera::Camera(GLFWwindow* w, float speed) : speed(speed), window(w)
{
	g_position = glm::vec3(0, 0, 1000.0f);
	g_initial_fov = glm::pi<float>() * 0.15f;
	g_direction = glm::vec3(0.0f, 0.0f, -1.0f);
	up = glm::vec3(0, 1, 0);
}

void Camera::rotateX(const float x)
{
	auto m = glm::rotate(glm::mat4(1.0f), x, glm::vec3(-1.0f, 0.0f, 0.0f));
	g_position = m * glm::vec4(g_position, 1.0f);
	g_direction = glm::vec3(0.0f, 0.0f, 0.0f) - g_position;
}

void Camera::rotateY(const float y)
{
	auto m = glm::rotate(glm::mat4(1.0f), y, glm::vec3(0.0f, -1.0f, 0.0f));
	g_position = m * glm::vec4(g_position, 1.0f);
	g_direction = glm::vec3(0.0f, 0.0f, 0.0f) - g_position;
}

void  Camera::rotateZ(const float z)
{
	auto m = glm::rotate(glm::mat4(1.0f), glm::pi<float>() * z / 180.0f, glm::vec3(0.0f, 0.0f, 1.0f));
}

void Camera::setOffsetX(const float& offset)
{
	g_position_offset_x = glm::vec3(offset, 0.0f, 0.0f);
}

void Camera::setOffsetY(const float& offset)
{
	g_position_offset_y = glm::vec3(0.0f, offset, 0.0f);
}


void Camera::computeViewProjectionMatrices(bool moveback, bool moveforward)
{
	static double last_time = glfwGetTime();

	double current_time = glfwGetTime();
	float delta_time = float(current_time - last_time);

	int width, height;
	glfwGetWindowSize(window, &width, &height);

	if (width <= 0 || height <= 0)
		return;

	if (false && glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
	{
		float extraspeed = (moveback ? 250.0f : 100.0f);

		g_position += g_direction * delta_time * speed * extraspeed;
	}
	else if (false && glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
	{
		float extraspeed = (moveforward ? 250.0f : 100.0f);

		g_position -= g_direction * delta_time * speed * extraspeed;
	}
	else if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS || moveback)
	{
		g_initial_fov -= 0.1f * delta_time * speed;
		if (g_initial_fov < 0.001f)
			g_initial_fov = 0.001f;
	}
	else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS || moveforward)
	{
		g_initial_fov += 0.1f * delta_time * speed;
		if (g_initial_fov > 0.800f)
			g_initial_fov = 0.800f;
	}

	g_projection_matrix = glm::perspective(g_initial_fov, (float)width / (float)height, 0.1f, 10000.0f);

	//update the view matrix
	g_view_matrix = glm::lookAt(g_position + g_position_offset_x + g_position_offset_y, g_position + g_position_offset_x + g_position_offset_y + g_direction, up);
	last_time = current_time;
}


void Camera::computeViewProjectionMatrices(float orthoLeft, float orthoRight, float orthoBottom, float orthoTop, float orthoNear, float orthoFar)
{
	g_projection_matrix = glm::ortho(orthoLeft, orthoRight, orthoBottom, orthoTop, orthoNear, orthoFar);

	g_view_matrix = glm::lookAt(g_position, g_position + g_direction, up);
}

