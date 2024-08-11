# Real-Time Fluid Dynamics Simulation

This project is an implementation of Jos Stam's "Real-Time Fluid Dynamics for Games" algorithm using C++ for the core logic and OpenGL for rendering. The simulation is interactive, allowing users to influence the fluid flow through mouse clicks.

## Features

- **Real-Time Fluid Simulation**: The project implements Jos Stam's fluid dynamics algorithm, which allows for the simulation of fluid behavior in real-time.
- **Interactive Controls**: Users can interact with the fluid by clicking the mouse, affecting the flow and movement of the fluid in the simulation.
- **OpenGL Rendering**: The fluid dynamics are rendered using OpenGL, providing a visually appealing and responsive display of the simulation.
- **High Performance**: The implementation is optimized for performance, ensuring smooth operation even in real-time scenarios.

## Controls

- **Left Mouse Click + Drag**: Add forces to the fluid, influencing its flow.
- **Right Mouse Click**: Add fluid to the simulation.

## Demo

Here is a quick demonstration of the simulation in action:

![Fluid Simulation Demo](/recording.gif)

## Technical Details

This implementation follows the method described by Jos Stam in his paper "Real-Time Fluid Dynamics for Games." The algorithm uses a grid-based approach to simulate fluid behavior and is designed to be efficient enough for real-time applications.

### Core Concepts:

- **Density Field**: The simulation uses a density field to represent the fluid's density in each cell.
- **Velocity Field**: The simulation uses a velocity field to represent the fluid's flow. Each cell in the grid contains a velocity vector.
- **Advection**: The advection step moves fluid quantities (like velocity and density) through the velocity field.
- **Diffusion**: The diffusion step models the spreading of fluid quantities over time.
- **Projection**: The projection step ensures that the velocity field remains divergence-free, preserving the incompressibility of the fluid.
- **External Forces**: Users can introduce external forces into the simulation by interacting with the mouse.

## References

[Jos Stam, "Real-Time Fluid Dynamics for Games," GDC 2003](http://graphics.cs.cmu.edu/nsp/course/15-464/Fall09/papers/StamFluidforGames.pdf)

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.
