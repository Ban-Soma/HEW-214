#include "GameTime.h"
#include <algorithm>
GameTime::Clock::time_point GameTime::m_LastTime;
float GameTime::m_DeltaTime = 0.0f;
float GameTime::m_FPS = 0.0f;

namespace
{
	float fpsTimer = 0.0f;
	int frameCount = 0;
}
void GameTime::Init()
{
	m_LastTime = Clock::now();
	m_DeltaTime = 0.0f;
	m_FPS = 0.0f;
	fpsTimer = 0.0f;
	frameCount = 0;
}
void GameTime::Update()
{
	auto now = Clock::now();
	float rawDeltaTime = std::chrono::duration<float>(now - m_LastTime).count();
	
	m_LastTime = now;
	//FPSŒv‘ª
	fpsTimer += rawDeltaTime;
	frameCount++;
	if (fpsTimer >= 0.5f)
	{
		m_FPS = frameCount / fpsTimer;
	
		fpsTimer = 0.0f;
		frameCount = 0;
	}

	
	m_DeltaTime =std::max(0.0f,std::min(rawDeltaTime,0.1f));
}

float GameTime::DeltaTime()
{
	return m_DeltaTime;
}
float GameTime::FPS()
{
	return m_FPS;
}
