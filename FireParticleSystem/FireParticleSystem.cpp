#include "raylib.h"
#include <vector>
#include <cmath>
#include <algorithm>

#define SCREEN_WIDTH 1200
#define SCREEN_HEIGHT 800
#define BLACK_HOLE_PI 3.14159265359f

float GetRandomFloat(float min, float max) {
    return min + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (max - min)));
}

struct Particle {
    Vector2 position;
    Vector2 velocity;
    Color color;
    float mass;
    float lifetime;
    bool active;

    Particle() {
        Reset();
    }

    void Reset() {
        float angle = GetRandomFloat(0, BLACK_HOLE_PI * 2);
        float radius = GetRandomFloat(200, 300);
        position.x = static_cast<float>(SCREEN_WIDTH) / 2 + cosf(angle) * radius;
        position.y = static_cast<float>(SCREEN_HEIGHT) / 2 + sinf(angle) * radius;

        float speed = sqrt(2000.0f / radius) * 2.0f;
        velocity.x = -sinf(angle) * speed;
        velocity.y = cosf(angle) * speed;

        mass = GetRandomFloat(0.1f, 1.0f);
        lifetime = 1.0f;
        active = true;

        float temp = GetRandomFloat(0.5f, 1.0f);
        color.r = (unsigned char)(255 * temp);
        color.g = (unsigned char)(255 * temp);
        color.b = 255;
        color.a = 255;
    }
};

struct Planet {
    Vector2 position;
    Vector2 velocity;
    float size;
    float mass;
    Color color;
    bool active;
    float rotation;
    float stretchFactor;
    float originalSize;

    Planet(Vector2 pos) {
        position = pos;
        velocity = {0, 0};
        originalSize = GetRandomFloat(20, 40);
        size = originalSize;
        mass = size * 2.0f;
        active = true;
        rotation = 0;
        stretchFactor = 1.0f;
        
        color.r = (unsigned char)GetRandomFloat(100, 255);
        color.g = (unsigned char)GetRandomFloat(100, 255);
        color.b = (unsigned char)GetRandomFloat(100, 255);
        color.a = 255;
    }

private:
    float GetRandomFloat(float min, float max) {
        return min + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (max - min)));
    }
};

class BlackHole {
private:
    Vector2 position;
    float radius;
    float eventHorizonRadius;
    std::vector<Particle> particles;
    std::vector<Vector2> accretionDisk;
    std::vector<Planet> planets;
    float time;

    static const int NUM_PARTICLES = 1000;
    static const int DISK_SEGMENTS = 720;

public:
    BlackHole() :
        position({ SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 }),
        radius(30.0f),
        eventHorizonRadius(20.0f),
        time(0) {

        particles.resize(NUM_PARTICLES);

        for (int i = 0; i < DISK_SEGMENTS; i++) {
            float angle = (float)i * 2 * BLACK_HOLE_PI / DISK_SEGMENTS;
            float r = radius * 3 + sinf(angle * 3) * 5;
            Vector2 point = {
                cosf(angle) * r,
                sinf(angle) * r
            };
            accretionDisk.push_back(point);
        }
    }

    void AddPlanet(Vector2 pos) {
        planets.emplace_back(pos);
        Vector2 toCenter = {
            position.x - pos.x,
            position.y - pos.y
        };
        float dist = sqrt(toCenter.x * toCenter.x + toCenter.y * toCenter.y);
        float angle = atan2f(toCenter.y, toCenter.x);
        float orbitalSpeed = sqrt(2000.0f / dist) * 0.8f;
        planets.back().velocity = {
            -sinf(angle) * orbitalSpeed,
            cosf(angle) * orbitalSpeed
        };
    }

    void Update() {
        time += GetFrameTime();

        for (auto& particle : particles) {
            if (!particle.active) {
                particle.Reset();
                continue;
            }

            Vector2 toCenter = {
                position.x - particle.position.x,
                position.y - particle.position.y
            };
            float dist = sqrt(toCenter.x * toCenter.x + toCenter.y * toCenter.y);

            Vector2 direction = {
                toCenter.x / dist,
                toCenter.y / dist
            };

            float forceMagnitude = 2000.0f / (dist * dist);
            forceMagnitude = std::min(forceMagnitude, 50.0f);

            particle.velocity.x += direction.x * forceMagnitude;
            particle.velocity.y += direction.y * forceMagnitude;

            particle.position.x += particle.velocity.x * GetFrameTime();
            particle.position.y += particle.velocity.y * GetFrameTime();

            if (dist < eventHorizonRadius * 1.5f) {
                particle.lifetime -= GetFrameTime() * 2.0f;

                float angle = atan2f(toCenter.y, toCenter.x);
                angle += GetFrameTime() * 5.0f;
                float spiral_radius = std::max(dist * 0.95f, eventHorizonRadius);
                particle.position.x = position.x - cosf(angle) * spiral_radius;
                particle.position.y = position.y - sinf(angle) * spiral_radius;
            }

            if (dist < eventHorizonRadius || particle.lifetime <= 0) {
                particle.active = false;
            }

            float speed = sqrt(particle.velocity.x * particle.velocity.x +
                particle.velocity.y * particle.velocity.y);
            float redShift = std::min(speed / 200.0f, 1.0f);
            particle.color.r = 255;
            particle.color.g = (unsigned char)(255 * (1.0f - redShift * 0.7f));
            particle.color.b = (unsigned char)(255 * (1.0f - redShift * 0.9f));
            particle.color.a = (unsigned char)(255 * particle.lifetime);
        }

        // Update planets
        for (auto& planet : planets) {
            if (!planet.active) continue;

            Vector2 toCenter = {
                position.x - planet.position.x,
                position.y - planet.position.y
            };
            float dist = sqrt(toCenter.x * toCenter.x + toCenter.y * toCenter.y);
            
            Vector2 direction = {
                toCenter.x / dist,
                toCenter.y / dist
            };

            float tidalForce = 6000.0f / (dist * dist * dist);
            float criticalDistance = eventHorizonRadius * 3.0f;
            
            if (dist < criticalDistance) {
                float distanceFactor = (criticalDistance - dist) / criticalDistance;
                planet.stretchFactor = 1.0f + (tidalForce * distanceFactor * 5.0f);
                float width = planet.originalSize / sqrt(planet.stretchFactor);
                planet.size = std::min(width, planet.originalSize);
            }

            float forceMagnitude = 3000.0f / (dist * dist);
            forceMagnitude *= planet.mass;

            if (dist < eventHorizonRadius * 3.0f) {
                float angle = atan2f(toCenter.y, toCenter.x);
                float tangentialForce = forceMagnitude * 0.5f;
                planet.velocity.x += -sinf(angle) * tangentialForce * GetFrameTime();
                planet.velocity.y += cosf(angle) * tangentialForce * GetFrameTime();
                planet.rotation += forceMagnitude * 0.02f;
            }

            planet.velocity.x += direction.x * forceMagnitude * GetFrameTime();
            planet.velocity.y += direction.y * forceMagnitude * GetFrameTime();

            planet.position.x += planet.velocity.x * GetFrameTime();
            planet.position.y += planet.velocity.y * GetFrameTime();

            if (dist < eventHorizonRadius) {
                planet.active = false;
                int particleCount = static_cast<int>(40 * planet.stretchFactor);
                for (int i = 0; i < particleCount; i++) {
                    if (particles.size() < NUM_PARTICLES * 2) {
                        Particle p;
                        float offset = GetRandomFloat(-planet.originalSize * planet.stretchFactor, 
                                                   planet.originalSize * planet.stretchFactor);
                        p.position.x = planet.position.x + direction.x * offset;
                        p.position.y = planet.position.y + direction.y * offset;
                        p.color = planet.color;
                        p.lifetime = 1.0f;
                        
                        float explosionAngle = GetRandomFloat(0, BLACK_HOLE_PI * 2);
                        float explosionSpeed = GetRandomFloat(100, 300);
                        p.velocity.x = cosf(explosionAngle) * explosionSpeed;
                        p.velocity.y = sinf(explosionAngle) * explosionSpeed;
                        particles.push_back(p);
                    }
                }
            }
        }
    }

    void Draw() {
        DrawCircleGradient(position.x, position.y, radius * 4,
            ColorAlpha(BLACK, 0.2f), ColorAlpha(BLACK, 0.0f));

        BeginBlendMode(BLEND_ADDITIVE);
        for (size_t i = 0; i < accretionDisk.size(); i++) {
            float angle = (float)i * 2 * BLACK_HOLE_PI / DISK_SEGMENTS;
            float brightness = (1.0f + sinf(angle * 3 + time * 2)) * 0.5f;

            Vector2 point = accretionDisk[i];
            Vector2 screenPos = {
                position.x + point.x,
                position.y + point.y
            };

            Color diskColor = {
                255,
                (unsigned char)(200 * brightness),
                (unsigned char)(150 * brightness),
                (unsigned char)(255 * brightness)
            };

            DrawCircle(screenPos.x, screenPos.y, 2.0f, diskColor);
        }

        for (const auto& particle : particles) {
            if (!particle.active) continue;

            Vector2 trail = {
                particle.position.x - particle.velocity.x * 0.1f,
                particle.position.y - particle.velocity.y * 0.1f
            };

            DrawLineV(particle.position, trail,
                ColorAlpha(particle.color, particle.color.a * 0.5f));
            DrawCircleV(particle.position, 2.0f, particle.color);
        }

        //  spaghettification
        for (const auto& planet : planets) {
            if (!planet.active) continue;

            Vector2 toCenter = {
                position.x - planet.position.x,
                position.y - planet.position.y
            };
            float dist = sqrt(toCenter.x * toCenter.x + toCenter.y * toCenter.y);
            Vector2 direction = {
                toCenter.x / dist,
                toCenter.y / dist
            };

            // stretched atmosphere
            for (float i = 0; i < planet.stretchFactor; i += 0.2f) {
                float offset = (i - planet.stretchFactor * 0.5f) * planet.originalSize;
                Vector2 pos = {
                    planet.position.x + direction.x * offset,
                    planet.position.y + direction.y * offset
                };
                float atmosphereSize = planet.size * 1.2f * (1.0f - (i / planet.stretchFactor) * 0.3f);
                DrawCircleGradient(pos.x, pos.y, atmosphereSize,
                    ColorAlpha(planet.color, 0.1f), ColorAlpha(planet.color, 0.0f));
            }

            //  stretched planet body
            int segments = static_cast<int>(planet.stretchFactor * 10);
            for (int i = 0; i < segments; i++) {
                float t = static_cast<float>(i) / segments;
                float offset = (t - 0.5f) * planet.originalSize * planet.stretchFactor;
                Vector2 pos = {
                    planet.position.x + direction.x * offset,
                    planet.position.y + direction.y * offset
                };
                
                float segmentSize = planet.size * (1.0f - powf(fabsf(t - 0.5f) * 2, 0.5f));
                Color segmentColor = planet.color;
                segmentColor.a = (unsigned char)(255 * (1.0f - powf(fabsf(t - 0.5f) * 2, 0.5f)));
                
                DrawCircle(pos.x, pos.y, segmentSize, segmentColor);
            }
        }
        EndBlendMode();

        DrawCircleGradient(position.x, position.y, eventHorizonRadius,
            BLACK, ColorAlpha(BLACK, 0.0f));
        DrawCircle(position.x, position.y, radius * 0.5f, BLACK);
    }
};

int main() {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Black Hole Simulation");
    SetTargetFPS(60);

    BlackHole blackHole;

    while (!WindowShouldClose()) {
        if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) {
            blackHole.AddPlanet(GetMousePosition());
        }

        blackHole.Update();

        BeginDrawing();
        ClearBackground(BLACK);
        blackHole.Draw();
        DrawText("Right Click: Spawn Planet", 10, 10, 20, WHITE);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}