#pragma once
#include <vector>
#include <cstdint>
#include <EventType.h>

namespace UN
{
    class SessionEvent
    {
    private:
        uint32_t m_FunctionIndex = 0;
        uint64_t m_CpuTicks = 0;
        EventType m_Type;

//    [[nodiscard]] inline EventType convertToEventType(uint32_t raw_index) const
//        {
//            return (raw_index >> 28) == 0x0 ? Begin: End;
//        }

//    inline void changeEventType()
//        {
//            m_Type = m_Type ? Begin: End;
//        }

    public:
        explicit SessionEvent(uint32_t raw_index, uint64_t cpu_ticks);

        [[nodiscard]] inline uint32_t FunctionIndex() const
        {
            return m_FunctionIndex;
        }

        [[nodiscard]] inline uint64_t CpuTicks() const
        {
            return m_CpuTicks;
        }

        [[nodiscard]] inline EventType EventType() const
        {
            return m_Type;
        }

//        static SessionEvent GetFakeEvent();

        static std::vector<SessionEvent> GetFakeEvents(uint32_t count);
    };
} // namespace UN

