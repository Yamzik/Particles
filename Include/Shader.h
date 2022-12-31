#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h> // include glad to get all the required OpenGL headers

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>


class Shader
{
public:
	// the program ID
	unsigned int ID;

	// constructor reads and builds the shader
	Shader(const char* vertexPath, const char* fragmentPath)
	{
		std::string vertexCode;
		std::string fragmentCode;
		std::ifstream vShaderFile;
		std::ifstream fShaderFile;
		// ensure ifstream objects can throw exceptions:
		vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		try
		{
			vShaderFile.open(vertexPath);
			fShaderFile.open(fragmentPath);
			std::stringstream vShaderStream, fShaderStream;
			vShaderStream << vShaderFile.rdbuf();
			fShaderStream << fShaderFile.rdbuf();

			vShaderFile.close();
			fShaderFile.close();

			vertexCode = vShaderStream.str();
			fragmentCode = fShaderStream.str();
		}
		catch (const std::exception&)
		{
			std::cout << "ERROR::SHADER::FILE_NOT_SUCCEFULLY_READ" << std::endl;
		}
		const char* vShaderCode = vertexCode.c_str();
		const char* fShaderCode = fragmentCode.c_str();

		unsigned int vertex, fragment;
		compile(&vertex, vShaderCode, GL_VERTEX_SHADER);
		compile(&fragment, fShaderCode, GL_FRAGMENT_SHADER);

		ID = glCreateProgram();
		glAttachShader(ID, vertex);
		glAttachShader(ID, fragment);
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
		glDeleteShader(vertex);
		glDeleteShader(fragment);
	}
	Shader(const char* vertexSrc, const char* fragmentSrc, int c)
	{
		const char* vShaderCode = vertexSrc;
		const char* fShaderCode = fragmentSrc;

		unsigned int vertex, fragment;
		compile(&vertex, vShaderCode, GL_VERTEX_SHADER);
		compile(&fragment, fShaderCode, GL_FRAGMENT_SHADER);

		ID = glCreateProgram();
		glAttachShader(ID, vertex);
		glAttachShader(ID, fragment);
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
		glDeleteShader(vertex);
		glDeleteShader(fragment);
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
	void setFloat(const std::string& name, float value) const
	{
		glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
	}
	void setVec2f(const std::string& name, float value1, float value2)
	{
		glUniform2f(glGetUniformLocation(ID, name.c_str()), value1, value2);
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
			std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
		}
	}
};

#endif