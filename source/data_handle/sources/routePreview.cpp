#include <shaders.h>
#include <routePreview.h>
#include <utilities.h>

#define GLM_ENABLE_EXPERIMENTAL

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

struct RoadVertex
{
    glm::vec3 left;
    glm::vec3 right;
    RGBA color;
};

RoutePreview::RoutePreview(std::vector<std::tuple<int, int, float, float, float>>& rt) : routeData(rt)
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
    float trackWidth = 10.0f;

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

    float maxGradient = getMaxRouteGradient(routeData);
    size_t n = routeData.size();
    if (n < 2)
    {
        return;
    }

    float halfWidth = trackWidth / 2.0f;

    auto safeNormalize = [](const glm::vec2& v) 
                            {
                                float len = glm::length(v);
                                if (len < 1e-5f) 
                                {
                                    return glm::vec2(1.0f, 0.0f);
                                }
                                    return v / len;
                            };

    auto getPerpendicular = [&](size_t idx1, size_t idx2) 
                        {
                            const auto& p1 = routeData[idx1];
                            const auto& p2 = routeData[idx2];
                            glm::vec2 diff(static_cast<float>(std::get<0>(p2) - std::get<0>(p1)),
                                            static_cast<float>(std::get<1>(p2) - std::get<1>(p1)));
                            glm::vec2 dir = safeNormalize(diff);
                            return glm::vec2(-dir.y, dir.x);
                        };

    std::vector<RoadVertex> roadVertices;
    roadVertices.reserve(n);

    for (size_t i = 0; i < n; ++i)
    {
        glm::vec2 normal;
        if (i == 0)
        {
            normal = getPerpendicular(0, 1);
        }
        else if (i == n - 1)
        {
            normal = getPerpendicular(n - 2, n - 1);
        }
        else
        {
            glm::vec2 n1 = getPerpendicular(i - 1, i);
            glm::vec2 n2 = getPerpendicular(i, i + 1);
            normal = safeNormalize(n1 + n2);
        }

        const auto& pt = routeData[i];
        float t = static_cast<float>(std::get<2>(pt));
        glm::vec3 pos(
            static_cast<float>(std::get<0>(pt)) - offsetX,
            static_cast<float>(std::get<1>(pt)) - offsetY,
            t
        );

        glm::vec3 left = pos + glm::vec3(normal * halfWidth, 0.0f);
        glm::vec3 right = pos - glm::vec3(normal * halfWidth, 0.0f);
        left.z = pos.z;
        right.z = pos.z;

        RGBA col = gradientToColor(std::get<3>(pt), maxGradient);
        roadVertices.push_back({ left, right, col });
    }

    location.reserve(roadVertices.size() * 18);
    color.reserve(roadVertices.size() * 24);

    for (size_t i = 0; i < roadVertices.size() - 1; ++i)
    {
        const auto& curr = roadVertices[i];
        const auto& next = roadVertices[i + 1];

        // Triangle 1: curr.left -> next.left -> next.right
        location.insert(location.end(), { curr.left.x, curr.left.y, curr.left.z });
        color.insert(color.end(), { curr.color.r, curr.color.g, curr.color.b, curr.color.a });

        location.insert(location.end(), { next.left.x, next.left.y, next.left.z });
        color.insert(color.end(), { next.color.r, next.color.g, next.color.b, next.color.a });

        location.insert(location.end(), { next.right.x, next.right.y, next.right.z });
        color.insert(color.end(), { next.color.r, next.color.g, next.color.b, next.color.a });

        // Triangle 2: curr.left -> next.right -> curr.right
        location.insert(location.end(), { curr.left.x, curr.left.y, curr.left.z });
        color.insert(color.end(), { curr.color.r, curr.color.g, curr.color.b, curr.color.a });

        location.insert(location.end(), { next.right.x, next.right.y, next.right.z });
        color.insert(color.end(), { next.color.r, next.color.g, next.color.b, next.color.a });

        location.insert(location.end(), { curr.right.x, curr.right.y, curr.right.z });
        color.insert(color.end(), { curr.color.r, curr.color.g, curr.color.b, curr.color.a });
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
