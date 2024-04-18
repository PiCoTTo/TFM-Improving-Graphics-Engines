#include "core/Entry.h"
#include <iostream>
#include "EditorLayer.h"


class MyApp : public nimo::Application
{
public:
    MyApp(const nimo::ApplicationDescription& desc, char* projectPath)
        : Application(desc)
    {
        NIMO_DEBUG("My app constructor");
        editorLayer = new EditorLayer();
        AddLayer(editorLayer);
        if (projectPath != nullptr)
            editorLayer->openProject(static_cast<nfdchar_t*>(projectPath));
    }

    ~MyApp()
    {
        NIMO_DEBUG("My app destructor");
    }
    EditorLayer* editorLayer;
};

nimo::Application* CreateApplication(int argc, char** argv)
{
    nimo::ApplicationDescription appDesc;
    appDesc.title = "Nimo Editor";
    char* projectPath = nullptr;
    if (argc > 1)
        projectPath = argv[1];
    return new MyApp(appDesc, projectPath);
}