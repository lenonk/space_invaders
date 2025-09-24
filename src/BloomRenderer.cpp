#include "../include/BloomRenderer.h"
#include <algorithm>

#include "Game.h"

namespace SpaceInvaders {
static Rectangle
SrcRect(const RenderTexture2D &rt) {
    return {0, 0, static_cast<float>(rt.texture.width), -static_cast<float>(rt.texture.height)};
}

static Rectangle
DstRect(const int32_t w, const int32_t h) { return {0, 0, static_cast<float>(w), static_cast<float>(h)}; }

BloomRenderer::~BloomRenderer() { destroy(); }

void
BloomRenderer::destroy() {
    if (!m_inited) return;
    UnloadRenderTexture(m_scene);
    UnloadRenderTexture(m_bright);
    UnloadRenderTexture(m_pingA);
    UnloadRenderTexture(m_pingB);
    m_inited = false;
}

void
BloomRenderer::Init(const int32_t screenW, const int32_t screenH) {
    destroy();
    m_w = screenW;
    m_h = screenH;
    m_hw = std::max(1, screenW / 2);
    m_hh = std::max(1, screenH / 2);

    m_scene = LoadRenderTexture(m_w, m_h);
    m_bright = LoadRenderTexture(m_hw, m_hh);
    m_pingA = LoadRenderTexture(m_hw, m_hh);
    m_pingB = LoadRenderTexture(m_hw, m_hh);

    if (const auto &shader = Game::Resources->Get<Shader>("bright.fs"); shader.has_value()) {
        m_shBright = shader.value();
        m_locThreshold = GetShaderLocation(m_shBright, "u_threshold");
        m_locKnee = GetShaderLocation(m_shBright, "u_knee");
    } else {
        throw std::runtime_error("Failed to load bright shader");
    }

    if (const auto &shader = Game::Resources->Get<Shader>("blur.fs"); shader.has_value()) {
        m_shBlur = shader.value();
        m_locTexel = GetShaderLocation(m_shBlur, "u_texelSize");
        m_locDir = GetShaderLocation(m_shBlur, "u_direction");
        m_locRadius = GetShaderLocation(m_shBlur, "u_radius");
    } else {
        throw std::runtime_error("Failed to load blur shader");
    }

    pushBrightUniforms();
    m_inited = true;
}

void
BloomRenderer::ResizeIfNeeded(const int32_t screenW, const int32_t screenH) {
    if (screenW == m_w && screenH == m_h) return;
    Init(screenW, screenH);
}

void
BloomRenderer::pushBrightUniforms() const {
    SetShaderValue(m_shBright, m_locThreshold, &m_threshold, SHADER_UNIFORM_FLOAT);
    SetShaderValue(m_shBright, m_locKnee, &m_knee, SHADER_UNIFORM_FLOAT);
}

void
BloomRenderer::BeginScene() const {
    BeginTextureMode(m_scene);
    ClearBackground(BLACK);
}

void
BloomRenderer::EndScene() {
    EndTextureMode();
}

void
BloomRenderer::setBlurUniforms(const RenderTexture2D &src, const Vector2 &dir) const {
    const Vector2 texel = {1.0f / static_cast<float>(src.texture.width), 1.0f / static_cast<float>(src.texture.height)};
    SetShaderValue(m_shBlur, m_locTexel, &texel, SHADER_UNIFORM_VEC2);
    SetShaderValue(m_shBlur, m_locDir, &dir, SHADER_UNIFORM_VEC2);
    SetShaderValue(m_shBlur, m_locRadius, &m_radius, SHADER_UNIFORM_FLOAT);
}

void
BloomRenderer::Apply() const {
    // Bright pass (downsample scene -> bright)
    BeginTextureMode(m_bright);
    ClearBackground(Colors::Black);
    BeginShaderMode(m_shBright);
    DrawTexturePro(m_scene.texture, SrcRect(m_scene), DstRect(m_hw, m_hh), {0, 0}, 0.0f, Colors::White);
    EndShaderMode();
    EndTextureMode();

    // Blur H: bright -> pingA
    BeginTextureMode(m_pingA);
    ClearBackground(Colors::Black);
    BeginShaderMode(m_shBlur);
    setBlurUniforms(m_bright, {1.0f, 0.0f});
    DrawTexturePro(m_bright.texture, SrcRect(m_bright), DstRect(m_hw, m_hh), {0, 0}, 0.0f, Colors::White);
    EndShaderMode();
    EndTextureMode();

    // Blur V: pingA -> pingB
    BeginTextureMode(m_pingB);
    ClearBackground(Colors::Black);
    BeginShaderMode(m_shBlur);
    setBlurUniforms(m_pingA, {0.0f, 1.0f});
    DrawTexturePro(m_pingA.texture, SrcRect(m_pingA), DstRect(m_hw, m_hh), {0, 0}, 0.0f, Colors::White);
    EndShaderMode();
    EndTextureMode();
}

void
BloomRenderer::Composite(const float bloomAlpha) const {
    // Draw scene to backbuffer
    DrawTexturePro(m_scene.texture, SrcRect(m_scene), DstRect(m_w, m_h), {0, 0}, 0.0f, Colors::White);

    // Add bloom
    BeginBlendMode(BLEND_ADDITIVE);
    DrawTexturePro(m_pingB.texture, SrcRect(m_pingB), DstRect(m_w, m_h), {0, 0}, 0.0f, ColorAlpha(Colors::White, bloomAlpha));
    EndBlendMode();
}
}