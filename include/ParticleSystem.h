#pragma once
#include <vector>
#include <raylib.h>

#include "Colors.h"

namespace SpaceInvaders {

enum class BurstType {
    Laser,
    Alien,
    Player,
    Mystery
};

struct Particle {
    Vector2 pos{};
    Vector2 vel{};
    float life{0.f}, maxLife{1.f};
    float size{2.f}, endSize{0.f};
    float rotation{0.f}, angularVel{0.f};
    Color start{}, end{};
    float damping{0.0f};   // 0..1 per second
    float gravity{1.0f};   // px/s^2 (positive = down)
    bool additive{false};
};

class ParticleSystem {
public:
    ParticleSystem() = default;
    ~ParticleSystem() = default;

    void Emit(BurstType type, Vector2 at, Color tint = Colors::White);
    void Update(float dt);
    void Draw() const;

    void Clear();

private:
    void emitLaser(Vector2 at);
    void emitAlien(Vector2 at, Color tint);
    void emitPlayer(Vector2 at);
    void emitMystery(Vector2 at);

    static Color lerp(const Color &a, const Color &b, float t);

private:
    std::vector<Particle> m_particles;
    int m_softDiscTex{-1};
    mutable std::vector<int> m_alphaIdx;
    mutable std::vector<int> m_additiveIdx;

    void ensureSoftDiscTexture();
    void drawParticle(const Particle &p) const;
};

}