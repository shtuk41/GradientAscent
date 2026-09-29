#ifndef CAMERA_H
#define CAMERA_H

#include <GLFW/glfw3.h>

#define GLM_ENABLE_EXPERIMENTAL

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

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
    GLFWwindow* window;

    // Quaternion orientation state
    glm::quat orientation{ 1.0f, 0.0f, 0.0f, 0.0f };

    // Helper to sync direction and up vectors from the quaternion each frame
    void updateVectorsFromOrientation()
    {
        glm::mat4 rotMat = glm::mat4_cast(orientation);
        g_direction = glm::vec3(rotMat * glm::vec4(0.0f, 0.0f, -1.0f, 0.0f));
        up = glm::vec3(rotMat * glm::vec4(0.0f, 1.0f, 0.0f, 0.0f));
    }

public:
    Camera(GLFWwindow* w, float speed = 3.0f);

    glm::mat4 getViewMatrix() const { return g_view_matrix; }
    glm::mat4 getProjectionMatrix() const { return g_projection_matrix; }
    glm::vec3 getPosition() const { return g_position; }
    void setPosition(const glm::vec3& current_pos) { g_position = current_pos; }
    float getFov() const { return g_initial_fov; }
    glm::vec3 getDirection() const { return g_direction; }
    void setDirection(const glm::vec3& dir) { g_direction = dir; }
    void SetUp(const glm::vec3& u) { up = u; }

    void setOffsetX(const float& offset);
    void setOffsetY(const float& offset);

    void computeViewProjectionMatrices(float orthoLeft, float orthoRight, float orthoBottom, float orthoTop, float orthoNear, float orthoFar, glm::quat &orientation);
};

#endif /* CAMERA_H */