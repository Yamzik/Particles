# A particles interaction model

![demo](https://github.com/Yamzik/Particles/blob/main/img/demo.gif)

Following forces are implemented:
- attraction (as ~r<sup>-2</sup>)
- repultion (as ~r<sup>-5</sup>)
- friction

The particles are also subject to viscous friction

## Libraries used
The particles are rendered with usage of OpenGL with [glfw](https://github.com/glfw/glfw) context and [glad](https://github.com/Dav1dde/glad) extractor with math types provided by [glm](https://github.com/g-truc/glm). Main file also #includes Windows.h wich is needed for selecting discrete GPU, so in order to do so with other OS, you are to find the other way.

## Files description
| File | Description |
| - | - |
| [Force.h](https://github.com/Yamzik/Particles/blob/main/Include/Force.h) | Contains namespaces with constants related to forces, also contains some physics laws functions (which are not used when calculating with GPU) |
| [Particle.h](https://github.com/Yamzik/Particles/blob/main/Include/Particle.h) | Contains definition of *struct Particle* and *ParticleGroup* class, which is container for particles of the same color and their renderer |
| [Shader.h](https://github.com/Yamzik/Particles/blob/main/Include/Shader.h) | Definition of *Shader* class dedicated to abstarct away OpenGL *render* program |
| [ComputeShader.h](https://github.com/Yamzik/Particles/blob/main/Include/ComputeShader.h) | Definition of *ComputeShader* class dedicated to abstarct away OpenGL *compute* program |
| [GpuParticleResolver](https://github.com/Yamzik/Particles/blob/main/Include/GpuParticleResolver.h) | Definition of *GpuParticleResolver* class which is used as GPU data bridge |