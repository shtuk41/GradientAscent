#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <render_object.h>

class AxisPlane : public RenderObject
{
private:
   	GLuint vertex_array_id[2];
    GLuint vertex_buffer[4];
	GLint position_attribute = -1;
	GLint color_attribute = -1;
         
    float size;
    glm::vec3 tangent;
    glm::vec3 normal;
    glm::vec3 binormal;
    glm::vec4 color;
    
public:

    AxisPlane(glm::vec3 t, glm::vec3 bn, glm::vec4 c, float s);
    ~AxisPlane();

    float getSize() const { return size; }
    glm::vec3 getTangent() const { return tangent; }
    glm::vec3 getNormal() const { return normal; }
    glm::vec3 getBinormal() const { return binormal; }
    glm::vec4 getColor() const { return color; }
    
    void SetProjection(glm::mat4 p);

    virtual void UpdateModel(const glm::mat4& cam_view);
    virtual void Setup();
    virtual void Draw();
};


