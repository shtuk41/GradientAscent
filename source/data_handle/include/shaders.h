#ifndef SHADERS_H
#define SHADERS_H

#include <GL/glew.h>
#include <string>

std::string readSourceFile(const char *path);
void CompileShader(std::string program_code, GLuint shader_id);
GLuint LoadShaders(const char * vertex_file_path, const char *fragment_file_path);



#endif /* SHADERS_H */

