#pragma once

#include "Event.h"

namespace nimo{
struct KeyPressedEvent: public Event
{
    KeyPressedEvent(int key): key(key){};
    int key;
};
struct KeyReleasedEvent: public Event
{
    KeyReleasedEvent(int key): key(key){};
    int key;
};
};