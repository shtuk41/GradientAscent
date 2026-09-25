#include <shaders.h>
#include <axes3d.h>

#define GLM_ENABLE_EXPERIMENTAL

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

Axes3d::Axes3d(float x, float y, float z) : xLength(x), yLength(y), zLength(z)
{
}

Axes3d::~Axes3d()
{
    glDeleteBuffers(2, vertex_buffer);
    glDeleteVertexArrays(1, &vertex_array_id);
    glDisableVertexAttribArray(position_attribute);
    glDisableVertexAttribArray(color_attribute);
}

void Axes3d::Setup()
{
#ifdef _WIN32	
    program_id = LoadShaders(".\\shaders\\axis.vert", ".\\shaders\\axis.frag");
#elif defined (__linux__)
	program_id = LoadShaders("./shaders\\axis.vert", "./shaders/axis.frag");
#endif

    glGenVertexArrays(1, &vertex_array_id);

    glBindVertexArray(vertex_array_id);

    GLfloat axis_lines[] = {
                                0.0f,0.0f,0.0f,
                                1.0f * xLength,0.0f,0.0f,
                                0.0f,0.0f,0.0f,
                                0.0f,1.0f * zLength,0.0f,
                                0.0f,0.0f,0.0f,
                                0.0f,0.0f,-1.0f * yLength
    };

    GLfloat axis_colors[] = {
                                1.0f,0.0f,0.0f,
                                1.0f,0.0f,0.0f,
                                0.0f,0.0f,1.0f,
                                0.0f,0.0f,1.0f,
                                0.0f,1.0f,0.0f,
                                0.0f,1.0f,0.0f
    };

    glGenBuffers(2, vertex_buffer);

    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(axis_lines), axis_lines, GL_STATIC_DRAW);
    position_attribute = glGetAttribLocation(program_id, "vPosition");
    glVertexAttribPointer(position_attribute, 3, GL_FLOAT, GL_FALSE, 0, (void*)nullptr);
    glEnableVertexAttribArray(position_attribute);

    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(axis_colors), axis_colors, GL_STATIC_DRAW);
    color_attribute = glGetAttribLocation(program_id, "vColor");
    glVertexAttribPointer(color_attribute, 3, GL_FLOAT, GL_FALSE, 0, (void*)nullptr);
    glEnableVertexAttribArray(color_attribute);

    model_view = glGetUniformLocation(program_id, "model_view");
    projection = glGetUniformLocation(program_id, "projection");

    model_matrix = glm::mat4(1.0f);
}

void Axes3d::UpdateModel(const glm::mat4& cam_view)
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

void Axes3d::SetProjection(glm::mat4 p)
{
    projection_matrix = p;
}

void Axes3d::Draw()
{
    glUniformMatrix4fv(model_view, 1, GL_FALSE, glm::value_ptr(model_view_matrix));
    glUniformMatrix4fv(projection, 1, GL_FALSE, glm::value_ptr(projection_matrix));

    glBindVertexArray(vertex_array_id);
    glDrawArrays(GL_LINES, 0, 6);
}
