#pragma once
#include "SessionEvent.h"
#include "SessionHeader.h"
#include <iostream>
#include <sstream>
#include <unordered_map>

namespace UN
{
    struct FunctionGlobalStats
    {
        uint64_t Count;
        uint64_t TotalTicks;
        uint64_t MaxTicks;
    };

    class ProfilingSession
    {
        SessionHeader m_Header;
        std::vector<SessionEvent> m_Events;

        uint32_t m_MaxHeight;
        uint64_t m_SessionTicks;
        std::vector<uint64_t> m_CallStats;
        std::unordered_map<std::string, FunctionGlobalStats> m_GlobalStats;

    public:
        ProfilingSession() = default;
        ProfilingSession(SessionHeader header, std::vector<SessionEvent> events);

        inline void functionStats(size_t beginIndex, double& selfNanos, double& totalMs, double& totalPercent, double& maxMs,
                                  uint64_t& count)
        {
            auto& beginEvent = m_Events[beginIndex];
            auto& name = m_Header.FunctionNames()[beginEvent.FunctionIndex()];
            selfNanos = (double)m_CallStats[beginIndex] * m_Header.NanosecondsInTick();
            totalMs = (double)m_GlobalStats[name].TotalTicks * m_Header.NanosecondsInTick() / 1'000'000;
            totalPercent = (double)m_GlobalStats[name].TotalTicks / (double)m_SessionTicks * 100;
            maxMs = (double)m_GlobalStats[name].MaxTicks * m_Header.NanosecondsInTick() / 1'000'000;
            count = m_GlobalStats[name].Count;
        }

        [[nodiscard]] inline const SessionHeader& Header() const
        {
            return m_Header;
        }

        [[nodiscard]] inline const std::vector<SessionEvent>& Events() const
        {
            return m_Events;
        }

        [[nodiscard]] inline std::vector<SessionEvent>& Events()
        {
            return m_Events;
        }

        inline void setEvents(std::vector<SessionEvent>& events)
        {
            m_Events = events;
            SortEvents();
            CalculateStats();
        }

        void SortEvents();
        void CalculateStats();

        static ProfilingSession GetFakeProfilingSession();

        static std::string ToString(const ProfilingSession& ps);
    };

    template<typename T>
    inline void Print(T t)
    {
        std::cout << t << std::endl;
    }

    template<typename T, typename... Args>
    inline void Print(T t, Args... args)
    {
        std::cout << t << ' ';
        Print(args...);
    }
} // namespace UN
