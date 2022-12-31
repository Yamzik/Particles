#pragma once
#include <Particle.h>
#include <Force.h>
#include <ComputeShader.h>

namespace res
{
	class GpuParticleResolver
	{
	public:
		GpuParticleResolver(ParticleGroup* begin, int count, ComputeShader* shader,
			glm::vec2* resolution, glm::vec2* mouse_pos, bool* mouse_click)
		{
			_shader = shader;
			_size = count;
			_groups = begin;
			_particles_cnt = 0;
			_resolution = resolution;
			_mouse_pos = mouse_pos;
			_mouse_click = mouse_click;

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

			_particles_transfer = std::vector<Particle>(_particles_cnt);
			//_check = std::vector<float>(8 * _particles_cnt);
			glGenBuffers(1, &_particles_buffer);
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, _particles_buffer);
			/*glGenBuffers(1, &_check_buffer);
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, _check_buffer);*/

		}
		void resolve()
		{
			const int WORKGROUP_SIZE = 10;
			const int NUM_INVOCATIONS = _particles_cnt / WORKGROUP_SIZE +
				(_particles_cnt % WORKGROUP_SIZE == 0 ? 0 : 1);

			//Particle* _particles_transfer = (Particle*)malloc(sizeof(Particle) * _particles_cnt);
			for (int i = 0; i < _particles_cnt; i++)
			{
				_particles_transfer[i] = *_particles[i];
				//_check[i] = 0;
			}

			glBindBuffer(GL_SHADER_STORAGE_BUFFER, _particles_buffer);
			glBufferData(GL_SHADER_STORAGE_BUFFER,
				_particles_cnt * sizeof(Particle),
				&_particles_transfer[0],
				GL_DYNAMIC_COPY);
			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, _particles_buffer);

			/*glBindBuffer(GL_SHADER_STORAGE_BUFFER, _check_buffer);
			glBufferData(GL_SHADER_STORAGE_BUFFER,
				8 * _particles_cnt * sizeof(float),
				&_check[0],
				GL_DYNAMIC_COPY);
			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, _check_buffer);*/


			int stage = 0;
			_shader->use();
			_shader->setInt("particles_number", _particles_cnt);
			_shader->setInt("stage", stage);
			_shader->setFloat("max_attraction", force::c::max_attraction);
			_shader->setFloat("attraction_coeff", /*force::c::attraction_coeff*/1.0f);
			_shader->setFloat("max_repultion", force::c::max_repultion);
			_shader->setFloat("repultion_coeff", /*force::c::repultion_coeff*/1.0f);
			_shader->setFloat("viscocity_coeff", force::c::viscocity_coeff);
			_shader->setFloat("max_force", force::c::max_force);
			_shader->setFloat("friction_coeff", force::c::friction_coeff);
			_shader->setVec2f("resolution", _resolution->x, _resolution->y);
			_shader->setVec2f("mouse_pos", *_mouse_pos / *_resolution);
			_shader->setBool("mouse_click", *_mouse_click);
			_shader->setFloat("ratio", _resolution->x / _resolution->y);

			glDispatchCompute(NUM_INVOCATIONS, 1, 1);
			glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

			stage++;
			_shader->setInt("stage", stage);
			glDispatchCompute(NUM_INVOCATIONS, 1, 1);
			glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

			stage++;
			_shader->setInt("stage", stage);
			glDispatchCompute(NUM_INVOCATIONS, 1, 1);
			glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

			stage++;
			_shader->setInt("stage", stage);
			glDispatchCompute(NUM_INVOCATIONS, 1, 1);
			glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

			Particle* buffer_data = (Particle*)glMapNamedBuffer(_particles_buffer, GL_READ_WRITE);
			//int* check_data = (int*)glMapNamedBuffer(_check_buffer, GL_READ_WRITE);
			for (int i = 0; i < _particles_cnt; i++) {
				_particles[i]->position = buffer_data[i].position;
				_particles[i]->velocity = buffer_data[i].velocity;
				_particles[i]->force = buffer_data[i].force;
				//out_data[i] = buffer_data[i];
			}
			/*for (int i = 0; i < 8 * _particles_cnt; i++) {
				_check[i] = check_data[i];
			}*/

			//free(_particles_transfer);
		}

	private:
		unsigned int _particles_buffer;
		//unsigned int _check_buffer;
		int muliplier = 4;
		int _size;
		int _particles_cnt;
		ComputeShader* _shader;
		ParticleGroup* _groups;
		Particle** _particles;
		std::vector<Particle> _particles_transfer;
		//std::vector<float> _check;
		glm::vec2* _resolution;
		glm::vec2* _mouse_pos;
		bool* _mouse_click;
	};
}