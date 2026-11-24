
#include "Profiler.h"

#include <unordered_map>
#include <algorithm>

namespace
{
    struct ProfileEntry
    {
        float frameMs = 0.0f;
        int frameCalls = 0;

        float totalMs = 0.0f;
        float maxMs = 0.0f;
        int totalCalls = 0;
    };

    std::unordered_map<std::string, ProfileEntry> entries;
    std::vector<ProfileStat> visibleStats;

    Profiler::Clock::time_point intervalStart;
    int frameCount = 0;
    bool initialized = false;
}

void Profiler::BeginFrame()
{
    if (!initialized)
    {
        intervalStart = Clock::now();
        initialized = true;
    }

    for (auto& pair : entries)
    {
        pair.second.frameMs = 0.0f;
        pair.second.frameCalls = 0;
    }
}

void Profiler::AddSample(
    const char* name,
    float elapsedMs)
{
    ProfileEntry& entry = entries[name];

    entry.frameMs += elapsedMs;
    entry.frameCalls++;
}

void Profiler::EndFrame()
{
    frameCount++;

    for (auto& pair : entries)
    {
        ProfileEntry& entry = pair.second;

        entry.totalMs += entry.frameMs;
        entry.totalCalls += entry.frameCalls;

        entry.maxMs =
            std::max(entry.maxMs, entry.frameMs);
    }

    auto now = Clock::now();

    float elapsed =
        std::chrono::duration<float>(
            now - intervalStart
        ).count();

    if (elapsed < 0.5f)
        return;

    visibleStats.clear();

    for (auto& pair : entries)
    {
        ProfileEntry& entry = pair.second;

        ProfileStat stat;
        stat.name = pair.first;

        stat.averageMs =
            entry.totalMs / frameCount;

        stat.maxMs = entry.maxMs;

        stat.callsPerFrame =
            static_cast<float>(entry.totalCalls)
            / frameCount;

        visibleStats.push_back(stat);

        entry.totalMs = 0.0f;
        entry.maxMs = 0.0f;
        entry.totalCalls = 0;
    }

    // d‚¢ˆ—‚ðã‚É•\Ž¦
    std::sort(
        visibleStats.begin(),
        visibleStats.end(),
        [](const ProfileStat& a, const ProfileStat& b)
        {
            return a.averageMs > b.averageMs;
        }
    );

    frameCount = 0;
    intervalStart = now;
}

const std::vector<ProfileStat>& Profiler::GetStats()
{
    return visibleStats;
}

