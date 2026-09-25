#ifndef CAMERA_H
#define CAMERA_H

#include <GLFW/glfw3.h>

#define GLM_ENABLE_EXPERIMENTAL

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

enum View
{
	Global,
	Sensor
};

class Camera
{
private:
    glm::vec3 g_position;  
	glm::vec3 g_position_offset_x = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 g_position_offset_y = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 g_direction;
	glm::vec3 up;
    float speed;
    float g_initial_fov;
    glm::mat4 g_view_matrix;
    glm::mat4 g_projection_matrix;
    GLFWwindow * window;
    
public:
	Camera(GLFWwindow* w, float speed = 3.0f);

    glm::mat4 getViewMatrix() const
    {
        return g_view_matrix;
    }
    
    glm::mat4 getProjectionMatrix() const
    {
        return g_projection_matrix;
    }
    
    glm::vec3 getPosition() const
    {
        return g_position;
    }

	void setPosition(const glm::vec3 &current_pos)
	{
		g_position = current_pos;
	}

	float getFov() const
	{
		return g_initial_fov;
	}

	glm::vec3 getDirection() const
	{
		return g_direction;
	}

	void setDirection(const glm::vec3 &dir)
	{
		g_direction = dir;
	}

	void SetUp(const glm::vec3 &u)
	{
		up = u;
	}

	void rotateX(const float x);
	void rotateY(const float y);
	void rotateZ(const float z);
	void setOffsetX(const float&);
	void setOffsetY(const float&);
	void computeViewProjectionMatrices(bool moveback, bool moveforward);
	//void computeViewProjectionMatrices(bool moveback, bool moveforward, [[maybe_unused]] fov const& currentFov);
	//void computeViewProjectionMatrices(float fovyRadians, [[maybe_unused]] fov const& currentFov);
	void computeViewProjectionMatrices(float orthoLeft, float orthoRight, float orthoBottom, float orthoTop, float orthoNear, float orthoFar);
};


#endif /* CAMERA_H */