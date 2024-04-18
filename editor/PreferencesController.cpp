#include "PreferencesController.h"


PreferencesController::PreferencesController(Preferences& preferences) :
	m_preferences(preferences)
{
	m_currentLimitFPS_Global = preferences.limitFPS_Global;
	m_currentLimitFPS_AppInDev = preferences.limitFPS_AppInDev;
}

void PreferencesController::setLimitFPS(bool global, bool appInDev)
{
	if (m_currentLimitFPS_Global != global || m_currentLimitFPS_AppInDev != appInDev)
	{
		m_preferences.limitFPS_Global = global;
		m_preferences.limitFPS_AppInDev = appInDev;

		m_currentLimitFPS_Global = global;
		m_currentLimitFPS_AppInDev = appInDev;

		savePreferencesToDisk();
	}
}

void PreferencesController::savePreferencesToDisk()
{
	m_preferences.save();
}
