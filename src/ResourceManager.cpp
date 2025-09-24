#include "../include/ResourceManager.h"

#include <algorithm>
#include <ranges>
#include <filesystem>
#include <iostream>

#include "Logger.h"

namespace SpaceInvaders {

ResourceManager::~ResourceManager() {
    using namespace std::ranges;

    for_each(m_texCache | views::values, [](const auto &tex) { ::UnloadTexture(tex); });
    for_each(m_sndCache | views::values, [](const auto &snd) { ::UnloadSound(snd); });
    for_each(m_musCache | views::values, [](const auto &mus) { ::UnloadMusicStream(mus); });
    for_each(m_fntCache | views::values, [](const auto &fnt) { ::UnloadFont(fnt); });
    for_each(m_shaderCache | views::values, [](const auto &shd) { ::UnloadShader(shd); });
}

template<RaylibResource ResourceType>
void
ResourceManager::LoadResources(const std::string &path, const std::string &extension,
                                   std::map<std::string, ResourceType> &cache) {
    namespace fs = std::filesystem;
    const fs::path dir(path);

    if (!fs::exists(dir)) {
        throw std::runtime_error("Resource directory does not exist: " + path);
    }

    for (const fs::directory_entry &entry : fs::recursive_directory_iterator(dir)) {
        if (!entry.is_regular_file())
            continue;

        const fs::path &p = entry.path();
        if (p.extension().string() != extension)
            continue;

        const auto resource = ResourceTraits<ResourceType>::Load(p.string().c_str());
        if (!ResourceTraits<ResourceType>::IsValid(resource)) {
            Logger::Log(LogLevel::Warning, std::format("Failed to load {}: {}",
                ResourceTraits<ResourceType>::TypeName(), p.filename().string()));
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
template void ResourceManager::LoadResources<Shader>(const std::string &path, const std::string &extension, std::map<std::string, Shader> &cache);

template<RaylibResource T>
std::optional<std::reference_wrapper<T>>
ResourceManager::Get(const std::string &path) {
    auto &cache = CacheSelector<T>::Get(*this);
    if (auto it = cache.find(path); it != cache.end()) {
        return std::ref(it->second);
    }

    return std::nullopt;
}

// Explicit template instantiations to ensure the template is compiled
template std::optional<std::reference_wrapper<Texture2D>> ResourceManager::Get<Texture2D>(const std::string &);
template std::optional<std::reference_wrapper<Sound>>     ResourceManager::Get<Sound>(const std::string &);
template std::optional<std::reference_wrapper<Music>>     ResourceManager::Get<Music>(const std::string &);
template std::optional<std::reference_wrapper<Font>>      ResourceManager::Get<Font>(const std::string &);
template std::optional<std::reference_wrapper<Shader>>    ResourceManager::Get<Shader>(const std::string &);

}
