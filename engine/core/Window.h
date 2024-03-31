#pragma once
#include <string>
#include <memory>

namespace nimo{
    struct WindowDescription{
        std::string title = "Nimo engine";
        unsigned int width = 1920;
        unsigned int height = 1080;
        bool decorated = true;
        bool fullscreen = false;
        bool vsync = true;
    };
    class Window{
    public:
        Window(const WindowDescription& description = WindowDescription());
        ~Window();

        bool ShouldClose();
        void ProcessEvents();
        void SwapBuffers();

        unsigned int GetWidth() const;
		unsigned int GetHeight() const;
		std::pair<unsigned int, unsigned int> GetSize() const;
		std::pair<float, float> GetWindowPos() const;

        void* GetNativeHandle() const;
 
        void VSync(bool enabled);
		bool VSync() const;
		void SetResizable(bool resizable) const;
		void Maximize();
		void Restore();
		void CenterWindow();
		const std::string& GetTitle() const;
		void SetTitle(const std::string& title);

    private:
        struct impl; std::unique_ptr<impl> m_pimpl;
    };
}