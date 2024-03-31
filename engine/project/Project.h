#pragma once 
#include <string>
#include <memory>
#include <filesystem>

namespace nimo
{
    class ProjectSerializer;
    struct ProjectSettings
    {
        std::string name;
        std::string assetDirectory;
        std::string assetIndexPath;
        std::string logsDirectory;
        std::string modulesDirectory;
        std::string startScene;

        std::filesystem::path projectDirectory;
    };

    class Project
    {
    public:
        Project(const ProjectSettings& settings);
        ~Project();
        static void SetActiveProject(std::shared_ptr<Project> p);
        inline static std::shared_ptr<Project> GetActiveProject(){ return m_activeProject;}
        inline ProjectSettings& GetSettings(){return m_settings;}
        inline static const std::filesystem::path GetAssetsFolderPath(){return GetActiveProject()->GetSettings().projectDirectory/GetActiveProject()->GetSettings().assetDirectory;}
        inline static const std::filesystem::path GetLogsFolderPath(){return GetActiveProject()->GetSettings().projectDirectory/GetActiveProject()->GetSettings().logsDirectory;}
        inline static const std::filesystem::path GetModulesFolderPath(){return GetActiveProject()->GetSettings().projectDirectory/GetActiveProject()->GetSettings().modulesDirectory;}
        inline static const std::filesystem::path GetAssetIndexPath(){return GetActiveProject()->GetSettings().projectDirectory/GetActiveProject()->GetSettings().assetIndexPath;}
    private:
        ProjectSettings m_settings;
        inline static std::shared_ptr<Project> m_activeProject;

        friend class ProjectSerializer;
    };
};