#pragma once
#include <cstdint>
#include <raylib.h>

namespace SpaceInvaders {
class BloomRenderer {
public:
    BloomRenderer() = default;
    ~BloomRenderer();

    void Init(int screenW, int screenH);

    void ResizeIfNeeded(int screenW, int screenH);

    void SetThreshold(const float t)  { m_threshold = t; if (m_inited) pushBrightUniforms(); }
    void SetKnee(const float k)       { m_knee = k; if (m_inited) pushBrightUniforms(); }
    void SetBlurRadius(const float r) { m_radius = r; }

    void BeginScene() const;
    void EndScene();
    void Apply() const;
    void Composite(float bloomAlpha = 1.0f) const;

    [[nodiscard]] const RenderTexture2D &GetSceneRT() const { return m_scene; }

private:
    void destroy();
    void pushBrightUniforms() const;
    void setBlurUniforms(const RenderTexture2D& src, const Vector2& dir) const;

private:
    bool m_inited   {false};
    int32_t m_w     {0};
    int32_t m_h     {0};
    int32_t m_hw    {0};
    int32_t m_hh    {0};

    RenderTexture2D m_scene{};
    RenderTexture2D m_bright{};
    RenderTexture2D m_pingA{};
    RenderTexture2D m_pingB{};

    Shader m_shBright{};
    Shader m_shBlur{};

    int m_locThreshold{-1}, m_locKnee{-1};
    int m_locTexel{-1}, m_locDir{-1}, m_locRadius{-1};

    float m_threshold{0.60f};
    float m_knee{0.20f};
    float m_radius{4.0f};
};
}