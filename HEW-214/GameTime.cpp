#include "GameTime.h"
#include <algorithm>
GameTime::Clock::time_point GameTime::m_LastTime;
float GameTime::m_DeltaTime = 0.0f;
float GameTime::m_FPS = 0.0f;
float GameTime::m_TimeScale = 1.0f;
float GameTime::m_UnscaledDeltaTime = 0.0f;

float GameTime::m_HitStopRemaining = 0.0f;
float GameTime::m_SlowRemaining = 0.0f;
float GameTime::m_SlowDuration = 0.10f;
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
	m_HitStopRemaining = 0;
	m_TimeScale = 1.0f;
	m_UnscaledDeltaTime = 0.0f;
	m_SlowDuration = 0.10f;
	m_SlowRemaining = 0.0f;
}
void GameTime::Update()
{
	auto now = Clock::now();
	float realDt = std::chrono::duration<float>(now - m_LastTime).count();
	
	m_LastTime = now;
	m_UnscaledDeltaTime = std::max(0.0f, realDt);

	if (m_HitStopRemaining > 0.0f)
	{
		m_HitStopRemaining -= realDt;
		m_TimeScale = 0.0f;
	}
	else if (m_SlowRemaining > 0.0f)
	{
		m_SlowRemaining -= realDt;
		float progress = 1.0f - std::max(
			0.0f, m_SlowRemaining) / m_SlowDuration;
		
		m_TimeScale = 0.25f + 0.75f * progress;
	}
	else
	{
		m_TimeScale = 1.0f;
	}
	const float clampedDt = std::min(m_UnscaledDeltaTime, 0.1f);
	m_DeltaTime = clampedDt * m_TimeScale;
	//FPSŒv‘ª
	fpsTimer += realDt;
	frameCount++;
	if (fpsTimer >= 0.5f)
	{
		m_FPS = frameCount / fpsTimer;
	
		fpsTimer = 0.0f;
		frameCount = 0;
	}
}

float GameTime::DeltaTime()
{
	return m_DeltaTime;
}
float GameTime::UnScaleDeltaTime()
{
	return m_UnscaledDeltaTime;
}
float GameTime::GetTimeScale()
{
	return m_TimeScale;
}
void GameTime::Killslow()
{
	m_HitStopRemaining = 0.04f;
	m_SlowRemaining = 0.10f;
}
float GameTime::FPS()
{
	return m_FPS;
}
