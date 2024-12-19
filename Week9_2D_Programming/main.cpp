#include "raylib.h"
#include <vector>
#include <cmath>
#include <algorithm>

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
const int GRID_COLS = 40;
const int GRID_ROWS = 40;
const float PARTICLE_RADIUS = 2.0f;
const float STICK_LENGTH = 8.0f;
const int PHYSICS_STEPS = 3;

// Forward declarations
struct Particle;
struct Stick;

// Particle definition
struct Particle {
    Vector2 position;
    Vector2 prevPosition;
    float heat;
    bool locked;

    Particle(float x, float y) {
        position = { x, y };
        prevPosition = position;
        heat = 0.0f;
        locked = false;
    }
};

// Stick definition
struct Stick {
    int p1, p2;
    float length;

    Stick(int particle1, int particle2, float targetLength) {
        p1 = particle1;
        p2 = particle2;
        length = targetLength;
    }
};

class ParticleSystem {
private:
    std::vector<Particle> particles;
    std::vector<Stick> sticks;
    float time;
    unsigned int randomSeed;

    float GetRandomFloat(float min, float max) {
        return min + (rand() / (float)RAND_MAX) * (max - min);
    }

    float Noise(float x, float y) {
        float noise1 = sinf(x * 0.1f * randomSeed / 100000.0f + time) *
            cosf(y * 0.1f - time * 0.5f);
        float noise2 = cosf((x + y) * 0.1f + time * 1.5f) *
            sinf(x * 0.2f - y * 0.1f + time * 0.7f);
        float noise3 = sinf(x * 0.02f + y * 0.03f + time * 0.9f);

        return (noise1 + noise2 + noise3) / 3.0f;
    }

public:
    ParticleSystem() : time(0) {
        Reset();
    }

    void Reset() {
        randomSeed = GetTime() * 1000000.0f;
        srand(randomSeed);

        particles.clear();
        sticks.clear();
        time = GetRandomFloat(0, 1000.0f);

        float startX = (SCREEN_WIDTH - (GRID_COLS - 1) * STICK_LENGTH) / 2;
        float startY = SCREEN_HEIGHT - GRID_ROWS * STICK_LENGTH - 50;

        // Create particles
        for (int y = 0; y < GRID_ROWS; y++) {
            for (int x = 0; x < GRID_COLS; x++) {
                float posX = startX + x * STICK_LENGTH + GetRandomFloat(-3, 3);
                float posY = startY + y * STICK_LENGTH + GetRandomFloat(-3, 3);
                particles.emplace_back(posX, posY);
            }
        }

        // Create connections
        for (int y = 0; y < GRID_ROWS; y++) {
            for (int x = 0; x < GRID_COLS; x++) {
                int i = y * GRID_COLS + x;

                if (x < GRID_COLS - 1) {
                    sticks.emplace_back(i, i + 1, STICK_LENGTH);
                }
                if (y < GRID_ROWS - 1) {
                    sticks.emplace_back(i, i + GRID_COLS, STICK_LENGTH);
                }
            }
        }
    }

    void Update() {
        time += GetFrameTime();

        for (int step = 0; step < PHYSICS_STEPS; step++) {
            UpdateParticles();
            ApplyConstraints();
            ApplyHeat();
        }
    }

    void UpdateParticles() {
        const float gravity = 0.15f;
        const float maxSpeed = 2.0f;
        const float baseRiseSpeed = 0.8f;

        for (auto& p : particles) {
            if (p.locked) continue;

            Vector2 velocity = {
                p.position.x - p.prevPosition.x,
                p.position.y - p.prevPosition.y
            };

            p.prevPosition = p.position;

            float turbulence = Noise(p.position.x, p.position.y) *
                GetRandomFloat(0.2f, 0.4f);

            if (p.heat > 0.1f) {
                float heatFactor = p.heat * GetRandomFloat(0.7f, 0.9f);

                float riseSpeed = baseRiseSpeed * heatFactor *
                    GetRandomFloat(0.9f, 1.1f);
                riseSpeed = fminf(riseSpeed, maxSpeed);
                p.position.y -= riseSpeed;

                float horizontalMove = turbulence * heatFactor *
                    GetRandomFloat(0.4f, 0.6f);
                horizontalMove = fminf(fmaxf(horizontalMove, -maxSpeed), maxSpeed);
                p.position.x += horizontalMove;

                float spiral = sinf(p.position.y * GetRandomFloat(0.009f, 0.011f) +
                    time * GetRandomFloat(1.4f, 1.6f)) *
                    GetRandomFloat(0.25f, 0.35f);
                p.position.x += spiral * heatFactor;
            }

            velocity.x *= GetRandomFloat(0.97f, 0.99f);
            velocity.y *= GetRandomFloat(0.97f, 0.99f);

            p.position.x += velocity.x;
            p.position.y += velocity.y + gravity * GetRandomFloat(0.95f, 1.05f);

            Vector2 currentVelocity = {
                p.position.x - p.prevPosition.x,
                p.position.y - p.prevPosition.y
            };
            float speed = sqrt(currentVelocity.x * currentVelocity.x +
                currentVelocity.y * currentVelocity.y);
            if (speed > maxSpeed) {
                float scale = maxSpeed / speed;
                p.position.x = p.prevPosition.x + currentVelocity.x * scale;
                p.position.y = p.prevPosition.y + currentVelocity.y * scale;
            }
        }
    }

    void ApplyConstraints() {
        for (auto& stick : sticks) {
            Particle& p1 = particles[stick.p1];
            Particle& p2 = particles[stick.p2];

            float dx = p2.position.x - p1.position.x;
            float dy = p2.position.y - p1.position.y;
            float distance = sqrt(dx * dx + dy * dy);
            if (distance > 0.0001f) {
                float difference = stick.length - distance;
                float percent = difference / distance / 2;

                Vector2 offset = {
                    dx * percent,
                    dy * percent
                };

                if (!p1.locked) {
                    p1.position.x -= offset.x;
                    p1.position.y -= offset.y;
                }
                if (!p2.locked) {
                    p2.position.x += offset.x;
                    p2.position.y += offset.y;
                }
            }
        }

        // Screen bounds
        for (auto& p : particles) {
            if (p.locked) continue;
            p.position.x = fminf(fmaxf(p.position.x, 0), SCREEN_WIDTH);
            p.position.y = fminf(fmaxf(p.position.y, 0), SCREEN_HEIGHT);
        }
    }

    void ApplyHeat() {
        float centerX = SCREEN_WIDTH / 2 + sinf(time * 0.5f) * 20.0f;
        float heatWidth = 200 + sinf(time * 0.7f) * 20.0f;

        for (auto& p : particles) {
            if (p.position.y > SCREEN_HEIGHT - 80) {
                float distFromCenter = fabs(p.position.x - centerX);
                if (distFromCenter < heatWidth) {
                    float baseLine = sinf(p.position.x * 0.03f + time * 1.5f +
                        GetRandomFloat(-0.1f, 0.1f)) * 10.0f;
                    if (p.position.y > SCREEN_HEIGHT - 60 + baseLine) {
                        p.heat = (1.0f - (distFromCenter / heatWidth)) *
                            GetRandomFloat(0.7f, 0.9f);
                        p.heat *= (1.0f + sinf(time * 2.0f + p.position.x * 0.05f) *
                            GetRandomFloat(0.05f, 0.15f));
                    }
                }
            }

            if (p.heat > 0.1f) {
                for (auto& other : particles) {
                    if (&other != &p) {
                        float dx = other.position.x - p.position.x;
                        float dy = other.position.y - p.position.y;
                        float dist = sqrt(dx * dx + dy * dy);
                        if (dist < STICK_LENGTH * 2 && other.position.y < p.position.y) {
                            other.heat = std::max(other.heat,
                                p.heat * GetRandomFloat(0.93f, 0.97f));
                        }
                    }
                }
            }

            p.heat *= GetRandomFloat(0.994f, 0.996f);
        }
    }

    void Draw() {
        BeginBlendMode(BLEND_ADDITIVE);
        for (const auto& p : particles) {
            if (p.heat > 0.05f) {
                Color innerColor = {
                    255,
                    (unsigned char)(255 * p.heat * 0.7f),
                    (unsigned char)(p.heat * p.heat * 100),
                    (unsigned char)(255 * p.heat)
                };

                Color outerColor = {
                    255,
                    (unsigned char)(100 * p.heat),
                    0,
                    (unsigned char)(255 * p.heat * 0.3f)
                };

                float baseRadius = PARTICLE_RADIUS * (1.0f + p.heat * 2.0f);
                DrawCircle(p.position.x, p.position.y, baseRadius * 4.0f, ColorAlpha(outerColor, 0.1f));
                DrawCircle(p.position.x, p.position.y, baseRadius * 2.0f, ColorAlpha(outerColor, 0.2f));
                DrawCircle(p.position.x, p.position.y, baseRadius * 1.5f, ColorAlpha(innerColor, 0.3f));
                DrawCircle(p.position.x, p.position.y, baseRadius, innerColor);
            }
        }
        EndBlendMode();
    }
};

int main() {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Realistic Fire");
    SetTargetFPS(60);

    ParticleSystem system;

    while (!WindowShouldClose()) {
        if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
            system.Reset();
        }

        system.Update();

        BeginDrawing();
        ClearBackground(BLACK);
        system.Draw();
        DrawText("Right Click: Restart Simulation", 10, 10, 20, WHITE);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}