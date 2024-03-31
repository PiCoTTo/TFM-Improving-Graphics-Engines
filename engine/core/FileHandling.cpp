#include "FileHandling.h"

#include <fstream>
#include "Log.h"

bool nimo::FileHandling::Copy(const std::filesystem::path& from, const std::filesystem::path& to)
{
    NIMO_DEBUG("Copying {} to {}", from.string(), to.string());
    if (FileHandling::Exists(to))
        return false;
    std::filesystem::copy(from, to, 
        std::filesystem::copy_options::recursive |
        std::filesystem::copy_options::update_existing
    );
    return true;
}

bool nimo::FileHandling::Move(const std::filesystem::path& from, const std::filesystem::path& to)
{
    if (nimo::FileHandling::Exists(to))
        return false;

    std::filesystem::rename(from, to);
    return true;
}
bool nimo::FileHandling::Remove(const std::filesystem::path& target)
{
		if (!FileHandling::Exists(target))
			return false;
        if (std::filesystem::is_directory(target))
			return std::filesystem::remove_all(target) > 0;
		return std::filesystem::remove(target);
}

bool nimo::FileHandling::CreateDiretory(const std::filesystem::path& folderpath)
{
    NIMO_DEBUG("Creating directory {}", folderpath.string());
    return std::filesystem::create_directories(folderpath);
}

bool nimo::FileHandling::Exists(const std::filesystem::path& filepath)
{
    return std::filesystem::exists(filepath);
}
bool nimo::FileHandling::IsDirectory(const std::filesystem::path& filepath)
{
    return std::filesystem::is_directory(filepath);
}
std::vector<std::filesystem::path> nimo::FileHandling::GetFilesWithExtension(const std::filesystem::path& folderpath, const std::string& extension)
{
    std::vector<std::filesystem::path> res;
    for (auto &p : std::filesystem::directory_iterator(folderpath))
    {
        if (p.path().extension() == extension)
            res.push_back(p.path());
    }
    return res;
}
