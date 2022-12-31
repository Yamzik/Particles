#pragma once
#include <glad/glad.h> // include glad to get all the required OpenGL headers
#include <glm/vec2.hpp>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class ComputeShader
{
public:
	unsigned int ID;

	ComputeShader(const char* computePath)
	{
		std::string computeCode;
		std::ifstream cShaderFile;
		// ensure ifstream objects can throw exceptions:
		cShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		try
		{
			cShaderFile.open(computePath);
			std::stringstream cShaderStream;
			cShaderStream << cShaderFile.rdbuf();

			cShaderFile.close();

			computeCode = cShaderStream.str();
		}
		catch (const std::exception&)
		{
			std::cout << "ERROR::SHADER::FILE_NOT_SUCCEFULLY_READ" << std::endl;
		}
		const char* cShaderCode = computeCode.c_str();

		unsigned int compute;
		compile(&compute, cShaderCode, GL_COMPUTE_SHADER);

		ID = glCreateProgram();
		glAttachShader(ID, compute);
		glLinkProgram(ID);

		int success;
		char infoLog[512];
		glGetProgramiv(ID, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(ID, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
		}

		// delete the shaders as they're linked into our program now and no longer necessary
		glDeleteShader(compute);
	}
	ComputeShader(const char* source, int c)
	{
		const char* cShaderCode = source;

		unsigned int compute;
		compile(&compute, cShaderCode, GL_COMPUTE_SHADER);

		ID = glCreateProgram();
		glAttachShader(ID, compute);
		glLinkProgram(ID);

		int success;
		char infoLog[512];
		glGetProgramiv(ID, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(ID, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
		}

		// delete the shaders as they're linked into our program now and no longer necessary
		glDeleteShader(compute);
	}

	// use/activate the shader
	void use()
	{
		glUseProgram(ID);
	}
	// utility uniform functions
	void setBool(const std::string& name, bool value) const
	{
		glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
	}
	void setInt(const std::string& name, int value) const
	{
		glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
	}
	void setUint(const std::string& name, u_int value) const
	{
		glUniform1ui(glGetUniformLocation(ID, name.c_str()), value);
	}
	void setFloat(const std::string& name, float value) const
	{
		glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
	}
	void setVec2f(const std::string& name, float value1, float value2)
	{
		glUniform2f(glGetUniformLocation(ID, name.c_str()), value1, value2);
	}
	void setVec2f(const std::string& name, glm::vec2 value)
	{
		glUniform2f(glGetUniformLocation(ID, name.c_str()), value.x, value.y);
	}
	void setVec4f(const std::string& name, float value1, float value2, float value3, float value4)
	{
		glUniform4f(glGetUniformLocation(ID, name.c_str()), value1, value2, value3, value4);
	}

private:
	void compile(unsigned int* shader, const char* sourceCode, GLenum type)
	{
		int success;
		char infoLog[512];

		*shader = glCreateShader(type);
		glShaderSource(*shader, 1, &sourceCode, NULL);
		glCompileShader(*shader);
		glGetShaderiv(*shader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(*shader, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::COMPUTE::COMPILATION_FAILED\n" << infoLog << std::endl;
		}
	}
};