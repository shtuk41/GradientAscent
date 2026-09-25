#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <render_object.h>

class Axes3d : public RenderObject
{
private:
   	GLuint vertex_array_id = -1;
    GLuint vertex_buffer[2];
	GLint position_attribute = -1;
	GLint color_attribute = -1;
         
    float xLength;
    float yLength;
    float zLength;
    
public:

    Axes3d(float x, float y, float z);
    ~Axes3d();
    
    void SetProjection(glm::mat4 p);

    virtual void UpdateModel(const glm::mat4& cam_view);
    virtual void Setup();
    virtual void Draw();
};


