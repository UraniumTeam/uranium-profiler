#pragma once

#include <iostream>
#include <sstream>
#include <SessionHeader.h>
#include <SessionEvent.h>

namespace UN
{
    class ProfilingSession
    {
    private:
        SessionHeader m_header;
        std::vector<SessionEvent> m_events;

    public:
        explicit ProfilingSession(SessionHeader header, std::vector<SessionEvent> events);

        [[nodiscard]] inline const SessionHeader& Header() const
        {
            return m_header;
        }

        [[nodiscard]] inline const std::vector<SessionEvent>& Events() const
        {
            return m_events;
        }

        static ProfilingSession GetFakeProfilingSession();

        static std::string ToString(const ProfilingSession& ps);
    };

//    void printf(ProfilingSession ps);


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
