#pragma once
#ifndef FIRE_PARTICLE_SYSTEM_H
#define FIRE_PARTICLE_SYSTEM_H

#include "raylib.h"
#include <vector>

struct FireParticle {
    Vector2 position;
    Vector2 oldPosition;
    Vector2 acceleration;
    float lifetime;
    float maxLifetime;
    float temperature;  // New temperature property
    Color color;
};

class FireParticleSystem {
private:
    std::vector<FireParticle> particles;
    Vector2 origin;
    const int MAX_PARTICLES = 145;
    const int SUBSTEPS = 8;
    const float INTERACTION_RADIUS = 15.0f;  // Radius for particle interaction
    const float HEAT_TRANSFER_RATE = 0.3f;   // Rate of heat transfer between particles

public:
    FireParticleSystem(Vector2 position);
    void update(float deltaTime);
    void draw();
    void addParticle();
    void verletIntegration(float dt);
    void solveConstraints();
    void transferHeat();
    Color getColorFromTemperature(float temperature);
};

#endif
