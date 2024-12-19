//#pragma once
//#ifndef FIRE_PARTICLE_SYSTEM_H
//#define FIRE_PARTICLE_SYSTEM_H
//
//#include "raylib.h"
//#include <vector>
//
//struct FireParticle {
//    Vector2 position;
//    Vector2 oldPosition;
//    Vector2 acceleration;
//    float lifetime;
//    float maxLifetime;
//    Color color;
//};
//
//class FireParticleSystem {
//private:
//    std::vector<FireParticle> particles;
//    Vector2 origin;
//    const int MAX_PARTICLES = 500;
//    const int SUBSTEPS = 8;
//
//public:
//    FireParticleSystem(Vector2 position);
//    void update(float deltaTime);
//    void draw();
//    void addParticle();
//    void verletIntegration(float dt);
//    void solveConstraints();
//};
//
//#endif
