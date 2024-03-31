#pragma once

#include "KeyCodes.h"
#include <utility>
#include <vector>
#include "events/KeyEvents.h"
#include "events/MouseEvents.h"

namespace nimo {
	class Input
	{
	public:
		static void Initialize();
		static void Cleanup();
		static void Update();
        //Mouse
		static bool GetMouseButton(MouseButton button);
		static bool GetMouseButtonPressed(MouseButton button);
		static bool GetMouseButtonReleased(MouseButton button);
		static float GetMouseX();
		static float GetMouseY();
		static std::pair<float, float> GetMousePosition();
		static std::pair<double, double> GetMouseScroll();
        //Key
		static bool GetKey(KeyCode keycode);
        static bool GetKeyPressed(KeyCode k);
        static bool GetKeyReleased(KeyCode k);	
        // Cursor mode
		static void SetCursorMode(CursorMode mode);
		static CursorMode GetCursorMode();
    private:
		static Input* m_instance;
        std::vector<MouseButton> m_pressedMouseButtons;
        std::vector<MouseButton> m_releasedMouseButtons;
        std::vector<KeyCode> m_pressedKeys;
        std::vector<KeyCode> m_releasedKeys;
		std::pair<double, double> m_mouseScroll = {0.0f, 0.0f};
		void OnMouseButtonPressed(const MouseButtonPressedEvent& e);
		void OnMouseButtonReleased(const MouseButtonReleasedEvent& e);
		void OnMouseScroll(const MouseScrollEvent& e);
		void OnKeyPressed(const KeyPressedEvent& e);
		void OnKeyReleased(const KeyReleasedEvent& e);
	};

}