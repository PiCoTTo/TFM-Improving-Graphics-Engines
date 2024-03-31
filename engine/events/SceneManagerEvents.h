#pragma once

#include "Event.h"
#include "scene/Scene.h"

namespace nimo{
struct SceneLoadedEvent: public Event
{
    SceneLoadedEvent(std::shared_ptr<Scene> scene) : scene(scene){};
    std::shared_ptr<Scene> scene;
};
};