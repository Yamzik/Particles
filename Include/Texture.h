#ifndef TEXTURE_H
#define TEXTURE_H

#include "../../Include/stb_image.h"
#include <glad/glad.h>
#include <iostream>

class Texture
{
public:
	unsigned int ID;

	int width;
	int height;
	int nrChannels;

	static Texture Create(const char* imagePath, GLenum format)
	{
		return Texture(imagePath, format);
	}
	static Texture Create(const char* imagePath, GLenum format,
		GLenum wrap_s, GLenum wrap_t)
	{
		return Texture(imagePath, format, wrap_s, wrap_t);
	}
	static Texture Create(const char* imagePath, GLenum format,
		GLenum wrap_s, GLenum wrap_t,
		GLenum min_filter, GLenum mag_filter)
	{
		return Texture(imagePath, format, wrap_s, wrap_t, min_filter, mag_filter);
	}

	void use(GLenum texUnit)
	{
		glActiveTexture(texUnit);
		glBindTexture(GL_TEXTURE_2D, ID);
	}

private:
	Texture(const char* imagePath, GLenum format,
		GLenum _wrap_s = GL_REPEAT, GLenum _wrap_t = GL_REPEAT,
		GLenum _min_filter = GL_LINEAR_MIPMAP_LINEAR, GLenum _mag_filter = GL_LINEAR)
	{
		unsigned char* data = stbi_load(imagePath, &width, &height, &nrChannels, 0);

		glGenTextures(1, &ID);
		glBindTexture(GL_TEXTURE_2D, ID);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, _wrap_s);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, _wrap_t);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, _min_filter);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, _mag_filter);

		if (data)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		else
		{
			std::cout << "Failed to load texture" << std::endl;
		}
		stbi_image_free(data);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
};

#endif