#pragma once

#include "Event.h"

namespace nimo{
struct WindowResizeEvent: public Event
{
    WindowResizeEvent(int width, int height): width(width), height(height){};
    int width;
    int height;
};
struct WindowCloseEvent: public Event{};
struct WindowMinimizedEvent: public Event{};
};