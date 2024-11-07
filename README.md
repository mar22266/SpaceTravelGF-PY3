# SpaceTravel Renderer by Andre Marroquin

This repository contains a software renderer that creates and displays a solar system with various celestial bodies, including stars, a sun, invented planets, and a spaceship, all using shaders. The project demonstrates creativity and shader complexity, rendering each celestial body with unique visual effects, solely by manipulating colors without relying on textures or materials.

## Demo Video

### Video Description
Take a look at this clip! The spaceship can't fly through the planets, so it stops when it gets too close, but you'll see all the planets orbiting the sun and stars twinkling in the background. It’s a neat little demo of the spaceship cruising around the solar system.

![Demo SpaceTravel](./video.mp4)


The aim of this project is to develop a 3D simulation of a solar system featuring a controllable spaceship navigating through an environment with seven distinct celestial bodies. Each celestial body, from rocky planets to gas giants and stars. Shaders:

- **Rocky Planet**
- **Gas Giant**
- **Sun**
- **Earth**
- **Mars**
- **Neptune**
- **Star**

## Features

### Shaders
Seven distinct shaders are used to render celestial bodies, each creating unique visual effects that enhance the representation of each type of celestial body in the solar system.

### Celestial Bodies
- **Sun**: A luminous, emissive core that provides dynamic lighting effects to the entire solar system.
- **Rocky Planets**: Includes planets like Earth and Mars, each rendered with detailed surface textures that reflect their unique geological features.
- **Gas Giant**: Neptune is visualized with swirling atmospheric shaders that mimic the turbulent, gaseous environment.
- **Star**: Additional stars in the background contribute twinkling and other radiant effects to the space scene.

### User Controls
- **Mouse Scroll**: Zoom in and zoom out within the solar system to view celestial bodies from different distances.
- **WASD Keys**: Navigate the spaceship forward, backward, and side to side, enabling comprehensive exploration of the solar system.
- **Arrow Keys**: Control the camera’s angle and perspective, allowing for up, down, left, and right movements to better view the celestial bodies and spaceship interactions.

### Additional Features
- **Skybox**: A star-filled skybox surrounds the solar system, adding depth and enhancing the realism of the space environment.
- **Spaceship Navigation**: A fully controllable spaceship navigates through the solar system, visiting various planets and providing interactive exploration capabilities.

## Points and Scoring

### Aesthetic Appeal
- **30 Points**: For the complete aesthetic of the solar system. The scoring is based on the subjective appeal and artistic integration of the celestial bodies and their interactions within the scene.

### Performance
- **20 Points**: For the overall performance of the scene. Maintaining a frame rate of 60 FPS is not necessary; it is sufficient that the performance feels fluid and appropriate for the complexity of the simulation.

### Spaceship Rendering
- **20 Points**: For rendering a custom-modeled spaceship that follows the camera. Additional points are awarded based on how convincing the illusion is that the user is controlling the spaceship.

### Dynamic Skybox
- **10 Points**: For creating a skybox that displays stars on the horizon, enhancing the depth and realism of the space environment.

### Collision Avoidance
- **10 Points**: For implementing conditions that prevent the spaceship/camera from passing through elements of the solar system, ensuring a realistic navigation experience.

### 3D Camera Movement
- **20 Points**: For implementing comprehensive 3D movement for the camera, allowing for immersive exploration and interaction within the solar system.

### Mouse Controls
- **10 Points**: For enabling camera control using the mouse, enhancing user interaction and navigation flexibility.

### Zoom Functionality
- **10 Points**: For using the mouse scroll to adjust the camera's distance from the spaceship, allowing for zooming in and out of the solar system for a detailed or broader view.

These points reflect the project's focus on interactive elements, user control, and visual appeal, contributing to a dynamic and engaging user experience in exploring the solar system.


## Project Structure

- `src/main.cpp`: Main program file that initializes SDL, handles user input, and controls rendering.
- `headers/`: Directory containing all header files for classes such as `camera`, `fragment`, `framebuffer`, `model`, and `shaders`.
- `models`: Includes de models of the .obj files for rendering the spaceship and spheres.

## Installation and Setup

### Prerequisites

Before starting, ensure the following dependencies are installed:
- **SDL2**: Used for window management and rendering.
- **GLM**: A C++ library for matrix transformations.
- **FastNoiseLite**: For noise generation in shaders.

### Building the Project

Use CMake to build the project:

1. **Create a Build Directory**:
   ```bash
   mkdir build
   cd build
   cmake ..
   make
   ./SpaceTravel


### Cloning the Repository
Clone this repository to your local machine:
```bash
git clone https://github.com/mar22266/SpaceTravelGF-PY3.git
cd SpaceTravelGF-PY3
