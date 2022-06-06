#include "ProfilingSession.h"
#include <algorithm>
#include <limits>
#include <stack>

namespace UN
{
    ProfilingSession::ProfilingSession(SessionHeader header, std::vector<SessionEvent> events)
        : m_Header(std::move(header))
        , m_Events(std::move(events))
        , m_MaxHeight(0)
        , m_SessionTicks(0)
    {
    }

    void ProfilingSession::calculateStats()
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

            minTime = std::min(minTime, event.cpuTicks());
            maxTime = std::max(maxTime, event.cpuTicks());

            if (event.type() == EventType::Begin)
            {
                eventStack.push(i);
                m_MaxHeight = std::max(m_MaxHeight, (uint32_t)eventStack.size());
                continue;
            }

            auto beginIndex = eventStack.top();
            auto& beginEvent = m_Events[beginIndex];
            eventStack.pop();
            auto length = event.cpuTicks() - beginEvent.cpuTicks();
            m_CallStats[beginIndex] = length;

            auto& name = m_Header.functionNames()[beginEvent.functionIndex()];
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

    ProfilingSession ProfilingSession::getFakeProfilingSession()
    {
        auto h = SessionHeader::getFakeHeader();
        return ProfilingSession(h, SessionEvent::getFakeEvents(h.eventCount()));
    }

    std::string ProfilingSession::toString(const ProfilingSession& ps)
    {
        std::stringstream res;
        res << "NanosecondsInTick " << std::to_string(ps.header().nanosecondsInTick()) << ";\nFunctionCount "
            << std::to_string(ps.header().eventCount()) << ";\nFunctionNames:\n";
        auto h = ps.header().functionNames();
        res << h.at(0);
        for (auto i = h.begin() + 1; i != h.end(); ++i)
        {
            res << ", " << *i;
        }
        res << ";\nEventCount " << std::to_string(ps.header().eventCount());
        return res.str();
    }

    void ProfilingSession::sortEvents()
    {
        std::sort(m_Events.begin(), m_Events.end(), [](const SessionEvent& lhs, const SessionEvent& rhs) {
            return lhs.cpuTicks() < rhs.cpuTicks();
        });
    }
} // namespace UN
