#pragma once
#include <Particle.h>
#include <glm/glm.hpp>

namespace force
{
	namespace c
	{
		float max_attraction = 100.0f;
		float attraction_coeff = 1 / 10.0f;
		float max_repultion = 100.0f;
		float repultion_coeff = 1 / 10.0f;
		float viscocity_coeff = -10.0f;
		float max_force = 0.5f;
		float friction_coeff = 0.01f;
	}

	namespace r
	{
		float get_attraction_coeff(Color A, Color B)
		{
			if (A != B)
			{
				int combination = (int)A + (int)B;
				switch (combination)
				{
				case 0b11:	 return	1.0f; // red-green
				case 0b101:	 return	1.0f; // red-blue
				case 0b110:	 return	1.0f; // green-blue
				case 0b1001: return	1.0f; // red-white
				case 0b1010: return	1.0f; // green-white
				case 0b1100: return	1.0f; // blue-white
				default: return 0;
				}
			}
			else
			{
				switch (A)
				{
				case 0b1:	 return -1.0f; // red-red
				case 0b10:	 return -1.0f; // green-green
				case 0b100:	 return -1.0f; // blue-blue
				case 0b1000: return -1.0f; // white-white
				default: return 0;
				}
			};
		}
		float get_repultion_coeff(Color A, Color B)
		{
			if (A != B)
			{
				int combination = (int)A + (int)B;
				switch (combination)
				{
				case 0b11:	 return	-0.20f;
				case 0b101:	 return	 0.20f;
				case 0b110:	 return	 0.20f;
				case 0b1001: return	-0.20f;
				case 0b1010: return	 0.20f;
				case 0b1100: return	-0.20f;
				default: return 0;
				}
			}
			else
			{
				switch (A)
				{
				case 0b1:	 return 0.5f;
				case 0b10:	 return 0.5f;
				case 0b100:	 return 0.5f;
				case 0b1000: return 0.5f;
				default: return 0;
				}
			};
		}
	}

	float dt = 0.0166f;

	void setup_force(float max_attraction,
		float attraction_coeff,
		float max_repultion,
		float repultion_coeff,
		float viscocity_coeff,
		float max_force)
	{
		c::max_attraction = max_attraction;
		c::attraction_coeff = attraction_coeff;
		c::max_repultion = max_repultion;
		c::repultion_coeff = repultion_coeff;
		c::viscocity_coeff = viscocity_coeff;
		c::max_force = max_force;
	}

	void setup_relations()
	{

	}

	void set_dt(float delta_t)
	{
		dt = delta_t;
	}

	glm::vec2 viscosity_law(Particle* P)
	{
		return P->velocity * pt::get_surface_area((Color)P->color) * c::viscocity_coeff;
	}
	glm::vec2 attraction_law(Particle* A, Particle* B)
	{
		glm::vec2 AB = B->position - A->position;
		float radii_avg = (pt::get_radius((Color)A->color) + pt::get_radius((Color)B->color)) / 2.0f;
		float numerator = pt::get_mass((Color)A->color) * pt::get_mass((Color)B->color);
		float denominator = glm::pow(glm::length(AB) + radii_avg, 2);
		float force = r::get_attraction_coeff((Color)A->color, (Color)B->color) * c::attraction_coeff * numerator / denominator;
		float result = std::min(force, c::max_attraction);
		return glm::normalize(AB) * result;
	}
	glm::vec2 repultion_law(Particle* A, Particle* B)
	{
		glm::vec2 AB = B->position - A->position;
		float radii_avg = (pt::get_radius((Color)A->color) + pt::get_radius((Color)B->color)) / 2.0f;
		float numerator = pt::get_mass((Color)A->color) * pt::get_mass((Color)B->color);
		float denominator = glm::pow(glm::length(AB) + radii_avg, 5);
		float force = r::get_repultion_coeff((Color)A->color, (Color)B->color) * c::repultion_coeff * numerator / denominator;
		float result = std::min(force, c::max_repultion);
		return glm::normalize(AB) * result;
	}
	void friction_law(Particle* P)
	{
		float force = glm::length(P->force);
		force = force - pt::get_mass((Color)P->color) * c::friction_coeff;
		if (force <= 0)
		{
			P->force = glm::vec2(0.0f);
		}
		else
		{
			P->force = glm::normalize(P->force) * force;
		}
	}
	void clamp_force(Particle* P)
	{
		float force = glm::length(P->force);
		if (force > c::max_force)
		{
			P->force *= c::max_force / force;
		}
	}

	void evolve_velocity(Particle* P)
	{
		P->velocity = P->velocity + P->force / pt::get_mass((Color)P->color) * dt;
	}
	void evolve_position(Particle* P)
	{
		glm::vec2 position = P->position + P->velocity * dt;
		if (position.x > 1 || position.x < -1)
		{
			P->force.x = -P->force.x;
			P->velocity.x = -P->velocity.x;
		}
		if (position.y > 1 || position.y < -1)
		{
			P->force.y = -P->force.y;
			P->velocity.y = -P->velocity.y;
		}
		P->position += P->velocity * dt;
	}
}