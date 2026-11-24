#pragma once

#include <chrono>
#include <string>
#include <vector>

struct ProfileStat
{
    std::string name;
    float averageMs = 0.0f;
    float maxMs = 0.0f;
    float callsPerFrame = 0.0f;
};

class Profiler
{
public:
    using Clock = std::chrono::steady_clock;
    static void BeginFrame();
    static void EndFrame();
    static void AddSample(const char* name,float elapsedMs);

    static const std::vector<ProfileStat>& GetStats();
};

class ProfileScope
{
public:
    explicit ProfileScope(const char* name)
        : m_Name(name),
        m_Start(Profiler::Clock::now())
    {
    }

    ~ProfileScope()
    {
        auto end = Profiler::Clock::now();

        float ms =
            std::chrono::duration<float, std::milli>(
                end - m_Start
            ).count();

        Profiler::AddSample(m_Name, ms);
    }

private:
    const char* m_Name;
    Profiler::Clock::time_point m_Start;
};

#define PROFILE_JOIN_IMPL(a, b) a##b
#define PROFILE_JOIN(a, b) PROFILE_JOIN_IMPL(a, b)

#define PROFILE_SCOPE(name) \
    ProfileScope PROFILE_JOIN(_profileScope_, __LINE__)(name)
