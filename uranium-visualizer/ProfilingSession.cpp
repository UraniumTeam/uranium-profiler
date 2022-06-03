#include "ProfilingSession.h"
#include <algorithm>

namespace UN
{
    ProfilingSession::ProfilingSession(SessionHeader header, std::vector<SessionEvent> events)
        : m_Header(std::move(header))
        , m_Events(std::move(events))
    {

    }

    ProfilingSession ProfilingSession::GetFakeProfilingSession()
    {
        auto h = SessionHeader::GetFakeHeader();
        return ProfilingSession(h, SessionEvent::GetFakeEvents(h.EventCount()));
    }

    std::string ProfilingSession::ToString(const ProfilingSession& ps)
    {
        std::stringstream res;
        res << "NanosecondsInTick "
            << std::to_string(ps.Header().NanosecondsInTick())
            << ";\nFunctionCount "
            << std::to_string(ps.Header().EventCount())
            << ";\nFunctionNames:\n";
        auto h = ps.Header().FunctionNames();
        res << h.at(0);
        for (auto i = h.begin() + 1; i != h.end(); ++i)
        {
            res << ", " << *i;
        }
        res << ";\nEventCount "
            << std::to_string(ps.Header().EventCount());
        return res.str();
    }

    void ProfilingSession::SortEvents()
    {
        std::sort(m_Events.begin(), m_Events.end(), [](const SessionEvent& lhs, const SessionEvent& rhs) {
            return lhs.CpuTicks() < rhs.CpuTicks();
        });
    }

//    void printf(ProfilingSession ps)
//    {

//        printf(ps.Header());
//    }
} // namespace UN
