#pragma once

#include "Event.h"
#include "project/Project.h"

namespace nimo{
struct ActiveProjectChanged: public Event
{
    ActiveProjectChanged(){};
};
};