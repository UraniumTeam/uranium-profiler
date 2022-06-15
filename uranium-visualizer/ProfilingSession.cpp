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

    //пока что не получилось построить дерево нерекурсивной функцией
    //в теории переполнение стека будет при вложенности >1000
    const FuncsTreeNode ProfilingSession::getFuncsTree(uint32_t eventIdx) const
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
                    res.descendants().emplace_back(impl(eventIdx, deep, impl));
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

    const std::vector<uint32_t> ProfilingSession::getFuncsDescendants(uint32_t eventIdx) const
    {
        std::vector<uint32_t> res;
        auto event = m_Events[eventIdx];
        auto n = eventIdx;
        if (event.type() == EventType::Begin) {
            ++n;
        }
        else {
            --n;
        }
        auto nextEvent = m_Events[n];
        while (nextEvent.functionIndex() != event.functionIndex()
               || (nextEvent.functionIndex() == event.functionIndex()
                   && nextEvent.type() == event.type()))
        {
            if (nextEvent.type() == event.type()) {
                res.emplace_back(nextEvent.functionIndex());
            }
            if (event.type() == EventType::Begin) {
                ++n;
            }
            else {
                --n;
            }
            nextEvent = m_Events[n];
        }
        return res;
    }
} // namespace UN
