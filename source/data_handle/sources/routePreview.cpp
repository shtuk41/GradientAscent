#include <shaders.h>
#include <routePreview.h>

#define GLM_ENABLE_EXPERIMENTAL

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

RoutePreview::RoutePreview(std::vector<std::tuple<int, int, float>>& rt) : routeData(rt)
{
}

RoutePreview::~RoutePreview()
{
    glDeleteBuffers(2, vertex_buffer);
    glDeleteVertexArrays(1, vertex_array_id);
    glDisableVertexAttribArray(position_attribute);
    glDisableVertexAttribArray(color_attribute);
}

void RoutePreview::Setup()
{
#ifdef _WIN32	
    program_id = LoadShaders(".\\shaders\\axisPlane.vert", ".\\shaders\\axisPlane.frag");
#elif defined (__linux__)
	program_id = LoadShaders("./shaders/axisPlane.vert", "./shaders/axisPlane.frag");
#endif

    float offsetX = 0.0f;
    float offsetY = 0.0f;

    if (!routeData.empty())
    {
        float sumX = 0.0f;
        float sumY = 0.0f;

        for (const auto& point : routeData)
        {
            sumX += std::get<0>(point);
            sumY += std::get<1>(point);
        }

        offsetX = sumX / routeData.size();
        offsetY = sumY / routeData.size();
    }


    location.reserve(routeData.size() * 18);
    color.reserve(routeData.size() * 24);

    auto t1 = routeData.front();

    float scale = 0.1f;

    float prevReal = 0.0f;

    std::vector<GLfloat> green = {0.0f, 1.0f, 0.0f, 1.0};
   
    for (auto it = routeData.begin() + 1; it != routeData.end(); ++it)
    {
        auto t2 = *it;

        location.push_back(std::get<0>(t1) - offsetX);
        location.push_back(std::get<1>(t1) - offsetY);
        location.push_back(0);

        color.insert(color.end(), green.begin(), green.end());

        location.push_back(std::get<0>(t2) - offsetX);
        location.push_back(std::get<1>(t2) - offsetY);
        location.push_back(0);

        color.insert(color.end(), green.begin(), green.end());

        location.push_back(std::get<0>(t1) - offsetX);
        location.push_back(std::get<1>(t1) - offsetY);

        float t = std::get<2>(t1);
        
        if (t < 0)
            t = prevReal;
        else
            prevReal = t;

        location.push_back(t * scale);

        color.insert(color.end(), green.begin(), green.end());

        location.push_back(std::get<0>(t1) - offsetX);
        location.push_back(std::get<1>(t1) - offsetY);
        location.push_back(t * scale);

        color.insert(color.end(), green.begin(), green.end());

        location.push_back(std::get<0>(t2) - offsetX);
        location.push_back(std::get<1>(t2) - offsetY);
        location.push_back(0);

        color.insert(color.end(), green.begin(), green.end());

        location.push_back(std::get<0>(t2) - offsetX);
        location.push_back(std::get<1>(t2) - offsetY);

        t = std::get<2>(t2);

        if (t < 0)
            t = prevReal;
        else
            prevReal = t;


        location.push_back(t * scale);

        color.insert(color.end(), { 0.0f, 0.0f, 0.0f, 1.0f });

        t1 = t2;
    }

    glGenVertexArrays(1, vertex_array_id);
    glBindVertexArray(vertex_array_id[0]);

    glGenBuffers(2, vertex_buffer);

    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer[0]);
    glBufferData(GL_ARRAY_BUFFER, location.size() * sizeof(GLfloat), location.data(), GL_STATIC_DRAW);
    position_attribute = glGetAttribLocation(program_id, "vPosition");
    glVertexAttribPointer(position_attribute, 3, GL_FLOAT, GL_FALSE, 0, (void*)nullptr);
    glEnableVertexAttribArray(position_attribute);

    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer[1]);
    glBufferData(GL_ARRAY_BUFFER, color.size() * sizeof(GLfloat), color.data(), GL_STATIC_DRAW);
    color_attribute = glGetAttribLocation(program_id, "vColor");
    glVertexAttribPointer(color_attribute, 4, GL_FLOAT, GL_FALSE, 0, (void*)nullptr);
    glEnableVertexAttribArray(color_attribute);

    model_view = glGetUniformLocation(program_id, "model_view");
    projection = glGetUniformLocation(program_id, "projection");

    model_matrix = glm::mat4(1.0f);
}

void RoutePreview::UpdateModel(const glm::mat4& cam_view)
{
    SetPosition(0, 0, 0);

    auto mm = glm::translate(model_matrix, glm::vec3(_X, _Y, _Z));

    auto shvec = glm::vec3(0.0f, 0.0f, 1.0f);
    //auto dir = glm::vec3(1.0f, 0.0f, 0.0f);
    auto dir = glm::vec3(-0.08f, 0.025f, 0.99f);
    auto cross = glm::normalize(glm::cross(direction, shvec));
    float theta = glm::acos(glm::dot(direction, shvec));

    //mm = glm::rotate(mm, -theta, cross);
    mm = glm::rotate(mm, -theta, glm::vec3(1,0,0));
    model_view_matrix = cam_view * mm;
}

void RoutePreview::SetProjection(glm::mat4 p)
{
    projection_matrix = p;
}

void RoutePreview::Draw()
{
    glUniformMatrix4fv(model_view, 1, GL_FALSE, glm::value_ptr(model_view_matrix));
    glUniformMatrix4fv(projection, 1, GL_FALSE, glm::value_ptr(projection_matrix));

    glBindVertexArray(vertex_array_id[0]);
    glDrawArrays(GL_TRIANGLES, 0, location.size() / 3);
}
