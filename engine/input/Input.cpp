#include "Input.h"
#include "glfw/glfw3.h"
#include "core/Window.h"
#include "core/Application.h"
#include "events/EventManager.h"

nimo::Input* nimo::Input::m_instance = nullptr;

void nimo::Input::Initialize()
{
    if(!m_instance)
        m_instance = new Input();
    m_instance->m_pressedMouseButtons.reserve(3);
    m_instance->m_releasedMouseButtons.reserve(3);
    m_instance->m_pressedKeys.reserve(10);
    m_instance->m_releasedKeys.reserve(10);
    EventManager::Subscribe(m_instance, &OnKeyPressed);
    EventManager::Subscribe(m_instance, &OnKeyReleased);
    EventManager::Subscribe(m_instance, &OnMouseButtonPressed);
    EventManager::Subscribe(m_instance, &OnMouseButtonReleased);
    EventManager::Subscribe(m_instance, &OnMouseScroll);
}
void nimo::Input::Cleanup()
{
    EventManager::Unsubscribe(m_instance, &OnKeyPressed);
    EventManager::Unsubscribe(m_instance, &OnKeyReleased);
    EventManager::Unsubscribe(m_instance, &OnMouseButtonPressed);
    EventManager::Unsubscribe(m_instance, &OnMouseButtonReleased);
    EventManager::Unsubscribe(m_instance, &OnMouseScroll);
    delete m_instance;
}
void nimo::Input::Update()
{
    m_instance->m_pressedMouseButtons.clear();
    m_instance->m_releasedMouseButtons.clear();
    m_instance->m_pressedKeys.clear();
    m_instance->m_releasedKeys.clear();
    m_instance->m_mouseScroll = {0.0, 0.0};
}
bool nimo::Input::GetMouseButton(MouseButton button)
{
    Window& window = static_cast<Window&>(Application::Instance().GetWindow());
    int state = glfwGetMouseButton(static_cast<GLFWwindow*>(window.GetNativeHandle()), static_cast<int32_t>(button));
    return state == GLFW_PRESS || state == GLFW_REPEAT;
}
bool nimo::Input::GetMouseButtonPressed(MouseButton button)
{
    return std::find(m_instance->m_pressedMouseButtons.begin(), m_instance->m_pressedMouseButtons.end(), button) != m_instance->m_pressedMouseButtons.end();
}
bool nimo::Input::GetMouseButtonReleased(MouseButton button)
{
    return std::find(m_instance->m_releasedMouseButtons.begin(), m_instance->m_releasedMouseButtons.end(), button) != m_instance->m_releasedMouseButtons.end();
}
float nimo::Input::GetMouseX()
{
    return GetMousePosition().first;
}
float nimo::Input::GetMouseY()
{
    return GetMousePosition().second;
}
std::pair<float, float> nimo::Input::GetMousePosition()
{

    Window& window = static_cast<Window&>(Application::Instance().GetWindow());
    double x, y;
    glfwGetCursorPos(static_cast<GLFWwindow*>(window.GetNativeHandle()), &x, &y);
    return { (float)x, (float)y };
}
std::pair<double, double> nimo::Input::GetMouseScroll()
{
    return m_instance->m_mouseScroll;
}
bool nimo::Input::GetKey(KeyCode keycode)
{
    Window& window = static_cast<Window&>(Application::Instance().GetWindow());
    int state = glfwGetKey(static_cast<GLFWwindow*>(window.GetNativeHandle()), static_cast<int32_t>(keycode));
    return state == GLFW_PRESS || state == GLFW_REPEAT;
}
bool nimo::Input::GetKeyPressed(KeyCode k)
{
    return std::find(m_instance->m_pressedKeys.begin(), m_instance->m_pressedKeys.end(), k) != m_instance->m_pressedKeys.end();
}
bool nimo::Input::GetKeyReleased(KeyCode k)
{
    return std::find(m_instance->m_releasedKeys.begin(), m_instance->m_releasedKeys.end(), k) != m_instance->m_releasedKeys.end();
}
void nimo::Input::SetCursorMode(CursorMode mode)
{
    Window& window = static_cast<Window&>(Application::Instance().GetWindow());
    glfwSetInputMode(static_cast<GLFWwindow*>(window.GetNativeHandle()), GLFW_CURSOR, GLFW_CURSOR_NORMAL + (int)mode);
}
nimo::CursorMode nimo::Input::GetCursorMode()
{
    Window& window = static_cast<Window&>(Application::Instance().GetWindow());
    return (CursorMode)(glfwGetInputMode(static_cast<GLFWwindow*>(window.GetNativeHandle()), GLFW_CURSOR) - GLFW_CURSOR_NORMAL);
}

void nimo::Input::OnMouseButtonPressed(const MouseButtonPressedEvent& e)
{
    m_pressedMouseButtons.push_back((MouseButton)e.button);
}
void nimo::Input::OnMouseButtonReleased(const MouseButtonReleasedEvent& e)
{
    m_releasedMouseButtons.push_back((MouseButton)e.button);
}
void nimo::Input::OnMouseScroll(const MouseScrollEvent& e)
{
    m_mouseScroll = {e.xOffset, e.yOffset};
}
void nimo::Input::OnKeyPressed(const KeyPressedEvent& e)
{
    m_pressedKeys.push_back((KeyCode)e.key);
}
void nimo::Input::OnKeyReleased(const KeyReleasedEvent& e)
{
    m_releasedKeys.push_back((KeyCode)e.key);
}
