#pragma once
#include "Project.h"
#include <memory>
#include <filesystem>

namespace nimo {

	class ProjectSerializer
	{
	public:
		ProjectSerializer(const std::shared_ptr<Project>& project);
		void Serialize(const std::filesystem::path& filepath);
		bool Deserialize(const std::filesystem::path& filepath);

	private:
		std::shared_ptr<Project> m_project;
	};

}