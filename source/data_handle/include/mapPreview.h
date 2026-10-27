#pragma once

#include <tuple>
#include <vector>

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <render_object.h>

#include <mapContainer.h>

class MapPreview : public RenderObject
{
private:
    GLuint vertex_array_id[1];
    GLuint vertex_buffer[2];
    GLint position_attribute = -1;
    GLint color_attribute = -1;

    std::vector<glm::vec3> location;
    std::vector<glm::vec4> color;
    map_container* mapContainer;
    std::vector<std::tuple<int, int, float, float, float>>* routeData;


public:

    MapPreview(map_container*  container, std::vector<std::tuple<int, int, float, float, float>>* rt);
    ~MapPreview();

    void SetProjection(glm::mat4 p);

    virtual void UpdateModel(const glm::mat4& cam_view);
    virtual void Setup();
    virtual void Draw();
};