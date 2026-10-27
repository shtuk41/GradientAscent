
//#define _MAP_PREVIEW_DEBUG

#ifdef _MAP_PREVIEW_DEBUG
#include <format>
#include <iostream>
#endif

#define GLM_ENABLE_EXPERIMENTAL

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <shaders.h>
#include <mapPreview.h>

const float defined_height = 50.0f;
const glm::vec4 houseColor = { 1.0f, 1.0f, 1.0f, 1.0f };

MapPreview::MapPreview(map_container* container, std::vector<std::tuple<int, int, float, float, float>>* rt) 
                    : mapContainer(container), routeData(rt)
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
//#ifdef _WIN32	
//    program_id = LoadShaders(".\\shaders\\map.vert", ".\\shaders\\map.frag");
//#elif defined (__linux__)
//    program_id = LoadShaders("./shaders/map.vert", "./shaders/map.frag");
//#endif

//TODO: temporary
#ifdef _WIN32	
    program_id = LoadShaders(".\\shaders\\axisPlane.vert", ".\\shaders\\axisPlane.frag");
#elif defined (__linux__)
    program_id = LoadShaders("./shaders/axisPlane.vert", "./shaders/axisPlane.frag");
#endif

    // std::vector<GLfloat> location;
    //std::vector<GLfloat> color;
    float offsetX = 0.0f;
    float offsetY = 0.0f;
    float trackWidth = 100.0f;
    glm::vec3 current(0.0f, 0.0f, 0.0f);
    glm::vec3 prev(0.0f, 0.0f, 0.0f);

    if (!routeData->empty())
    {
        float sumX = 0.0f;
        float sumY = 0.0f;

        for (const auto& point : *routeData)
        {
            sumX += std::get<0>(point);
            sumY += std::get<1>(point);
        }

        offsetX = sumX / (*routeData).size();
        offsetY = sumY / (*routeData).size();
    }

    if (mapContainer->polygons.size() > 0)
    {
        size_t  polygonNumber = 0;

        for (auto& poly : mapContainer->polygons)
        {
            for (size_t pointIndex = 0; pointIndex < poly.getNumberOfPoints(); pointIndex++)
            {
                auto coordinates = poly.getProjectedCoordinatesAt(pointIndex);
#ifdef _MAP_PREVIEW_DEBUG
                std::cout << std::format("Poly {}, PointIndex {} : {},{},{}\n", 
                                                    polygonNumber, 
                                                    pointIndex,
                                                    std::get<0>(coordinates) - offsetX,
                                                    -std::get<1>(coordinates) - offsetY,
                                                    std::get<2>(coordinates));
#endif
            }

            polygonNumber++;
        }
    }

    if (mapContainer->multipolygons.size() > 0)
    {
        size_t multiPolyNum = 0;

        for (auto& multipoly : mapContainer->multipolygons)
        {
            size_t numPolygons = multipoly.getNumberOfPolygons();

            for (size_t polyIndex  = 0; polyIndex < numPolygons; polyIndex++)
            {
                auto poly = multipoly.getPolygonAt(polyIndex);

                

                for (size_t pointIndex = 0; pointIndex < poly.getNumberOfPoints(); pointIndex++)
                {
                    auto coordinates = poly.getProjectedCoordinatesAt(pointIndex);
#ifdef _MAP_PREVIEW_DEBUG
                    std::cout << std::format("multiPolyNum {} Poly {}, index {}: {},{},{}\n", 
                                                multiPolyNum,
                                                polyIndex, 
                                                pointIndex, 
                                                std::get<0>(coordinates) - offsetX,
                                                -std::get<1>(coordinates) - offsetY,
                                                std::get<2>(coordinates));
#endif
                    current = glm::vec3(std::get<0>(coordinates) - offsetX,
                                -std::get<1>(coordinates) - offsetY,
                                std::get<2>(coordinates) * 10);

                    if (pointIndex > 0)
                    {
                        glm::vec3 h1 = prev;
                        h1.z += defined_height;

                        glm::vec3 h2 = current;
                        h2.z += defined_height;

                        location.push_back(prev);
                        location.push_back(current);
                        location.push_back(h1);

                        location.push_back(h1);
                        location.push_back(current);
                        location.push_back(h2);

                        color.push_back(houseColor);
                        color.push_back(houseColor);
                        color.push_back(houseColor);
                        color.push_back(houseColor);
                        color.push_back(houseColor);
                        color.push_back(houseColor);
                    }

                    prev = current;

                }
            }

            multiPolyNum++;
        }
    }

    glGenVertexArrays(1, vertex_array_id);
    glBindVertexArray(vertex_array_id[0]);

    glGenBuffers(2, vertex_buffer);

    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer[0]);
    glBufferData(GL_ARRAY_BUFFER, location.size() * sizeof(glm::vec3), location.data(), GL_STATIC_DRAW);
    position_attribute = glGetAttribLocation(program_id, "vPosition");
    glVertexAttribPointer(position_attribute, 3, GL_FLOAT, GL_FALSE, 0, (void*)nullptr);
    glEnableVertexAttribArray(position_attribute);

    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer[1]);
    glBufferData(GL_ARRAY_BUFFER, color.size() * sizeof(glm::vec4), color.data(), GL_STATIC_DRAW);
    color_attribute = glGetAttribLocation(program_id, "vColor");
    glVertexAttribPointer(color_attribute, 4, GL_FLOAT, GL_FALSE, 0, (void*)nullptr);
    glEnableVertexAttribArray(color_attribute);

    model_view = glGetUniformLocation(program_id, "model_view");
    projection = glGetUniformLocation(program_id, "projection");

    model_matrix = glm::mat4(1.0f);

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
    glDrawArrays(GL_TRIANGLES, 0, location.size());
}


