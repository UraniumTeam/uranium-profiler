#include "ProfilingSession.h"

namespace UN
{
    ProfilingSession::ProfilingSession(SessionHeader header, vec_SE events)
        : m_header(header)
        , m_events(events)
    {

    }

    ProfilingSession ProfilingSession::GetFakeProfilingSession()
    {
        auto h = SessionHeader::GetFakeHeader();
        return ProfilingSession(h, SessionEvent::GetFakeEvents(h.EventCount()));
    }

    std::string ProfilingSession::ToString(ProfilingSession ps)
    {
        std::string res;
        res.append("NanosecondsInTick "
                   + std::to_string(ps.Header().NanosecondsInTick())
                   + ";\nFunctionCount "
                   + std::to_string(ps.Header().EventCount())
                   + ";\nFunctionNames:\n");
        auto h = ps.Header().FunctionNames();
        res.append(h.at(0));
        for (auto i = h.begin() + 1; i != h.end(); ++i)
            res.append(", " + *i);
        res.append(";\nEventCount "
                   + std::to_string(ps.Header().EventCount()));
        return res;
    }

//    void printf(ProfilingSession ps)
//    {

//        printf(ps.Header());
//    }
} // namespace UN
