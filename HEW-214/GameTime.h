#pragma once

#include <chrono>
#include <array>

class GameTime
{

public:
	using Clock = std::chrono::steady_clock;

	static void Init();
	static void Update();
	
	static float DeltaTime();
	static float FPS();
	


private:
	static Clock::time_point m_LastTime;
	
	static float m_DeltaTime;
	static float m_FPS;
};