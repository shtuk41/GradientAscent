/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include <GL/glew.h>

#include <string>
#include <fstream>
#include <iostream>
#include <vector>

std::string readSourceFile(const char *path)
{
	std::string code;

	std::ifstream file_stream(path,std::ios::in);

	if (file_stream.is_open())
	{
		std::string line =	"";
		while (getline(file_stream, line))
			code += "\n" + line;

		file_stream.close();

		return code;

	}
	else
	{
		std::cout << "Failed to open " << path << std::endl;
		return "";
	}
}

void CompileShader(std::string program_code, GLuint shader_id)
{
	GLint result = GL_FALSE;
	int infolog_length;
	char const *program_code_pointer = program_code.c_str();
	glShaderSource(shader_id, 1, &program_code_pointer, NULL);
	glCompileShader(shader_id);
	glGetShaderiv(shader_id, GL_COMPILE_STATUS, &result);
	glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &infolog_length);

	if (infolog_length > 0)
	{
		std::vector<char> error_msg(infolog_length + 1);
		glGetShaderInfoLog(shader_id, infolog_length, NULL, &error_msg[0]);
		std::cout << &error_msg[0] << std::endl;
	}
}

GLuint LoadShaders(const char * vertex_file_path, const char *fragment_file_path)
{
	GLuint vertex_shader_id = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragment_shader_id = glCreateShader(GL_FRAGMENT_SHADER);
	std::string vertex_shader_code = readSourceFile(vertex_file_path);
	if (vertex_shader_code == "")
	{
		return 0;
	}
	std::string fragment_shader_code = readSourceFile(fragment_file_path);
	if (fragment_shader_code == "")
	{
		return 0;
	}

	std::cout << "Compiling Vertex shader: " << vertex_file_path << std::endl;
	CompileShader(vertex_shader_code, vertex_shader_id);
	std::cout << "Compiling Fragment sahder : " << fragment_file_path << std::endl;
	CompileShader(fragment_shader_code,fragment_shader_id);

	GLint result = GL_FALSE;
	int infolog_length;
	std::cout << "Linking program" << std::endl;

	GLuint program_id = glCreateProgram();
	glAttachShader(program_id, vertex_shader_id);
	glAttachShader(program_id, fragment_shader_id);
	glLinkProgram(program_id);

	glGetProgramiv(program_id, GL_LINK_STATUS, &result);
	glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &infolog_length);
	if (infolog_length > 0)
	{
		std::vector<char> program_error_msg(infolog_length + 1);
		glGetProgramInfoLog(program_id, infolog_length, NULL, &program_error_msg[0]);
		std::cout << &program_error_msg[0] << std::endl;
	}
	else
	{
		std::cout << "Linked Successfully" << std::endl;
	}

	glDeleteShader(vertex_shader_id);
	glDeleteShader(fragment_shader_id);
	return program_id;

}


