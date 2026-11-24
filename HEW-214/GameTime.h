#pragma once

#include <chrono>
#include <array>

class GameTime
{

public:
	using Clock = std::chrono::steady_clock;
public:

	static void Init();
	static void Update();
	
	static float FPS();

	static float DeltaTime();
	static float UnScaleDeltaTime();
	
	static float GetTimeScale();
	
	static void Killslow();
	


private:
	static Clock::time_point m_LastTime;
	
	static float m_DeltaTime;
	static float m_FPS;

	static float m_TimeScale;
	static float m_UnscaledDeltaTime;

	static float m_HitStopRemaining;
	static float m_SlowRemaining;
	static float m_SlowDuration;
};