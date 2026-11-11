# BasicEngine

A step-by-step journey building a basic game engine in C++.

## Learning Roadmap

### Step 0 — Hello World

**Goal:** Verify the environment is set up correctly, including GLFW

**Source:** [View Step 0](https://github.com/xenychtisdev/BasicEngine/tree/Step0-GLFW-HelloWorld)

### Step 1 — Draw a Triangle

**Goal:** Draw a simple triangle to the screen, using a basic engine layout

**Source:** [View Step 1](https://github.com/xenychtisdev/BasicEngine/tree/Step1-DrawTriangle)

### Step 2 — Transform a Triangle

**Goal:** Move, rotate, and scale the triangle with keyboard input, using delta time and GLM transformation matrices

**Source:** [View Step 2](https://github.com/xenychtisdev/BasicEngine/tree/Step2-TransformTriangle)

### Step 3 — Draw a Textured Quad

**Goal:** Load a PNG with stb_image and render it on a quad using indexed drawing an texture coordinates. Use alpha blending to preserve the image's transparency and adjust its opacity at runtime. The existing keyboard controls move, rotate, and scale the sprite.

**Source:** [View Step 3](https://github.com/xenychtisdev/BasicEngine/tree/Step3-TexturedQuad)

### Step 4 — Add a 2D Camera

**Goal:** Add an orthographic camera with arrow-key panning and aspect-ration correction. Render a small scene with a controllable sprite and stationary landmarks. Scale the sprite's movement speed with its size, and hold LShift to sprint.

**Source:** [View Step 4](https://github.com/xenychtisdev/BasicEngine/tree/Step4-2DCamera)

### Step 5 — Add Entities and Components

**Goal:** Introduce EnTT and store the player, landmarks, and drifters as entities in a scene registry. Use Transform and Srpite components to draw entities, and a Velocity component to move drifters automatically. Click the scene to spawn a drifter at the cursor's world position. Press R to reset the player and remove all drifters.

**Source:** [View Step 5](https://github.com/xenychtisdev/BasicEngine/tree/Step5-EntityComponents)

## How to Follow Along

Each step represents a milestone in the development of BasicEngine. Browse the source code for that stage to see the changes introduced as the engine evolves.

## Technologies

- C++
- CMake
- OpenGL
- GLFW
- GLAD
- GLM
- Dear ImGui
- stb_image
- EnTT
