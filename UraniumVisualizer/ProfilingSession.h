#pragma once

#include <SessionEvent.h>
#include <SessionHeader.h>
#include <iostream>
#include <sstream>

namespace UN
{
    class ProfilingSession
    {
    private:
        SessionHeader m_Header;
        std::vector<SessionEvent> m_Events;

    public:
        ProfilingSession() = default;
        ProfilingSession(SessionHeader header, std::vector<SessionEvent> events);

        [[nodiscard]] inline const SessionHeader& Header() const
        {
            return m_Header;
        }

        [[nodiscard]] inline const std::vector<SessionEvent>& Events() const
        {
            return m_Events;
        }

        void SortEvents();

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
