#pragma once
#include <map>
#include <optional>
#include <raylib.h>
#include <string>

namespace SpaceInvaders {
class ResourceManager final {
public:
    ResourceManager() = default;
    ~ResourceManager();

    void LoadTextures(const std::string &path);
    void LoadSounds(const std::string &path);
    void LoadMusic(const std::string &path);
    void LoadFonts(const std::string &path);

    [[nodiscard]] std::optional<std::reference_wrapper<Texture2D>> GetTexture(const std::string &path);
    [[nodiscard]] std::optional<std::reference_wrapper<Sound>> GetSound(const std::string &path);
    [[nodiscard]] std::optional<std::reference_wrapper<Music>> GetMusic(const std::string &path);
    [[nodiscard]] std::optional<std::reference_wrapper<Font>> GetFont(const std::string &path);

private:
    std::map<std::string, Texture2D> m_texCache {};
    std::map<std::string, Sound> m_sndCache     {};
    std::map<std::string, Music> m_musCache     {};
    std::map<std::string, Font> m_fntCache      {};
};

}
