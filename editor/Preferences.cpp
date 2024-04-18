#include "Preferences.h"
#include "json.hpp"
#include <fstream>
#include "core/Log.h"

Preferences::Preferences()
{
	load();
}

void Preferences::load()
{
	std::ifstream ifs(m_saveFilePath);
	if (ifs.good())
	{
		nlohmann::ordered_json j;
		j << ifs;
		try
		{
			limitFPS_AppInDev = j["LimitFPS"];
			limitFPS_Global = j["LimitGlobalFPS"];
		}
		catch (const std::exception& e)
		{
			NIMO_ERROR("Error loading Nimo preferences: {}", e.what());
		}
	}

	notify();
}

void Preferences::save()
{
	nlohmann::ordered_json j;
	j["LimitFPS"] = limitFPS_AppInDev;
	j["LimitGlobalFPS"] = limitFPS_Global;

	std::ofstream ofs(m_saveFilePath);
	ofs << j;
	ofs.close();

	notify();
}
