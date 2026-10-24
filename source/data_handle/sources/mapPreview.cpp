
#define GLM_ENABLE_EXPERIMENTAL

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <shaders.h>
#include <mapPreview.h>

MapPreview::MapPreview(map_container* container) : mapContainer(container)
{

}

MapPreview::~MapPreview()
{
    glDeleteBuffers(2, vertex_buffer);
    glDeleteVertexArrays(1, vertex_array_id);
    glDisableVertexAttribArray(position_attribute);
    glDisableVertexAttribArray(color_attribute);
}

void MapPreview::Setup()
{
#ifdef _WIN32	
    program_id = LoadShaders(".\\shaders\\map.vert", ".\\shaders\\map.frag");
#elif defined (__linux__)
    program_id = LoadShaders("./shaders/map.vert", "./shaders/map.frag");
#endif


}

void MapPreview::UpdateModel(const glm::mat4& cam_view)
{
    SetPosition(0, 0, 0);

    auto mm = glm::translate(model_matrix, glm::vec3(_X, _Y, _Z));

    auto shvec = glm::vec3(0.0f, 0.0f, 1.0f);
    //auto dir = glm::vec3(1.0f, 0.0f, 0.0f);
    auto dir = glm::vec3(-0.08f, 0.025f, 0.99f);
    auto cross = glm::normalize(glm::cross(direction, shvec));
    float theta = glm::acos(glm::dot(direction, shvec));

    //mm = glm::rotate(mm, -theta, cross);
    mm = glm::rotate(mm, -theta, glm::vec3(1, 0, 0));
    model_view_matrix = cam_view * mm;
}

void MapPreview::SetProjection(glm::mat4 p)
{
    projection_matrix = p;
}

void MapPreview::Draw()
{
    glUniformMatrix4fv(model_view, 1, GL_FALSE, glm::value_ptr(model_view_matrix));
    glUniformMatrix4fv(projection, 1, GL_FALSE, glm::value_ptr(projection_matrix));

    glBindVertexArray(vertex_array_id[0]);
    glDrawArrays(GL_TRIANGLES, 0, location.size() / 3);
}


