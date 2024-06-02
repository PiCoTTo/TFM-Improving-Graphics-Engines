#pragma once
#include <memory>
#include <map>
#include <string>
#include <typeindex>
#include "glm/glm.hpp"


namespace nimo
{
	struct ExportedVariable
	{
		ExportedVariable(const std::string& name, const std::type_index& type, void* value) : m_name(name), m_type(type), m_value(value) {}

		std::string m_name;
		const std::type_index m_type;
		void* m_value;
	};

	class ExportedVariablesManager
	{
	public:
		static std::shared_ptr<ExportedVariablesManager>& instance();

		std::map<std::string, std::shared_ptr<ExportedVariable>> variables();
		bool addVariable(std::string name, std::shared_ptr<ExportedVariable>& variable);
		bool addVariable(std::string name, bool& variable);
		bool addVariable(std::string name, unsigned int& variable);
		bool addVariable(std::string name, glm::vec3& variable);

	private:
		static std::shared_ptr<ExportedVariablesManager> m_instance;

		std::map<std::string, std::shared_ptr<ExportedVariable>> m_variables;
	};
}