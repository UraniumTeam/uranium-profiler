#include <SessionEvent.h>

namespace UN
{
    SessionEvent::SessionEvent(uint32_t raw_index, uint64_t cpu_ticks)
        : m_FunctionIndex(raw_index & 0x0fffffff)
        , m_CpuTicks(cpu_ticks)
        , m_Type(getEventType(raw_index)){}

    std::vector<SessionEvent> SessionEvent::getFakeEvents(uint32_t count)
    {
        std::vector<SessionEvent> res;
        auto cpuTicks = 10000000;
        for(auto i = 0; i < count/2; ++i)
        {
            auto fe = SessionEvent(i, cpuTicks);
            res.push_back(fe);
            res.push_back(SessionEvent(fe.functionIndex() + (1 << 28), fe.cpuTicks()+1000));
             cpuTicks += 1010;
        }
        return res;
    }
} // namespace UN
