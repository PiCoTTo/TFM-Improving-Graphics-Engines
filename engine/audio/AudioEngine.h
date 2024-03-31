#pragma once 
#include "miniaudio.h"
namespace nimo
{
class AudioEngine
{
public:
	static void Initialize();
	static void Cleanup();
    inline static ma_engine* GetEngine(){return m_engine;}
private:
    static ma_engine* m_engine;
};
} // namespace nimo
