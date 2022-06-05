#include "ProfilingSession.h"
#include <algorithm>
#include <limits>
#include <stack>

namespace UN
{
    ProfilingSession::ProfilingSession(SessionHeader header, std::vector<SessionEvent> events)
        : m_Header(std::move(header))
        , m_Events(std::move(events))
        , m_SessionTicks(0)
        , m_MaxHeight(0)
    {
    }

    void ProfilingSession::CalculateStats()
    {
        auto minTime = std::numeric_limits<uint64_t>::max();
        auto maxTime = std::numeric_limits<uint64_t>::min();
        m_MaxHeight  = 0;

        m_CallStats.clear();
        m_CallStats.resize(m_Events.size());

        std::stack<size_t> eventStack;
        for (size_t i = 0; i < m_Events.size(); ++i)
        {
            auto& event = m_Events[i];

            minTime = std::min(minTime, event.CpuTicks());
            maxTime = std::max(maxTime, event.CpuTicks());

            if (event.EventType() == EventType::Begin)
            {
                eventStack.push(i);
                m_MaxHeight = std::max(m_MaxHeight, (uint32_t)eventStack.size());
                continue;
            }

            auto beginIndex = eventStack.top();
            auto& beginEvent = m_Events[beginIndex];
            eventStack.pop();
            auto length = event.CpuTicks() - beginEvent.CpuTicks();
            m_CallStats[beginIndex] = length;

            auto& name = m_Header.FunctionNames()[beginEvent.FunctionIndex()];
            if (m_GlobalStats.find(name) == m_GlobalStats.end())
            {
                m_GlobalStats[name] = {};
            }
            m_GlobalStats[name].MaxTicks = std::max(m_GlobalStats[name].MaxTicks, length);
            m_GlobalStats[name].TotalTicks += length;
            m_GlobalStats[name].Count++;
        }

        m_SessionTicks = maxTime - minTime;
    }

    ProfilingSession ProfilingSession::GetFakeProfilingSession()
    {
        auto h = SessionHeader::GetFakeHeader();
        return ProfilingSession(h, SessionEvent::GetFakeEvents(h.EventCount()));
    }

    std::string ProfilingSession::ToString(const ProfilingSession& ps)
    {
        std::stringstream res;
        res << "NanosecondsInTick " << std::to_string(ps.Header().NanosecondsInTick()) << ";\nFunctionCount "
            << std::to_string(ps.Header().EventCount()) << ";\nFunctionNames:\n";
        auto h = ps.Header().FunctionNames();
        res << h.at(0);
        for (auto i = h.begin() + 1; i != h.end(); ++i)
        {
            res << ", " << *i;
        }
        res << ";\nEventCount " << std::to_string(ps.Header().EventCount());
        return res.str();
    }

    void ProfilingSession::SortEvents()
    {
        std::sort(m_Events.begin(), m_Events.end(), [](const SessionEvent& lhs, const SessionEvent& rhs) {
            return lhs.CpuTicks() < rhs.CpuTicks();
        });
    }
} // namespace UN
