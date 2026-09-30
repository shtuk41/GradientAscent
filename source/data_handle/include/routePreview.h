#pragma once

#include <tuple>
#include <vector>

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <render_object.h>

class RoutePreview : public RenderObject
{
private:
   	GLuint vertex_array_id[1];
    GLuint vertex_buffer[2];
	GLint position_attribute = -1;
	GLint color_attribute = -1;

    std::vector<GLfloat> location;
    std::vector<GLfloat> color;

    std::vector<std::tuple<int, int, float, float, float>> routeData;
public:

    RoutePreview(std::vector<std::tuple<int, int, float, float, float>> &routeData);
    ~RoutePreview();

    void SetProjection(glm::mat4 p);

    virtual void UpdateModel(const glm::mat4& cam_view);
    virtual void Setup();
    virtual void Draw();
};


