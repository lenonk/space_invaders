#pragma once
#include <map>
#include <optional>
#include <raylib.h>
#include <string>

namespace SpaceInvaders {

// Resource loading traits - specialized for each type
template<typename T>
struct ResourceTraits;

template<>
struct ResourceTraits<Texture2D> {
    static Texture2D Load(const char *path) { return LoadTexture(path); }
    static bool IsValid(const Texture2D &resource) { return IsTextureValid(resource); }
    static constexpr const char *TypeName() { return "texture"; }
};

template<>
struct ResourceTraits<Sound> {
    static Sound Load(const char *path) { return LoadSound(path); }
    static bool IsValid(const Sound &resource) { return IsSoundValid(resource); }
    static constexpr const char *TypeName() { return "sound"; }
};

template<>
struct ResourceTraits<Music> {
    static Music Load(const char *path) { return LoadMusicStream(path); }
    static bool IsValid(const Music &resource) { return IsMusicValid(resource); }
    static constexpr const char *TypeName() { return "music"; }
};

template<>
struct ResourceTraits<Font> {
    static Font Load(const char *path) { return LoadFontEx(path, 64, nullptr, 0); }
    static bool IsValid(const Font &resource) { return IsFontValid(resource); }
    static constexpr const char *TypeName() { return "font"; }
};

// Concept to ensure we only work with valid resource types
template<typename T>
concept RaylibResource = requires(const char *path, const T &resource) {
    { ResourceTraits<T>::Load(path) } -> std::same_as<T>;
    { ResourceTraits<T>::IsValid(resource) } -> std::same_as<bool>;
    { ResourceTraits<T>::TypeName() } -> std::same_as<const char *>;
};

class ResourceManager final {
public:
    ResourceManager() = default;
    ~ResourceManager();

    void LoadTextures(const std::string &path) { LoadResources<Texture2D>(path, ".png", m_texCache); }
    void LoadSounds(const std::string &path) { LoadResources<Sound>(path, ".ogg", m_sndCache); }
    void LoadFonts(const std::string &path) { LoadResources<Font>(path, ".ttf", m_fntCache); }
    void LoadMusic(const std::string &path) { LoadResources<Music>(path, ".ogg", m_musCache); };

    [[nodiscard]] std::optional<std::reference_wrapper<Texture2D>> GetTexture(const std::string &path);
    [[nodiscard]] std::optional<std::reference_wrapper<Sound>> GetSound(const std::string &path);
    [[nodiscard]] std::optional<std::reference_wrapper<Music>> GetMusic(const std::string &path);
    [[nodiscard]] std::optional<std::reference_wrapper<Font>> GetFont(const std::string &path);

private:
    std::map<std::string, Texture2D> m_texCache {};
    std::map<std::string, Sound> m_sndCache     {};
    std::map<std::string, Music> m_musCache     {};
    std::map<std::string, Font> m_fntCache      {};

    template<RaylibResource ResourceType> void LoadResources(const std::string &path, const std::string &extension, std::map<std::string, ResourceType> &cache);
};

}
