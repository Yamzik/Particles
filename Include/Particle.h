#pragma once
#include "Color.h"
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

typedef struct Particle
{
	glm::vec2 position;
	glm::vec2 velocity;
	glm::vec2 force;
	glm::vec2 rg;
	glm::vec2 ba;
	glm::vec2 diameter_mass;
	float z;
	float y;
	float id;
	Color color;
} Particle;

namespace pt
{
	namespace diam
	{
		float red = 1.0f;
		float green = 1.0f;
		float blue = 1.0f;
		float white = 1.0f;
	}
	namespace radii
	{
		float red = 0.5f;
		float green = 0.5f;
		float blue = 0.5f;
		float white = 0.5f;
	}
	namespace surface_area
	{
		float red = 0.785375f;
		float green = 0.785375f;
		float blue = 0.785375f;
		float white = 0.785375f;
	}
	namespace mass
	{
		float red = 1.0;
		float green = 1.0;
		float blue = 1.0;
		float white = 1.0;
	}

	void setup_diameters(float red, float green, float blue, float white)
	{
		diam::red = red;
		radii::red = red / 2;
		surface_area::red = glm::quarter_pi<float>() * pow(red, 2);

		diam::green = green;
		radii::green = green / 2;
		surface_area::green = glm::quarter_pi<float>() * pow(green, 2);

		diam::blue = blue;
		radii::blue = blue / 2;
		surface_area::blue = glm::quarter_pi<float>() * pow(blue, 2);

		diam::white = white;
		radii::white = white / 2;
		surface_area::white = glm::quarter_pi<float>() * pow(white, 2);
	}
	void setup_masses(float red, float green, float blue, float white)
	{
		mass::red = red;
		mass::green = green;
		mass::blue = blue;
		mass::white = white;
	}

	static float id_cnt = 0;

	float get_diameter(Color color)
	{
		switch (color)
		{
		case Red:	return diam::red;
		case Green:	return diam::green;
		case Blue:	return diam::blue;
		case White:	return diam::white;
		default:break;
		}
	}
	float get_radius(Color color)
	{
		switch (color)
		{
		case Red:	return radii::red;
		case Green:	return radii::green;
		case Blue:	return radii::blue;
		case White:	return radii::white;
		default:break;
		}
	}
	float get_surface_area(Color color)
	{
		float diam = 0;
		switch (color)
		{
		case Red:	diam = diam::red;
		case Green:	diam = diam::green;
		case Blue:	diam = diam::blue;
		case White:	diam = diam::white;
		default:break;
		}
		return glm::quarter_pi<float>() * pow(diam, 2);
	}
	float get_mass(Color color)
	{
		switch (color)
		{
		case Red:	return mass::red;
		case Green:	return mass::green;
		case Blue:	return mass::blue;
		case White:	return mass::white;
		default:break;
		}
	}
	void get_color_rgb(Color color, glm::vec2* rg, glm::vec2* ba)
	{
		switch (color)
		{
		case Red:	*rg = glm::vec2(1.0f, 0.0f); *ba = glm::vec2(0.0f, 1.0f); break;
		case Green:	*rg = glm::vec2(0.0f, 1.0f); *ba = glm::vec2(0.0f, 1.0f); break;
		case Blue:	*rg = glm::vec2(0.0f, 0.0f); *ba = glm::vec2(1.0f, 1.0f); break;
		case White:	*rg = glm::vec2(1.0f, 1.0f); *ba = glm::vec2(1.0f, 1.0f); break;
		default:break;
		}
	}
}
float z_buffer = 0;
Particle CreateParticle(Color color)
{
	Particle p;
	p.id = ++pt::id_cnt;
	pt::get_color_rgb(color, &p.rg, &p.ba);
	p.diameter_mass.x = pt::get_diameter(p.color);
	p.diameter_mass.y = pt::get_mass(p.color);
	p.color = color;
	p.position = glm::vec2(
		((float)rand() / RAND_MAX) * 2 - 1,
		((float)rand() / RAND_MAX) * 2 - 1);
	p.z = z_buffer++;// ((float)rand() / RAND_MAX) * 2 - 1;
	return p;
}

class ParticleGroup
{
public:
	ParticleGroup(int count, Color color, Shader* shader, glm::vec2* resolution)
	{
		_shader = shader;
		_resolution = resolution;
		_ratio = _resolution->x / _resolution->y;
		Count = count;
		Particles = std::vector<Particle>(count);
		Color = color;

		for (int i = 0; i < count; i++)
		{
			Particle p = CreateParticle(color);
			Particles[i] = p;
		}

		glGenVertexArrays(1, &_VAO);
		glGenBuffers(1, &_VBO);
		glGenBuffers(1, &_particleBuffer);

		set_buffers();
	}

	void shuffle()
	{
		for (int i = 0; i < Particles.size(); i++)
		{
			Particles[i].position = glm::vec2(
				((float)rand() / RAND_MAX) * 2 - 1,
				((float)rand() / RAND_MAX) * 2 - 1);
		}
	}
	void render(bool reset_buffers)
	{
		std::vector<Particle> p = Particles;
		if (reset_buffers)
		{
			set_buffers();
		}

		_shader->use();
		_shader->setFloat("diameter", pt::get_diameter(Color));
		_shader->setFloat("ratio", _resolution->x / _resolution->y);
		_shader->setVec2f("resolution", _resolution->x, _resolution->y);
		glBindVertexArray(_VAO);
		glDrawArrays(GL_POINTS, 0, Particles.size());
	}

	int Count;
	Color Color;
	std::vector<Particle> Particles;

private:
	unsigned int _VAO, _VBO;
	unsigned int _particleBuffer;
	Shader* _shader;
	glm::vec2* _resolution;
	float _ratio;

	void set_buffers()
	{
		glBindVertexArray(_VAO);

		glBindBuffer(GL_ARRAY_BUFFER, _VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Particle) * Particles.size(), &Particles[0], GL_STATIC_DRAW);

		// position attribute
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Particle), (void*)0);
		glEnableVertexAttribArray(0);

		// velocity attribute
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Particle), (void*)(sizeof(glm::vec2)));
		glEnableVertexAttribArray(1);

		// force attribute
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Particle), (void*)(2 * sizeof(glm::vec2)));
		glEnableVertexAttribArray(2);

		// color attribute RG
		glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(Particle), (void*)(3 * sizeof(glm::vec2)));
		glEnableVertexAttribArray(3);

		// color attribute BA
		glVertexAttribPointer(4, 2, GL_FLOAT, GL_FALSE, sizeof(Particle), (void*)(4 * sizeof(glm::vec2)));
		glEnableVertexAttribArray(4);

		// depth buffer
		glVertexAttribPointer(5, 1, GL_FLOAT, GL_FALSE, sizeof(float), (void*)(7 * sizeof(glm::vec2)));
		glEnableVertexAttribArray(5);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}
};