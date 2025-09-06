#include "../include/ResourceManager.h"

#include <algorithm>
#include <ranges>
#include <filesystem>
#include <iostream>

namespace SpaceInvaders {

namespace fs = std::filesystem;

ResourceManager::~ResourceManager() {
    using namespace std::ranges;

    for_each(m_texCache | views::values, [](const auto &tex) { ::UnloadTexture(tex); });
    for_each(m_sndCache | views::values, [](const auto &snd) { ::UnloadSound(snd); });
    for_each(m_musCache | views::values, [](const auto &mus) { ::UnloadMusicStream(mus); });
    for_each(m_fntCache | views::values, [](const auto &fnt) { ::UnloadFont(fnt); });
}

void
ResourceManager::LoadTextures(const std::string &path) {
    const fs::path dir(path);
    for (const fs::directory_entry &entry : fs::recursive_directory_iterator(dir)) {
        if (!entry.is_regular_file())
            continue;

        const fs::path &p = entry.path();
        if (p.extension().string() != ".png")
            continue;

        const auto tex = LoadTexture(p.string().c_str());
        if (!IsTextureValid(tex)) {
#if defined(WIN32)
            std::cerr << "WARNING: Failed to load texture: " << p.filename().string() << std::endl;
#else
            std::println(std::cerr, "WARNING: Failed to load texture: {}", p.filename().string());
#endif
            continue;
        }
        m_texCache[p.filename().string()] = tex;
    }
}

void
ResourceManager::LoadSounds(const std::string &path) {
    const fs::path dir(path);
    for (const fs::directory_entry &entry : fs::recursive_directory_iterator(dir)) {
        if (!entry.is_regular_file())
            continue;

        const fs::path &p = entry.path();
        if (p.extension().string() != ".ogg")
            continue;

        const auto snd = LoadSound(p.string().c_str());
        if (!IsSoundValid(snd)) {
#if defined(WIN32)
            std::cerr << "WARNING: Failed to load sound: " << p.filename().string() << std::endl;
#else
            std::println(std::cerr, "WARNING: Failed to load sound: {}", p.filename().string());
#endif
            continue;
        }
        m_sndCache[p.filename().string()] = snd;
    }
}

void
ResourceManager::LoadMusic(const std::string &path) {
    const fs::path dir(path);
    for (const fs::directory_entry &entry : fs::recursive_directory_iterator(dir)) {
        if (!entry.is_regular_file())
            continue;

        const fs::path &p = entry.path();
        if (p.extension().string() != ".ogg")
            continue;

        const auto mus = LoadMusicStream(p.string().c_str());
        if (!IsMusicValid(mus)) {
#if defined(WIN32)
            std::cerr << "WARNING: Failed to load music: " << p.filename().string() << std::endl;
#else
            std::println(std::cerr, "WARNING: Failed to load music: {}", p.filename().string());
#endif
            continue;
        }
        m_musCache[p.filename().string()] = mus;
    }
}

void
ResourceManager::LoadFonts(const std::string &path) {
    const fs::path dir(path);
    for (const fs::directory_entry &entry : fs::recursive_directory_iterator(dir)) {
        if (!entry.is_regular_file())
            continue;

        const fs::path &p = entry.path();
        if (p.extension().string() != ".ttf")
            continue;

        const auto fnt = LoadFont(p.string().c_str());
        if (!IsFontValid(fnt)) {
#if defined(WIN32)
            std::cerr << "WARNING: Failed to load font: " << p.filename().string() << std::endl;
#else
            std::println(std::cerr, "WARNING: Failed to load font: {}", p.filename().string());
#endif
            continue;
        }
        m_fntCache[p.filename().string()] = fnt;
    }
}

std::optional<std::reference_wrapper<Texture2D>>
ResourceManager::GetTexture(const std::string &path) {
    if (m_texCache.contains(path)) {
        return m_texCache[path];
    }
    return std::nullopt;
}

std::optional<std::reference_wrapper<Sound>>
ResourceManager::GetSound(const std::string &path) {
    if (m_sndCache.contains(path)) {
        return m_sndCache[path];
    }
    return std::nullopt;
}

std::optional<std::reference_wrapper<Music>>
ResourceManager::GetMusic(const std::string &path) {
    if (m_musCache.contains(path)) {
        return m_musCache[path];
    }
    return std::nullopt;
}

std::optional<std::reference_wrapper<Font>>
ResourceManager::GetFont(const std::string &path) {
    if (m_fntCache.contains(path)) {
        return m_fntCache[path];
    }
    return std::nullopt;
}

}
