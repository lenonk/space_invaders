#include <algorithm>
#include <cmath>

#include "ParticleSystem.h"

namespace SpaceInvaders {

static float
frand(const float a, const float b) {
    return a + (b - a) * static_cast<float>(GetRandomValue(0, 10000)) / 10000.f;
}

static Vector2 frandDir(float speed, float spreadRad){
    // Random base orientation for the cone
    const float base = frand(0.0f, 2.0f * PI);
    // Random offset inside the cone
    const float off  = frand(-spreadRad * 0.5f, spreadRad * 0.5f);
    const float ang  = base + off;
    return Vector2{ cosf(ang) * speed, sinf(ang) * speed };
}

void
ParticleSystem::Clear() {
    m_particles.clear();
}

void
ParticleSystem::Emit(const BurstType type, const Vector2 at, const Color tint) {
    switch (type) {
        case BurstType::Laser: emitLaser(at);
            break;
        case BurstType::Alien: emitAlien(at, tint);
            break;
        case BurstType::Player: emitPlayer(at);
            break;
        case BurstType::Mystery: emitMystery(at);
            break;
    }
}

void
ParticleSystem::Update(float dt) {
    dt = std::min(dt, 1.0f / 15.0f);
    auto it = m_particles.begin();
    while (it != m_particles.end()) {
        auto &p = *it;
        p.life -= dt;
        if (p.life <= 0.f) {
            it = m_particles.erase(it);
            continue;
        }
        // basic physics
        p.vel.y += p.gravity * dt;
        p.pos.x += p.vel.x * dt;
        p.pos.y += p.vel.y * dt;
        p.rotation += p.angularVel * dt;
        if (p.damping > 0.f) {
            const auto k = std::max(0.f, 1.f - p.damping * dt);
            p.vel.x *= k;
            p.vel.y *= k;
            p.angularVel *= k;
        }
        ++it;
    }
}

Color
ParticleSystem::lerp(const Color &a, const Color &b, float t) {
    t = std::clamp(t, 0.f, 1.f);
    return Color{
        static_cast<unsigned char>((1 - t) * a.r + t * b.r),
        static_cast<unsigned char>((1 - t) * a.g + t * b.g),
        static_cast<unsigned char>((1 - t) * a.b + t * b.b),
        static_cast<unsigned char>((1 - t) * a.a + t * b.a)
    };
}

void
ParticleSystem::ensureSoftDiscTexture() {
    if (m_softDiscTex != -1) return;
    constexpr int N = 16;
    const auto img = GenImageColor(N, N, BLANK);
    const auto px = static_cast<Color *>(img.data);
    for (int y = 0; y < N; y++) {
        for (int x = 0; x < N; x++) {
            const auto dx = (x + 0.5f) / N * 2.f - 1.f;
            const auto dy = (y + 0.5f) / N * 2.f - 1.f;
            const auto r = sqrtf(dx * dx + dy * dy);
            const auto a = std::clamp(1.0f - r, 0.0f, 1.0f);
            px[y * N + x] = Color{255, 255, 255, static_cast<unsigned char>(a * 255)};
        }
    }
    const Texture2D tex = LoadTextureFromImage(img);
    UnloadImage(img);
    m_softDiscTex = tex.id;
}

void
ParticleSystem::drawParticle(const Particle &p) const {
    const auto t = 1.f - p.life / p.maxLife;
    const Color c = lerp(p.start, p.end, t);
    const auto sz = p.size + (p.endSize - p.size) * t;

    if (m_softDiscTex != -1) {
        Texture2D tex {};
        tex.id = m_softDiscTex;
        tex.width = tex.height = 16;
        const Rectangle src{0, 0, static_cast<float>(tex.width), static_cast<float>(tex.height)};
        const Rectangle dst{p.pos.x, p.pos.y, sz * 2.f, sz * 2.f};
        const Vector2 origin{sz, sz};
        DrawTexturePro(tex, src, dst, origin, p.rotation * RAD2DEG, c);
    }
    else {
        DrawCircleV(p.pos, sz, c);
    }
}

void
ParticleSystem::Draw() const {
    m_alphaIdx.clear();
    m_additiveIdx.clear();
    m_alphaIdx.reserve(m_particles.size());
    m_additiveIdx.reserve(m_particles.size());
    for (size_t i = 0; i < m_particles.size(); ++i) {
        (m_particles[i].additive ? m_additiveIdx : m_alphaIdx).push_back(static_cast<int>(i));
    }

    // alpha blend first
    if (!m_alphaIdx.empty()) {
        for (const int i: m_alphaIdx) drawParticle(m_particles[i]);
    }
    if (!m_additiveIdx.empty()) {
        BeginBlendMode(BLEND_ADDITIVE);
        for (const auto i: m_additiveIdx) drawParticle(m_particles[i]);
        EndBlendMode();
    }
}

void
ParticleSystem::emitLaser(const Vector2 at) {
    ensureSoftDiscTexture();
    const auto count = GetRandomValue(18, 28);
    for (int i = 0; i < count; i++) {
        Particle p;
        p.pos = at;
        p.maxLife = p.life = frand(0.22f, 0.45f);
        const auto sp = frand(120.f, 220.f);
        p.vel = frandDir(sp, 2.44f); // ~140 deg
        p.size = 3.f;
        p.endSize = 0.f;
        p.start = Color{255, 216, 74, 255};
        p.end = Color{255, 216, 74, 0};
        p.gravity = 220.f;
        p.damping = 0.8f; // mild drag
        p.additive = true;
        m_particles.push_back(p);
    }
}

void
ParticleSystem::emitAlien(const Vector2 at, const Color tint) {
    ensureSoftDiscTexture();
    const auto count = GetRandomValue(50, 80);
    for (int i = 0; i < count; i++) {
        Particle p;
        p.pos = at;
        p.maxLife = p.life = frand(0.35f, 0.75f);
        const auto sp = frand(140.f, 260.f);
        p.vel = frandDir(sp, PI);
        p.size = 4.f;
        p.endSize = 1.f;
        const auto start = Color{
            static_cast<unsigned char>(std::min(255, tint.r + 40)),
            static_cast<unsigned char>(std::min(255, tint.g + 60)),
            static_cast<unsigned char>(std::min(255, tint.b + 80)), 255
        };
        p.start = start;
        p.end = Color{start.r, start.g, start.b, 0};
        p.gravity = 250.f;
        p.damping = 0.9f;
        p.additive = true;
        m_particles.push_back(p);
    }
}

void
ParticleSystem::emitPlayer(const Vector2 at) {
    ensureSoftDiscTexture();
    // yellow core
    const int count = GetRandomValue(120, 160);
    for (int i = 0; i < count; i++) {
        Particle p;
        p.pos = at;
        p.maxLife = p.life = frand(0.5f, 1.0f);
        const auto sp = frand(160.f, 320.f);
        p.vel = frandDir(sp, 4.19f); // ~240°
        p.size = 5.f;
        p.endSize = 0.f;
        p.start = Color{255, 220, 90, 255};
        p.end = Color{255, 220, 90, 0};
        p.gravity = 300.f;
        p.damping = 0.85f;
        p.additive = true;
        m_particles.push_back(p);
    }
    // cyan motes linger
    const auto moteCount = GetRandomValue(18, 28);
    for (int i = 0; i < moteCount; i++) {
        Particle p;
        p.pos = at;
        p.maxLife = p.life = frand(0.9f, 1.4f);
        const auto sp = frand(80.f, 140.f);
        p.vel = frandDir(sp, 3.14f);
        p.size = 3.f;
        p.endSize = 0.5f;
        p.start = Color{130, 255, 232, 200};
        p.end = Color{130, 255, 232, 0};
        p.gravity = 160.f;
        p.damping = 0.9f;
        p.additive = true;
        m_particles.push_back(p);
    }
}

void
ParticleSystem::emitMystery(const Vector2 at) {
    ensureSoftDiscTexture();
    // red burst
    const auto count = GetRandomValue(100, 140);
    for (int i = 0; i < count; i++) {
        Particle p;
        p.pos = at;
        p.maxLife = p.life = frand(0.5f, 0.9f);
        const auto sp = frand(180.f, 320.f);
        p.vel = frandDir(sp, 3.8f);
        p.size = 5.f;
        p.endSize = 1.f;
        p.start = Color{220, 70, 70, 255};
        p.end = Color{220, 70, 70, 0};
        p.gravity = 260.f;
        p.damping = 0.88f;
        p.additive = true;
        m_particles.push_back(p);
    }
    // cyan underglow
    const auto glowCount = GetRandomValue(18, 26);
    for (int i = 0; i < glowCount; i++) {
        Particle p;
        p.pos = at;
        p.maxLife = p.life = frand(0.6f, 1.0f);
        const auto sp = frand(90.f, 140.f);
        p.vel = frandDir(sp, 2.6f);
        p.size = 3.5f;
        p.endSize = 0.8f;
        p.start = Color{92, 255, 232, 200};
        p.end = Color{92, 255, 232, 0};
        p.gravity = 200.f;
        p.damping = 0.9f;
        p.additive = true;
        m_particles.push_back(p);
    }
}
}
