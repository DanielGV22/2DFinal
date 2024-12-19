//#include "FireParticleSystem.h"
//#include <cstdlib>
//
//FireParticleSystem::FireParticleSystem(Vector2 position) {
//    origin = position;
//}
//
//void FireParticleSystem::addParticle() {
//    if (particles.size() >= MAX_PARTICLES) return;
//
//    FireParticle particle;
//    particle.position = origin;
//    particle.position.x += GetRandomValue(-10, 10);
//    particle.oldPosition = particle.position;
//    particle.acceleration = { 0, -400 }; // Upward acceleration
//    particle.maxLifetime = GetRandomValue(5, 15) / 10.0f;
//    particle.lifetime = particle.maxLifetime;
//    particle.color = Color{ 255, static_cast<unsigned char>(GetRandomValue(100, 200)), 0, 255 };
//
//
//    particles.push_back(particle);
//}
//
//void FireParticleSystem::verletIntegration(float dt) {
//    for (auto& particle : particles) {
//        Vector2 temp = particle.position;
//        float friction = 0.99f;
//
//        particle.position.x = particle.position.x + (particle.position.x - particle.oldPosition.x) * friction + particle.acceleration.x * dt * dt;
//        particle.position.y = particle.position.y + (particle.position.y - particle.oldPosition.y) * friction + particle.acceleration.y * dt * dt;
//
//        particle.oldPosition = temp;
//
//        // Add some random movement for more realistic fire
//        particle.position.x += GetRandomValue(-2, 2);
//    }
//}
//
//void FireParticleSystem::solveConstraints() {
//    for (auto& particle : particles) {
//        // Keep particles within screen bounds
//        if (particle.position.x < 0) particle.position.x = 0;
//        if (particle.position.x > GetScreenWidth()) particle.position.x = GetScreenWidth();
//        if (particle.position.y < 0) particle.position.y = 0;
//        if (particle.position.y > GetScreenHeight()) particle.position.y = GetScreenHeight();
//    }
//}
//
//void FireParticleSystem::update(float deltaTime) {
//    // Add new particles
//    if (GetRandomValue(0, 2) == 0) {
//        addParticle();
//    }
//
//    // Update using substeps for better stability
//    float subDt = deltaTime / SUBSTEPS;
//    for (int i = 0; i < SUBSTEPS; i++) {
//        verletIntegration(subDt);
//        solveConstraints();
//    }
//
//    // Update lifetime and remove dead particles
//    for (int i = particles.size() - 1; i >= 0; i--) {
//        particles[i].lifetime -= deltaTime;
//        if (particles[i].lifetime <= 0) {
//            particles.erase(particles.begin() + i);
//        }
//    }
//}
//
//void FireParticleSystem::draw() {
//    for (const auto& particle : particles) {
//        Color particleColor = particle.color;
//        particleColor.a = (unsigned char)(255.0f * (particle.lifetime / particle.maxLifetime));
//        DrawCircleV(particle.position, 3.0f, particleColor);
//    }
//}
