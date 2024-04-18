#pragma once
#include "Subject.h"
#include <string>

class Preferences :
    public Subject<Preferences>
{
public:
    Preferences();

    bool limitFPS_AppInDev = true;
    bool limitFPS_Global = true;

    void load();
    void save();

private:
    std::string m_saveFilePath = "preferences.nimo";
};

