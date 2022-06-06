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
            auto& name = m_Header.functionNames()[beginEvent.functionIndex()];
            selfNanos = (double)m_CallStats[beginIndex] * m_Header.nanosecondsInTick();
            totalMs = (double)m_GlobalStats[name].TotalTicks * m_Header.nanosecondsInTick() / 1'000'000;
            totalPercent = (double)m_GlobalStats[name].TotalTicks / (double)m_SessionTicks * 100;
            maxMs = (double)m_GlobalStats[name].MaxTicks * m_Header.nanosecondsInTick() / 1'000'000;
            count = m_GlobalStats[name].Count;
        }

        [[nodiscard]] inline const SessionHeader& header() const
        {
            return m_Header;
        }

        [[nodiscard]] inline const std::vector<SessionEvent>& events() const
        {
            return m_Events;
        }

        [[nodiscard]] inline std::vector<SessionEvent>& events()
        {
            return m_Events;
        }

        inline void setEvents(std::vector<SessionEvent>& events)
        {
            m_Events = events;
            sortEvents();
            calculateStats();
        }

        void sortEvents();
        void calculateStats();

        static ProfilingSession getFakeProfilingSession();

        static std::string toString(const ProfilingSession& ps);
    };

    template<typename T>
    inline void print(T t)
    {
        std::cout << t << std::endl;
    }

    template<typename T, typename... Args>
    inline void print(T t, Args... args)
    {
        std::cout << t << ' ';
        print(args...);
    }
} // namespace UN
