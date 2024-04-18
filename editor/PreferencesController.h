#pragma once
#include "Preferences.h"


class PreferencesController
{
public:
	PreferencesController(Preferences& preferences);

	void setLimitFPS(bool global, bool scenes);
	void savePreferencesToDisk();

private:
	bool m_currentLimitFPS_Global = 0;
	bool m_currentLimitFPS_AppInDev = 0;

	Preferences& m_preferences;
};