#include "core/Entry.h"
#include "core/Log.h"
#include "core/FileHandling.h"
#include "RuntimeLayer.h"

class RuntimeApp : public nimo::Application
{
public:
    RuntimeApp(const nimo::ApplicationDescription& desc, const std::filesystem::path& projectFilepath)
        : Application(desc)
    {
        layer = new RuntimeLayer(projectFilepath);
        AddLayer(layer);
    }

    ~RuntimeApp()
    {
        delete layer;
    }
    RuntimeLayer* layer;
};

nimo::Application* CreateApplication(int argc, char** argv)
{
    nimo::ApplicationDescription appDesc;
    appDesc.startMaximized = false;
    
    // Search project file at location
    auto projs = nimo::FileHandling::GetFilesWithExtension(".", ".nproj");
    if(projs.size() == 0)
        return nullptr;
    // Set title from project filename
    appDesc.title = projs[0].stem().string();
    // Create runtime layer with project file 
        // Create project, scene, scene renderer
    return new RuntimeApp(appDesc, projs[0]);
}