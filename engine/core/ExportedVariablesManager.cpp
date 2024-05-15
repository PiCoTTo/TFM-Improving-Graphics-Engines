#include "ExportedVariablesManager.h"

std::shared_ptr<ExportedVariablesManager>& ExportedVariablesManager::instance()
{
	if (m_instance.get() == nullptr)
		m_instance = std::make_shared<ExportedVariablesManager>();

	return m_instance;
}

std::map<std::string, std::shared_ptr<ExportedVariable>> ExportedVariablesManager::variables()
{
	return m_variables;
}

bool ExportedVariablesManager::addVariable(std::string name, std::shared_ptr<ExportedVariable>& variable)
{
	if (m_variables.find(name) == m_variables.end())
		m_variables[name] = variable;

	return false;
}

bool ExportedVariablesManager::addVariable(std::string name, bool& variable)
{
	if (m_variables.find(name) == m_variables.end())
		m_variables[name] = std::make_shared<ExportedVariable>(name, typeid(bool), &variable);

	return false;
}

bool ExportedVariablesManager::addVariable(std::string name, unsigned int& variable)
{
	if (m_variables.find(name) == m_variables.end())
		m_variables[name] = std::make_shared<ExportedVariable>(name, typeid(unsigned int), &variable);

	return false;
}

std::shared_ptr<ExportedVariablesManager> ExportedVariablesManager::m_instance{ nullptr };