#include <shaders.h>
#include <volume.h>

#define GLM_ENABLE_EXPERIMENTAL

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

Volume::Volume(int x, int y, int z, Camera* c) : xLength(x), yLength(y), zLength(z), cam(c)
{
 
}

Volume::~Volume()
{
    glDeleteBuffers(1, &vertex_buffer);
    glDeleteVertexArrays(1, &vertex_array_id);
}

void fillCylinder(std::vector<GLubyte>& volumeData, int width, int height, int depth)
{
    float cx = width / 2.0f;
    float cy = height / 2.0f;
    float radius = width * 0.3f; // 30% of width

    for (int z = 0; z < depth; ++z) {
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                float dx = x - cx;
                float dy = y - cy;
                if ((dx * dx + dy * dy) <= (radius * radius)) {
                    volumeData[x + y * width + z * width * height] = 255;
                }
            }
        }
    }
}

void fillSphere(std::vector<GLubyte>& volumeData, int width, int height, int depth)
{
    float cx = width / 2.0f;
    float cy = height / 2.0f;
    float cz = depth / 2.0f;
    float radius = 150.0f;

    for (int z = 0; z < depth; ++z) {
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                float dx = x - cx;
                float dy = y - cy;
                float dz = z - cz;
                float dist2 = dx * dx + dy * dy + dz * dz;
                if (dist2 < radius * radius) {
                    volumeData[x + y * width + z * width * height] = 255;
                }
                else {
                    volumeData[x + y * width + z * width * height] = 0;
                }
            }
        }
    }
}

void fillTorus(std::vector<GLubyte>& volumeData, int width, int height, int depth)
{
    float cx = width / 2.0f;
    float cy = height / 2.0f;
    float cz = depth / 2.0f;

    float R = 200.0f;  // major radius
    float r = 50.0f;   // minor radius

    for (int z = 0; z < depth; ++z) {
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                float dx = x - cx;
                float dy = y - cy;
                float dz = z - cz;

                float xyLen = sqrt(dx * dx + dy * dy);
                float dist = (xyLen - R) * (xyLen - R) + dz * dz;

                if (dist < r * r) {
                    volumeData[x + y * width + z * width * height] = 255;
                }
            }
        }
    }
}

void fillKleinBottle(std::vector<GLubyte>& volumeData, int width, int height, int depth)
{
    int w = width, h = height, d = depth;
    auto idx = [&](int x, int y, int z) {
        return x + y * w + z * w * h;
        };

    const float pi = 3.14159265f;

    for (float u = 0; u < 2.0f * pi; u += 0.015f) {
        for (float v = 0; v < 2.0f * pi; v += 0.015f) {
            float r = 2.0f;

            float x = (r + cos(u / 2.0f) * sin(v) - sin(u / 2.0f) * sin(2.0f * v)) * cos(u);
            float y = (r + cos(u / 2.0f) * sin(v) - sin(u / 2.0f) * sin(2.0f * v)) * sin(u);
            float z = sin(u / 2.0f) * sin(v) + cos(u / 2.0f) * sin(2.0f * v);

            // Scale and center in volume
            int xi = static_cast<int>((x * 50.0f) + w / 2);
            int yi = static_cast<int>((y * 50.0f) + h / 2);
            int zi = static_cast<int>((z * 50.0f) + d / 2);

            for (int dz = -1; dz <= 1; ++dz)
                for (int dy = -1; dy <= 1; ++dy)
                    for (int dx = -1; dx <= 1; ++dx) {
                        int nx = xi + dx, ny = yi + dy, nz = zi + dz;
                        if (nx >= 0 && ny >= 0 && nz >= 0 && nx < w && ny < h && nz < d)
                            volumeData[idx(nx, ny, nz)] = 255;
                    }
        }
    }
}

void fillKleinBottle2(std::vector<GLubyte>& volumeData, int width, int height, int depth)
{
    int w = width, h = height, d = depth;
    auto idx = [&](int x, int y, int z) {
        return x + y * w + z * w * h;
        };

    for (float u = 0; u < 2.0f * 3.14159f; u += 0.02f) {
        for (float v = 0; v < 2.0f * 3.14159f; v += 0.02f) {
            float r = 4.0f * (1 - cos(u) / 2.0f);

            float x = (6.0f * cos(u) * (1 + sin(u))) + r * cos(v);
            float y = (16.0f * sin(u)) + r * sin(v);
            float z = -2.0f * sin(u);

            // Center and scale into volume
            int xi = static_cast<int>(x * 10 + w / 2);
            int yi = static_cast<int>(y * 10 + h / 2);
            int zi = static_cast<int>(z * 10 + d / 2);

            // Draw a small sphere around each point
            for (int dz = -1; dz <= 1; ++dz)
                for (int dy = -1; dy <= 1; ++dy)
                    for (int dx = -1; dx <= 1; ++dx) {
                        int nx = xi + dx, ny = yi + dy, nz = zi + dz;
                        if (nx >= 0 && ny >= 0 && nz >= 0 && nx < w && ny < h && nz < d)
                            volumeData[idx(nx, ny, nz)] = 255;
                    }
        }
    }
}

void fillKleinBottleFigure8(std::vector<GLubyte>& volumeData, int width, int height, int depth) {
    int w = width, h = height, d = depth;
    auto idx = [&](int x, int y, int z) {
        return x + y * w + z * w * h;
        };

    for (float u = 0; u < 2.0f * 3.14159f; u += 0.02f) {
        for (float v = 0; v < 2.0f * 3.14159f; v += 0.02f) {
            float r = 2.0f;

            float x = (r + cos(u / 2.0f) * sin(v) - sin(u / 2.0f) * sin(2.0f * v)) * cos(u);
            float y = (r + cos(u / 2.0f) * sin(v) - sin(u / 2.0f) * sin(2.0f * v)) * sin(u);
            float z = sin(u / 2.0f) * sin(v) + cos(u / 2.0f) * sin(2.0f * v);

            // Apply figure-8 warping
            x += 0.5f * cos(2 * u);
            y += 0.5f * sin(3 * v);
            z += 0.3f * cos(u + v);

            int xi = static_cast<int>(x * 10 + w / 2);
            int yi = static_cast<int>(y * 10 + h / 2);
            int zi = static_cast<int>(z * 10 + d / 2);

            for (int dz = -1; dz <= 1; ++dz)
                for (int dy = -1; dy <= 1; ++dy)
                    for (int dx = -1; dx <= 1; ++dx) {
                        int nx = xi + dx, ny = yi + dy, nz = zi + dz;
                        if (nx >= 0 && ny >= 0 && nz >= 0 && nx < w && ny < h && nz < d)
                            volumeData[idx(nx, ny, nz)] = 255;
                    }
        }
    }
}

void fillKleinBottleSwirl(std::vector<GLubyte>& volumeData, int width, int height, int depth) {
    int w = width, h = height, d = depth;
    auto idx = [&](int x, int y, int z) {
        return x + y * w + z * w * h;
        };

    for (float u = 0; u < 2.0f * 3.14159f; u += 0.015f) {
        for (float v = 0; v < 2.0f * 3.14159f; v += 0.015f) {
            // Radius changes with sin(3u) for swirl effect
            float r = 4.0f + 2.0f * sin(3.0f * u);

            float x = (r + cos(u / 2.0f) * sin(v) - sin(u / 2.0f) * sin(2.0f * v)) * cos(u);
            float y = (r + cos(u / 2.0f) * sin(v) - sin(u / 2.0f) * sin(2.0f * v)) * sin(u);
            float z = sin(u / 2.0f) * sin(v) + cos(u / 2.0f) * sin(2.0f * v);

            int xi = static_cast<int>(x * 7 + w / 2);
            int yi = static_cast<int>(y * 7 + h / 2);
            int zi = static_cast<int>(z * 7 + d / 2);

            for (int dz = -1; dz <= 1; ++dz)
                for (int dy = -1; dy <= 1; ++dy)
                    for (int dx = -1; dx <= 1; ++dx) {
                        int nx = xi + dx, ny = yi + dy, nz = zi + dz;
                        if (nx >= 0 && ny >= 0 && nz >= 0 && nx < w && ny < h && nz < d)
                            volumeData[idx(nx, ny, nz)] = 255;
                    }
        }
    }
}

void fillMengerSponge(std::vector<GLubyte>& volumeData, int width, int height, int depth)
{
    auto isHole = [](int x, int y, int z) {
        x = (x / 64) % 3;
        y = (y / 64) % 3;
        z = (z / 64) % 3;
        return (x == 1 && y == 1) || (y == 1 && z == 1) || (x == 1 && z == 1);
        };

    for (int z = 0; z < depth; ++z) {
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                if (!isHole(x, y, z))
                    volumeData[x + y * width + z * width * height] = 255;
            }
        }
    }
}

void fillTwistedTube(std::vector<GLubyte>& volumeData, int width, int height, int depth)
{
    for (int z = 0; z < depth; ++z) {
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                float fx = (x - width / 2.0f);
                float fy = (y - height / 2.0f);
                float fz = (z - depth / 2.0f);

                float angle = fz * 0.02f;
                float tx = fx * cos(angle) - fy * sin(angle);
                float ty = fx * sin(angle) + fy * cos(angle);

                float r = sqrt(tx * tx + ty * ty);
                if (r < 60.0f)
                    volumeData[x + y * width + z * width * height] = 255;
            }
        }
    }
}

void fillKleinBottlePinched(std::vector<GLubyte>& volumeData, int width, int height, int depth) {
    int w = width, h = height, d = depth;
    auto idx = [&](int x, int y, int z) {
        return x + y * w + z * w * h;
        };

    for (float u = 0; u < 2.0f * 3.14159f; u += 0.015f) {
        for (float v = 0; v < 2.0f * 3.14159f; v += 0.015f) {
            float r = 6.0f + 3.0f * sin(2.0f * v) * sin(3.0f * u);

            float x = (r + cos(u / 2.0f) * sin(v) - sin(u / 2.0f) * sin(2.0f * v)) * cos(u);
            float y = (r + cos(u / 2.0f) * sin(v) - sin(u / 2.0f) * sin(2.0f * v)) * sin(u);
            float z = sin(u / 2.0f) * sin(v) + cos(u / 2.0f) * sin(2.0f * v);

            int xi = static_cast<int>(x * 6 + w / 2);
            int yi = static_cast<int>(y * 6 + h / 2);
            int zi = static_cast<int>(z * 6 + d / 2);

            for (int dz = -1; dz <= 1; ++dz)
                for (int dy = -1; dy <= 1; ++dy)
                    for (int dx = -1; dx <= 1; ++dx) {
                        int nx = xi + dx, ny = yi + dy, nz = zi + dz;
                        if (nx >= 0 && ny >= 0 && nz >= 0 && nx < w && ny < h && nz < d)
                            volumeData[idx(nx, ny, nz)] = 255;
                    }
        }
    }
}







void Volume::Setup()
{
#ifdef _WIN32
    program_id = LoadShaders(".\\shaders\\volume.vert", ".\\shaders\\volume.frag");
#elif defined (__linux__)
	program_id = LoadShaders("./shaders/volume.vert", "./shaders/volume.frag");
#endif


    // === 1. Generate Dummy Volume Data ===
    const int width = yLength;
    const int height = zLength;
    const int depth = xLength;

    std::vector<GLubyte> volumeData(width * height * depth, 0);
        //fillSphere(volumeData, width, height, depth);
        //fillTorus(volumeData, width, height, depth);
        //fillCylinder(volumeData, width, height, depth);
    fillKleinBottle(volumeData, width, height, depth);
        //fillKleinBottle2(volumeData, width, height, depth);
        //fillKleinBottleSwirl(volumeData, width, height, depth);
        //fillKleinBottlePinched(volumeData, width, height, depth);
    //fillKleinBottleFigure8(volumeData, width, height, depth);
   

    // === 2. Create 3D Texture ===
    glGenTextures(1, &tex3D);
    glBindTexture(GL_TEXTURE_3D, tex3D);
    glTexImage3D(GL_TEXTURE_3D, 0, GL_R8, width, height, depth, 0,
        GL_RED, GL_UNSIGNED_BYTE, volumeData.data());

    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    // === 3. Create Cube Geometry ===
    float cube[] = {
        // pos              // texcoord
        //-1,-1,-1,  0,0,0,
        // 1,-1,-1,  1,0,0,
        // 1, 1,-1,  1,1,0,
        //-1, 1,-1,  0,1,0,
        //-1,-1, 1,  0,0,1,
        // 1,-1, 1,  1,0,1,
        // 1, 1, 1,  1,1,1,
        //-1, 1, 1,  0,1,1,

        // Each face of the cube as triangles (12 triangles, 36 vertices total)
        // Back face
        -1,-1,-1,0,0,0,  1, 1,-1,1,1,0,  1,-1,-1,1,0,0,
        -1,-1,-1,0,0,0, -1, 1,-1,0,1,0,  1, 1,-1,1,1,0,
        // Front face
        -1,-1, 1,0,0,1,  1,-1, 1,1,0,1,  1, 1, 1,1,1,1,
        -1,-1, 1,0,0,1,  1, 1, 1,1,1,1, -1, 1, 1,0,1,1,
        // Left face
        -1,-1,-1,0,0,0, -1,-1, 1,0,0,1, -1, 1, 1,0,1,1,
        -1,-1,-1,0,0,0, -1, 1, 1,0,1,1, -1, 1,-1,0,1,0,
        // Right face
         1,-1,-1,1,0,0,  1, 1,-1,1,1,0,  1, 1, 1,1,1,1,
         1,-1,-1,1,0,0,  1, 1, 1,1,1,1,  1,-1, 1,1,0,1,
         // Bottom face
         -1,-1,-1,0,0,0,  1,-1,-1,1,0,0,  1,-1, 1,1,0,1,
         -1,-1,-1,0,0,0,  1,-1, 1,1,0,1, -1,-1, 1,0,0,1,
         // Top face
         -1, 1,-1,0,1,0, -1, 1, 1,0,1,1,  1, 1, 1,1,1,1,
         -1, 1,-1,0,1,0,  1, 1, 1,1,1,1,  1, 1,-1,1,1,0
    };

    glGenVertexArrays(1, &vertex_array_id);
    glBindVertexArray(vertex_array_id);

    glGenBuffers(1, &vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube), cube, GL_STATIC_DRAW);

    // pos (location = 0)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // texcoord (location = 1)
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    model_view = glGetUniformLocation(program_id, "model_view");
    projection = glGetUniformLocation(program_id, "projection");
    invModelViewProj = glGetUniformLocation(program_id, "invMVP");
    cameraPos = glGetUniformLocation(program_id, "cameraPos");

    model_matrix = glm::mat4(1.0f);
}

void Volume::UpdateModel(const glm::mat4& cam_view)
{
    SetPosition(0, 0, 0);

    auto mm = glm::translate(model_matrix, glm::vec3(_X, _Y, _Z));
    model_view_matrix = cam_view * mm;
}

void Volume::SetProjection(glm::mat4 p)
{
    projection_matrix = p;
}

void Volume::Draw()
{
    glUniformMatrix4fv(model_view, 1, GL_FALSE, glm::value_ptr(model_view_matrix));
    glUniformMatrix4fv(projection, 1, GL_FALSE, glm::value_ptr(projection_matrix));

    glm::mat4 mvp = projection_matrix * model_view_matrix;
    glm::mat4 invMVP = glm::inverse(mvp);

    glUniformMatrix4fv(invModelViewProj, 1, GL_FALSE, glm::value_ptr(invMVP));

    glm::vec3 camPosition = cam->getPosition();
    glUniform3fv(cameraPos, 1, glm::value_ptr(camPosition));

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_3D, tex3D);
    GLuint te = glGetUniformLocation(program_id, "volumeTex");
    glUniform1i(te, 0);

    glBindVertexArray(vertex_array_id);
    glDrawArrays(GL_TRIANGLES, 0, 36);  // full cube
}
