#include "Entry.h"
#include "core/Log.h"
#include "scripting/ScriptManager.h"
#include "assets/AssetManager.h"
#include "scene/SceneManager.h"
#include "audio/AudioEngine.h"
#include "input/Input.h"
#include "renderer/Renderer.h"

int main(int argc, char** argv)
{
    nimo::Log::Initialize();
    nimo::Input::Initialize();
	nimo::AudioEngine::Initialize();
	auto app = CreateApplication(argc, argv);
	nimo::Renderer::Initialize();
	if(app == nullptr) 
	{
		NIMO_CRITICAL("Client application not created!");
		return 1;
	}
	app->Run();
	delete app;
	nimo::SceneManager::SetActiveScene({});
	nimo::AssetManager::UnloadUnused();
	nimo::AssetManager::Cleanup();
    nimo::ScriptManager::Cleanup();
	nimo::AudioEngine::Cleanup();
    nimo::Input::Cleanup();
    nimo::Log::Cleanup();
    return 0;
}