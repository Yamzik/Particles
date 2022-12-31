#version 430 core

struct Particle {
	vec2 position;
	vec2 velocity;
	vec2 force;
	vec2 rg;
	vec2 ba;
	vec2 diameter_mass;
	float z;
	float id;
	int color;
};

layout(std430, binding = 0) buffer float_buffer {
	Particle data[];
};

layout(local_size_x = 10) in;

uniform int particles_number;
uniform int stage; // 0 - force; 1 - velocity + position;
uniform float max_attraction;
uniform float attraction_coeff;
uniform float max_repultion;
uniform float repultion_coeff;
uniform float viscocity_coeff;
uniform float max_force;
uniform float friction_coeff;
uniform vec2 resolution;
uniform vec2 mouse_pos;
uniform bool mouse_click;
uniform float ratio;

float dt = 0.016;

highp float rand(vec2 co) {
	highp float a = 12.9898;
	highp float b = 78.233;
	highp float c = 43758.5453;
	highp float dt = dot(co.xy, vec2(a, b));
	highp float sn = mod(dt, 3.14);
	return fract(sin(sn) * c);
}

vec2 viscosity_law(Particle P)
{
	return P.velocity * 0.785375 * pow(P.diameter_mass.x, 2) * viscocity_coeff;
}
vec2 attraction_law(Particle A, Particle B) {
	vec2 AB = B.position - A.position;
	float radii_avg = (A.diameter_mass.x + B.diameter_mass.x) / 2.0;
	float numerator = A.diameter_mass.y * B.diameter_mass.y;
	float denominator = pow(length(AB) + radii_avg, 2.0);
	float force = attraction_coeff * numerator / denominator;
	float result = min(force, max_attraction);
	return normalize(AB) * result;
}
vec2 repultion_law(Particle A, Particle B) {
	vec2 AB = B.position - A.position;
	float radii_avg = (A.diameter_mass.x + B.diameter_mass.x) / 2.0;
	float numerator = A.diameter_mass.y * B.diameter_mass.y;
	float denominator = pow(length(AB) + radii_avg, 5.0);
	float force = -repultion_coeff * numerator / denominator;
	float result = min(force, max_repultion);
	return normalize(AB) * result;
}
vec2 mouse_repultion_law(Particle P, vec2 mousePos) {
	vec2 AB = P.position - mousePos;
	float radii_avg = P.diameter_mass.x / 2.0;
	float numerator = P.diameter_mass.y;
	float denominator = pow(length(AB) + radii_avg, 5.0);
	float force = -repultion_coeff * numerator / denominator;
	float result = min(force, max_repultion);
	return normalize(AB) * result;
}
vec2 friction_law(Particle P)
{
	float force = length(P.force);
	force = force - P.diameter_mass.y * friction_coeff;
	if (force <= 0.000001)
		return vec2(0.0f);
	else
	{
		return normalize(P.force) * force;
	}
}
vec2 clamp_force(Particle P)
{
	float force = length(P.force);
	if (force > max_force)
		return P.force * max_force / (force + 0.000001);
	return P.force;
}

void main() {
	uint ident = gl_GlobalInvocationID.x;
	/*if (ident > particles_number)
		return;*/
	data[ident].position.x *= ratio;
	vec2 mouse = mouse_pos * 2 - vec2(1);
	mouse.x = mouse.x * ratio;
	if (stage == 0)
	{
		data[ident].force = viscosity_law(data[ident]);
	}
	if (stage == 1)
	{
		for (int i = 0;
			i < particles_number;
			i++) {
			if (ident != i) {
				data[ident].force += attraction_law(data[ident], data[i]);
				data[ident].force += 100 * repultion_law(data[ident], data[i]);
				if (mouse_click)
					data[ident].force -= 10 * mouse_repultion_law(data[ident], mouse);
			}
		}
	}
	if (stage == 2)
	{
		data[ident].force = friction_law(data[ident]);
		data[ident].force = clamp_force(data[ident]);
	}
	if (stage == 3)
	{
		//float lim = 1 - 10 * data[ident].diameter_mass.x / resolution.y;
		//float lim = 1 - 0.1;
		
		data[ident].velocity += dt * data[ident].force / data[ident].diameter_mass.y;

		vec2 position = data[ident].position + data[ident].velocity * dt;
		if (position.x > ratio || position.x < -ratio)
		{
			data[ident].force.x = -data[ident].force.x;
			data[ident].velocity.x = -data[ident].velocity.x;
		}
		//if (position.y > lim || position.y < -lim)
		if (position.y > 1.0 || position.y < -1.0)
		{
			data[ident].force.y = -data[ident].force.y;
			data[ident].velocity.y = -data[ident].velocity.y;
		}
		data[ident].position += data[ident].velocity * dt;
	}
	data[ident].position.x /= ratio;

	if (isnan(data[ident].position.x)) data[ident].position.x = 0;
	if (isnan(data[ident].force.x)) data[ident].force.x = 0;
	if (isnan(data[ident].velocity.x)) data[ident].velocity.x = 0;
	if (isnan(data[ident].position.y)) data[ident].position.y = 0;
	if (isnan(data[ident].force.y)) data[ident].force.y = 0;
	if (isnan(data[ident].velocity.y)) data[ident].velocity.y = 0;
}