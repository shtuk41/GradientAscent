
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include <camera.h>

Camera::Camera(GLFWwindow* w, float speed) : speed(speed), window(w)
{
	g_position = glm::vec3(0, 0, 1000.0f);
	g_initial_fov = glm::pi<float>() * 0.15f;
	g_direction = glm::vec3(0.0f, 0.0f, -1.0f);
	up = glm::vec3(0, 1, 0);
}

void Camera::setOffsetX(const float& offset)
{
	g_position_offset_x = glm::vec3(offset, 0.0f, 0.0f);
}

void Camera::setOffsetY(const float& offset)
{
	g_position_offset_y = glm::vec3(0.0f, offset, 0.0f);
}

void Camera::computeViewProjectionMatrices(float orthoLeft, float orthoRight, float orthoBottom, float orthoTop, float orthoNear, float orthoFar, glm::quat& orientation)
{
	float radius = 5.0f; 
	glm::vec3 target = glm::vec3(0.0f, 0.0f, 0.0f);

	// 1. Calculate camera position by rotating a default offset vector using the quaternion
	g_position = orientation * glm::vec3(0.0f, 0.0f, radius);

	// 2. Extract direction and up vectors from the orientation rotation matrix
	glm::mat4 rotMat = glm::mat4_cast(orientation);
	g_direction = glm::vec3(rotMat * glm::vec4(0.0f, 0.0f, -1.0f, 0.0f));
	glm::vec3 up = glm::vec3(rotMat * glm::vec4(0.0f, 1.0f, 0.0f, 0.0f));

	// 3. Build projection and view matrices looking at (0,0,0)
	g_projection_matrix = glm::ortho(orthoLeft, orthoRight, orthoBottom, orthoTop, orthoNear, orthoFar);
	g_view_matrix = glm::lookAt(g_position, target, up);
}

