#include "Application.h"

#include "Log.h"
#include "glfw/glfw3.h"
#include "events/EventManager.h"
#include "input/Input.h"

nimo::Application* nimo::Application::instance = nullptr;

nimo::Application::Application(const ApplicationDescription& description)
    : m_appRunning(true)
    , m_appDescription(description)
{
    instance = this;
    NIMO_INFO("Created nimo::Application");
    EventManager::Subscribe(this, &Application::OnWindowClose);
    EventManager::Subscribe(this, &Application::OnWindowResize);
    WindowDescription windowDesc;
    windowDesc.title = description.title;
    windowDesc.width = description.windowWidth;
    windowDesc.height = description.windowHeight;
    windowDesc.fullscreen = description.fullscreen;
    windowDesc.decorated = description.windowHeight;
    windowDesc.vsync = description.vsync;
    m_window =  std::make_unique<Window>(windowDesc);

    if(description.startMaximized)
        m_window->Maximize();
}
void nimo::Application::Run()
{
    NIMO_INFO("nimo::App run");
    OnStart();
    while(m_appRunning)
    {
        Input::Update();
        m_window->ProcessEvents();
        OnUpdate(m_frameTime);
        for(auto l : m_layers)
            l->OnUpdate(m_frameTime);
        m_window->SwapBuffers();
        float time = Time();
        m_frameTime = time - m_lastFrameTime;
        m_lastFrameTime = time;
    }
    OnClose();
}
void nimo::Application::Close()
{
    m_appRunning = false;
    NIMO_INFO("nimo::Application::Close");
}
nimo::Application::~Application()
{
    EventManager::Unsubscribe(this, &Application::OnWindowClose);
    EventManager::Unsubscribe(this, &Application::OnWindowResize);
    for(auto layer : m_layers)
        delete layer;
    NIMO_INFO("Destroyed nimo::Application");    
}

void nimo::Application::AddLayer(Layer* layer)
{
    m_layers.push_back(layer);
    layer->OnAttach();
}
void nimo::Application::RemoveLayer(Layer* layer)
{
    auto it = std::find(m_layers.begin(), m_layers.end(), layer);
    if (it != m_layers.end())
    {
        m_layers.erase(it);
    }
    layer->OnDetach();
}

float nimo::Application::Time()
{
    return (float)glfwGetTime();
}

void nimo::Application::OnWindowResize(const WindowResizeEvent& e)
{
    NIMO_INFO("Window resized to {},{}", e.width, e.height);
}
void nimo::Application::OnWindowClose(const WindowCloseEvent& e)
{
    NIMO_INFO("Window closed");
    Close();
}
