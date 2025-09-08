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

template<RaylibResource ResourceType>
void
ResourceManager::LoadResources(const std::string &path, const std::string &extension,
                                   std::map<std::string, ResourceType> &cache) {
    const fs::path dir(path);

    if (!fs::exists(dir)) {
        std::println(std::cerr, "WARNING: Resource directory does not exist: {}", path);
        return;
    }

    for (const fs::directory_entry &entry : fs::recursive_directory_iterator(dir)) {
        if (!entry.is_regular_file())
            continue;

        const fs::path &p = entry.path();
        if (p.extension().string() != extension)
            continue;

        const auto resource = ResourceTraits<ResourceType>::Load(p.string().c_str());
        if (!ResourceTraits<ResourceType>::IsValid(resource)) {
            std::println(std::cerr, "WARNING: Failed to load {}: {}",
                        ResourceTraits<ResourceType>::TypeName(),
                        p.filename().string());
            continue;
        }

        cache[p.filename().string()] = resource;
    }
}

// Explicit template instantiations to ensure the template is compiled
template void ResourceManager::LoadResources<Texture2D>(const std::string &path, const std::string &extension, std::map<std::string, Texture2D> &cache);
template void ResourceManager::LoadResources<Sound>(const std::string &path, const std::string &extension, std::map<std::string, Sound> &cache);
template void ResourceManager::LoadResources<Music>(const std::string &path, const std::string &extension, std::map<std::string, Music> &cache);
template void ResourceManager::LoadResources<Font>(const std::string &path, const std::string &extension, std::map<std::string, Font> &cache);

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
