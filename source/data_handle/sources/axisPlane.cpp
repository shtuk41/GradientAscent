#include <shaders.h>
#include <axisPlane.h>

#define GLM_ENABLE_EXPERIMENTAL

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

AxisPlane::AxisPlane(glm::vec3 t, glm::vec3 bn, glm::vec4 c, float s) : tangent(t), binormal(bn), color(c), size(s)
{
    normal = glm::cross(tangent, binormal);
}

AxisPlane::~AxisPlane()
{
    glDeleteBuffers(4, vertex_buffer);
    glDeleteVertexArrays(2, vertex_array_id);
    glDisableVertexAttribArray(position_attribute);
    glDisableVertexAttribArray(color_attribute);
}

void AxisPlane::Setup()
{
#ifdef _WIN32	
    program_id = LoadShaders(".\\shaders\\axisPlane.vert", ".\\shaders\\axisPlane.frag");
#elif defined (__linux__)
	program_id = LoadShaders("./shaders/axisPlane.vert", "./shaders/axisPlane.frag");
#endif

    glGenVertexArrays(2, vertex_array_id);
    glBindVertexArray(vertex_array_id[0]);

    float half_size = size / 2;

    glm::vec3 t1 = -tangent * half_size - binormal * half_size;
    glm::vec3 t2 =  tangent * half_size - binormal * half_size;
    glm::vec3 t3 =  tangent * half_size + binormal * half_size;
    glm::vec3 t4 =  tangent * half_size + binormal * half_size;
    glm::vec3 t5 = -tangent * half_size + binormal * half_size;
    glm::vec3 t6 = -tangent * half_size - binormal * half_size;

    GLfloat plane[] = {
                        t1.x, t1.y, t1.z,
                        t2.x, t2.y, t2.z,
                        t3.x, t3.y, t3.z,
                        t4.x, t4.y, t4.z,
                        t5.x, t5.y, t5.z,
                        t6.x, t6.y, t6.z
    };

    GLfloat planeColor[] = {
                                color[0], color[1], color[2], color[3],
                                color[0], color[1], color[2], color[3],
                                color[0], color[1], color[2], color[3],
                                color[0], color[1], color[2], color[3],
                                color[0], color[1], color[2], color[3],
                                color[0], color[1], color[2], color[3]
    };

    glGenBuffers(4, vertex_buffer);

    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(plane), plane, GL_STATIC_DRAW);
    position_attribute = glGetAttribLocation(program_id, "vPosition");
    glVertexAttribPointer(position_attribute, 3, GL_FLOAT, GL_FALSE, 0, (void*)nullptr);
    glEnableVertexAttribArray(position_attribute);

    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(planeColor), planeColor, GL_STATIC_DRAW);
    color_attribute = glGetAttribLocation(program_id, "vColor");
    glVertexAttribPointer(color_attribute, 4, GL_FLOAT, GL_FALSE, 0, (void*)nullptr);
    glEnableVertexAttribArray(color_attribute);

    GLfloat axis_lines_lines[] = {
                                t1.x, t1.y, t1.z,
                                t2.x, t2.y, t2.z,
                                t2.x, t2.y, t2.z,
                                t3.x, t3.y, t3.z,
                                t4.x, t4.y, t4.z,
                                t5.x, t5.y, t5.z,
                                t5.x, t5.y, t5.z,
                                t6.x, t6.y, t6.z
    };

    GLfloat axis_colors_lines[] = {
                                color[0], color[1], color[2], 1.0f,
                                color[0], color[1], color[2], 1.0f,
                                color[0], color[1], color[2], 1.0f,
                                color[0], color[1], color[2], 1.0f,
                                color[0], color[1], color[2], 1.0f,
                                color[0], color[1], color[2], 1.0f,
                                color[0], color[1], color[2], 1.0f,
                                color[0], color[1], color[2], 1.0f
    };

    glBindVertexArray(vertex_array_id[1]);

    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer[2]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(axis_lines_lines), axis_lines_lines, GL_STATIC_DRAW);
    position_attribute = glGetAttribLocation(program_id, "vPosition");
    glVertexAttribPointer(position_attribute, 3, GL_FLOAT, GL_FALSE, 0, (void*)nullptr);
    glEnableVertexAttribArray(position_attribute);

    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer[3]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(axis_colors_lines), axis_colors_lines, GL_STATIC_DRAW);
    color_attribute = glGetAttribLocation(program_id, "vColor");
    glVertexAttribPointer(color_attribute, 4, GL_FLOAT, GL_FALSE, 0, (void*)nullptr);
    glEnableVertexAttribArray(color_attribute);

    model_view = glGetUniformLocation(program_id, "model_view");
    projection = glGetUniformLocation(program_id, "projection");

    model_matrix = glm::mat4(1.0f);
}

void AxisPlane::UpdateModel(const glm::mat4& cam_view)
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

void AxisPlane::SetProjection(glm::mat4 p)
{
    projection_matrix = p;
}

void AxisPlane::Draw()
{
    glUniformMatrix4fv(model_view, 1, GL_FALSE, glm::value_ptr(model_view_matrix));
    glUniformMatrix4fv(projection, 1, GL_FALSE, glm::value_ptr(projection_matrix));

    glBindVertexArray(vertex_array_id[0]);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    glBindVertexArray(vertex_array_id[1]);
    glDrawArrays(GL_LINES, 0, 8);
}
