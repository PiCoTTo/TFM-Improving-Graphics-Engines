#pragma once
#include <string>
namespace nimo{
    class Layer
	{
	public:
		Layer(const std::string& name = "Layer") : m_name(name){}
		virtual ~Layer(){}

		virtual void OnAttach() {}
		virtual void OnDetach() {}
		virtual void OnUpdate(float deltaTime) {}

		inline const std::string& GetName() const { return m_name; }
	protected:
		std::string m_name;
	};
};