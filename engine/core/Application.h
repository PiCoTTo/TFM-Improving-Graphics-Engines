#pragma once
#include <vector>
#include "Window.h"
#include "Layer.h"
#include <memory>
#include "events/WindowEvents.h"

namespace nimo
{    
    struct ApplicationDescription{
        std::string title = "Nimo engine";
        unsigned int windowWidth = 1920;
        unsigned int windowHeight = 1080;
        bool windowDecorated = true;
        bool fullscreen = false;
        bool vsync = false;
        bool startMaximized = true;
		bool resizable = true;
    };
    class __declspec(dllexport) Application{
    public:
        Application(const ApplicationDescription& description);
        virtual ~Application();

        void Run();
        void Close();

        static inline Application& Instance(){ return *instance;}

        virtual void OnStart(){}
        virtual void OnUpdate(float deltaTime){}
        virtual void OnClose(){}

        float Time();
		inline Window& GetWindow() { return *m_window; }
        inline const ApplicationDescription& GetDescription() {return m_appDescription;}

        void AddLayer(Layer* layer);
        void RemoveLayer(Layer* layer);
    private:
        void OnWindowResize(const WindowResizeEvent& e);
        void OnWindowClose(const WindowCloseEvent& e);
        void OnWindowMinimize();
        std::unique_ptr<Window> m_window;
        std::vector<Layer*> m_layers;
        float m_frameTime;
        float m_lastFrameTime;
        bool m_appRunning;
        ApplicationDescription m_appDescription;

        static Application* instance;
    };
}