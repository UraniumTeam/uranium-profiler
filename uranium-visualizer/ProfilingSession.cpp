#include "ProfilingSession.h"
#include <algorithm>
#include <limits>
#include <stack>

namespace UN
{
    ProfilingSession::ProfilingSession(SessionHeader header, std::vector<SessionEvent> events)
        : m_Header(std::move(header))
        , m_Events(std::move(events))
        , m_SessionStats()
    {
    }

    void ProfilingSession::calculateStats()
    {
        auto minTime             = std::numeric_limits<uint64_t>::max();
        auto maxTime             = std::numeric_limits<uint64_t>::min();
        m_SessionStats.MaxHeight = 0;

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
                m_SessionStats.MaxHeight = std::max(m_SessionStats.MaxHeight, (uint32_t)eventStack.size());
                continue;
            }

            auto beginIndex  = eventStack.top();
            auto& beginEvent = m_Events[beginIndex];
            eventStack.pop();
            auto length             = event.cpuTicks() - beginEvent.cpuTicks();
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

        m_SessionStats.SessionStart = minTime;
        m_SessionStats.SessionEnd   = maxTime;
    }

    std::vector<uint32_t> ProfilingSession::functionDescendantsImpl(uint32_t eventIdx) const
    {
        std::vector<uint32_t> result;
        std::stack<uint32_t> eventStack;

        for (size_t i = eventIdx; i < m_Events.size(); ++i)
        {
            auto& event = m_Events[i];

            if (event.type() == EventType::Begin)
            {
                eventStack.push(i);
                result.push_back(i);
                continue;
            }

            eventStack.pop();
            if (eventStack.empty())
            {
                break;
            }
        }

        return result;
    }

    ProfilingSession ProfilingSession::getFakeProfilingSession()
    {
        auto h = SessionHeader::getFakeHeader();
        return { h, SessionEvent::getFakeEvents(h.eventCount()) };
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

    FuncsTreeNode ProfilingSession::getFuncsTree(uint32_t eventIdx) const
    {
        uint32_t deep = 0;
        const auto recurSearch = [&](uint32_t eventIdx, uint32_t& deep) -> FuncsTreeNode
        {
            auto recurSearch_impl = [&](uint32_t eventIdx, uint32_t& deep, const auto& impl) -> FuncsTreeNode
            {
                auto event = m_Events[eventIdx];
                FuncsTreeNode res(event.functionIndex());
                if (event.type() == EventType::Begin) {
                    ++deep;
                }
                else {
                    --deep;
                }
                auto nextEvent = m_Events[eventIdx + deep];
                while (nextEvent.functionIndex() != event.functionIndex()
                       || (nextEvent.functionIndex() == event.functionIndex()
                           && nextEvent.type() == event.type()))
                {
                    res.m_Descendants.emplace_back(impl(eventIdx, deep, impl));
                    if (event.type() == EventType::Begin) {
                        ++deep;
                    }
                    else {
                        --deep;
                    }
                    nextEvent = m_Events[eventIdx + deep];
                }
                return res;
            };
            return recurSearch_impl(eventIdx, deep, recurSearch_impl);
        };
        return recurSearch(eventIdx, deep);
    }

    std::vector<uint32_t> ProfilingSession::functionDescendants(uint32_t eventIdx) const
    {
        return functionDescendantsImpl(eventIdx);
    }
} // namespace UN
