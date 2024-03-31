#pragma once
#include <filesystem>

namespace nimo
{
    class FileHandling{
    public:
        static bool Copy(const std::filesystem::path& from, const std::filesystem::path& to);
        static bool Move(const std::filesystem::path& from, const std::filesystem::path& to);
        static bool Remove(const std::filesystem::path& target);
        static bool CreateDiretory(const std::filesystem::path& folderpath);
        static bool Exists(const std::filesystem::path& filepath);
        static bool IsDirectory(const std::filesystem::path& filepath);
        static std::vector<std::filesystem::path> GetFilesWithExtension(const std::filesystem::path& folderpath, const std::string& extension);
    };
} // namespace nimo
