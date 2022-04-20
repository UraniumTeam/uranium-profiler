#pragma once

#include <iostream>
#include <SessionHeader.h>
#include <SessionEvent.h>

namespace UN
{
using vec_SE = std::vector<SessionEvent>;

    class ProfilingSession
    {
    private:
        SessionHeader m_header;
        std::vector<SessionEvent> m_events;

    public:
        explicit ProfilingSession(SessionHeader header, vec_SE events);

        [[nodiscard]] inline SessionHeader Header() const
        {
            return m_header;
        }

        [[nodiscard]] inline vec_SE Events() const
        {
            return m_events;
        }

        static ProfilingSession GetFakeProfilingSession();

        static std::string ToString(ProfilingSession ps);
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
