#pragma once
#include <Particle.h>
#include <Force.h>

namespace res
{
	class ParticleResolver
	{
	public:
		ParticleResolver(ParticleGroup* begin, int count)
		{
			_size = count;
			_groups = begin;
			_particles_cnt = 0;

			for (int i = 0; i < count; i++)
				_particles_cnt += (_groups + i)->Particles.size();

			_particles = new Particle * [_particles_cnt];

			int offset = 0;
			for (int i = 0; i < count; i++)
			{
				int group_particles_count = (_groups + i)->Particles.size();
				for (int j = 0; j < group_particles_count; j++)
				{
					_particles[offset] = &(_groups + i)->Particles[j];
					offset++;
				}
			}
		}

		void resolve()
		{
			resolve_force_by_particle();
			resolve_position();
		}
	private:
		int _size;
		int _particles_cnt;
		ParticleGroup* _groups;
		Particle** _particles;
		void resolve_force()
		{
			for (int i = 0; i < _size; i++)
			{
				ParticleGroup* groupA = &_groups[i];
				for (int j = 0; j < groupA->Particles.size(); j++)
				{
					Particle* particleA = &groupA->Particles[j];
					particleA->force = glm::vec2(0.0f);
					particleA->force += force::viscosity_law(particleA);
					for (int k = 0; k < _size; k++)
					{
						ParticleGroup* groupB = &_groups[k];
						for (int w = 0; w < groupB->Particles.size(); w++)
						{
							Particle* particleB = &groupB->Particles[w];
							if (particleA == particleB) continue;
							glm::vec2 force;
							force = force::attraction_law(particleA, particleB);
							force += force::repultion_law(particleA, particleB);
							particleA->force += force;
							particleB->force -= force;
						}
					}
				}
			}
		}
		void resolve_force_by_particle()
		{
			for (int i = 0; i < _particles_cnt; i++)
				_particles[i]->force = glm::vec2(0.0f);

			for (int i = 0; i < _particles_cnt; i++)
			{
				Particle* particleA = _particles[i];
				particleA->force += force::viscosity_law(particleA);
				for (int j = i + 1; j < _particles_cnt; j++)
				{
					glm::vec2 prev_froce = particleA->force;
					Particle* particleB = _particles[j];
					glm::vec2 force;
					force = force::attraction_law(particleA, particleB);
					force += force::repultion_law(particleA, particleB);
					particleA->force += force;
					particleB->force -= force;
				}
				force::friction_law(particleA);
				force::clamp_force(particleA);
			}
		}

		void resolve_position()
		{
			for (int i = 0; i < _size; i++)
			{
				ParticleGroup* group = &_groups[i];
				for (int j = 0; j < group->Particles.size(); j++)
				{
					Particle* particle = &group->Particles[j];
					glm::vec2 pos = particle->position;
					force::evolve_velocity(particle);
					force::evolve_position(particle);
				}
			}
		}
	};
}