# 2DFinal
Final project for 2D programming 
# Black Hole Simulation

A visually pleasing and physically-inspired simulation of a black hole with realistic gravitational effects, particle systems, and spaghettification of planets.
![image alt](https://github.com/DanielGV22/2DFinal/blob/bd26b0637fdf863a89dcf64b7f557531ee074c20/Capture.PNG)

## Features

- Interactive black hole simulation with accretion disk
- Realistic gravitational physics
- Particle system with dynamic behavior
- Planet spawning with right-click
- Spaghettification effect as planets approach the event horizon
- Visual effects including:
  - Gravitational lensing
  - Red shift
  - Particle trails
  - Dynamic lighting
  - Atmospheric distortion

## Controls

- **Right Click**: Spawn a planet at cursor location
- **ESC**: Exit the simulation

## Physics Simulation

The simulation includes several physical phenomena:
- Gravitational forces (inverse square law)
- Tidal forces causing spaghettification
- Orbital mechanics
- Particle dynamics
- Event horizon effects

## Technical Details

### Dependencies
- Raylib (for graphics and window management)
- C++ Standard Library
- C++ 11 or higher

### Resolution
- Default window size: 1200x800 pixels

### Performance
- Particle count: 1000
- Accretion disk segments: 720
- Target FPS: 60

## Building the Project

1. Make sure you have Raylib installed
2. Compile using a C++ compiler with C++11 support
3. Link against Raylib

Example compilation (using g++):
```bash
g++ -o blackhole main.cpp -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
