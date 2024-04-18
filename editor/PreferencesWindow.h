#pragma once
#include "Observer.h"
#include "PreferencesController.h"
#include "Preferences.h"
//#include <memory>

class PreferencesWindow :
    public Observer<Preferences>
{
public:
    PreferencesWindow(std::shared_ptr<PreferencesController>& controller);

    void show(bool& show);
    void update(Preferences* subject) override;

private:
    std::shared_ptr<PreferencesController>& m_controller;

    bool m_limitFPS_3D      = false;
    bool m_limitFPS_Global  = false;
    bool m_anyChange        = false;
};

