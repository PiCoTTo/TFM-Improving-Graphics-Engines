#pragma once

#include "Event.h"

namespace nimo{
struct MouseButtonPressedEvent: public Event
{
    MouseButtonPressedEvent(int button): button(button){};
    int button;
};
struct MouseButtonReleasedEvent: public Event
{
    MouseButtonReleasedEvent(int button): button(button){};
    int button;
};
struct MousePositionEvent: public Event
{
    MousePositionEvent(double xPos, double yPos):xPos(xPos), yPos(yPos){};
    double xPos;
    double yPos;
};
struct MouseScrollEvent: public Event
{
    MouseScrollEvent(double xOffset, double yOffset): xOffset(xOffset), yOffset(yOffset) {};
    double xOffset;
    double yOffset;
};
};