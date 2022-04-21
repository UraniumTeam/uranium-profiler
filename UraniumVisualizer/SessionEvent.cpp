#include <SessionEvent.h>

namespace UN
{
    SessionEvent::SessionEvent(uint32_t raw_index, uint64_t cpu_ticks)
        : m_FunctionIndex(raw_index & 0x0fffffff)
        , m_CpuTicks(cpu_ticks)
        , m_Type((raw_index >> 28) == 0x0 ? EventType::Begin: EventType::End){}


//    SessionEvent SessionEvent::GetFakeEvent()
//    {
//        std::random_device rd;
//        std::uniform_int_distribution<> index(0, 1000);
//
//    }

    std::vector<SessionEvent> SessionEvent::GetFakeEvents(uint32_t count)
    {
        std::vector<SessionEvent> res;
        auto cpuTicks = 10000000;
        for(auto i = 0; i < count/2; ++i)
        {
            auto fe = SessionEvent(i % 2, cpuTicks);
            res.push_back(fe);
            res.push_back(SessionEvent(fe.FunctionIndex() + (1 << 28), fe.CpuTicks()+1000));
             cpuTicks += 1010;
        }
        return res;
    }
} // namespace UN
