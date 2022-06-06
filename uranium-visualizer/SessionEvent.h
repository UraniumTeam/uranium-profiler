#pragma once
#include <vector>
#include <cstdint>

namespace UN
{
    enum class EventType
    {
        Begin,
        End,
        None = 0x7fffffff
    };

    class SessionEvent
    {
    private:
        uint32_t m_FunctionIndex = 0;
        uint64_t m_CpuTicks = 0;
        EventType m_Type = EventType::None;

    [[nodiscard]] inline static EventType getEventType(uint32_t raw_index)
        {
            return (raw_index >> 28) == 0x0 ? EventType::Begin: EventType::End;
        }

    public:
        SessionEvent() = default;
        SessionEvent(uint32_t raw_index, uint64_t cpu_ticks);

        [[nodiscard]] inline uint32_t functionIndex() const
        {
            return m_FunctionIndex;
        }

        [[nodiscard]] inline uint64_t cpuTicks() const
        {
            return m_CpuTicks;
        }

        [[nodiscard]] inline EventType type() const
        {
            return m_Type;
        }

        [[nodiscard]] inline SessionEvent makeEnd(uint64_t cpuTicks) const
        {
            SessionEvent result;
            result.m_CpuTicks = cpuTicks;
            result.m_FunctionIndex = m_FunctionIndex;
            result.m_Type = EventType::End;
            return result;
        }

        static std::vector<SessionEvent> getFakeEvents(uint32_t count);
    };
} // namespace UN

