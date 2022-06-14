#pragma once
#include "SessionEvent.h"
#include "SessionHeader.h"
#include <iostream>
#include <sstream>
#include <unordered_map>

namespace UN
{
    class FuncsTreeNode
    {
    private:
        uint32_t m_FuncIdx; //index in header().functionNames()
        std::vector<FuncsTreeNode> m_Descendants;

    public:
        FuncsTreeNode(uint32_t funcIdx)
            : m_FuncIdx(funcIdx) {};

        [[nodiscard]] const uint32_t funcIdx() const
        {
            return m_FuncIdx;
        }

        [[nodiscard]] const std::vector<FuncsTreeNode>& descendants() const
        {
            return m_Descendants;
        }

        [[nodiscard]] std::vector<FuncsTreeNode>& descendants()
        {
            return m_Descendants;
        }
    };

    struct SessionStats
    {
        uint32_t MaxHeight;
        uint64_t SessionStart;
        uint64_t SessionEnd;
    };

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

        SessionStats m_SessionStats;
        std::vector<uint64_t> m_CallStats;
        std::unordered_map<std::string, FunctionGlobalStats> m_GlobalStats;

    public:
        ProfilingSession() = default;
        ProfilingSession(SessionHeader header, std::vector<SessionEvent> events);

        [[nodiscard]] const SessionStats& globalStats() const
        {
            return m_SessionStats;
        }

        inline void functionStats(size_t beginIndex, double& selfNanos, double& totalMs, double& totalPercent, double& maxMs,
                                  uint64_t& count)
        {
            auto& beginEvent = m_Events[beginIndex];
            auto& name = m_Header.functionNames()[beginEvent.functionIndex()];
            auto sessionTicks = globalStats().SessionEnd - globalStats().SessionStart;
            selfNanos = (double)m_CallStats[beginIndex] * m_Header.nanosecondsInTick();
            totalMs = (double)m_GlobalStats[name].TotalTicks * m_Header.nanosecondsInTick() / 1'000'000;
            totalPercent = (double)m_GlobalStats[name].TotalTicks / (double)sessionTicks * 100;
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

        ///строит дерево текущего вызова, в узлах которого индексы имён функций из заголовка
        const FuncsTreeNode getFuncsTree(uint32_t eventIdx /*index in events()*/) const;

        ///возвращает список индексов имён функций из заголовка, состоящий только из потомков текущего вызова входящей функции
        const std::vector<uint32_t> getFuncsDescendants(uint32_t eventIdx /*index in events()*/) const;
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
