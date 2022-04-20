#include <SessionHeader.h>

namespace UN
{
    SessionHeader::SessionHeader(
            double NanosecondsInTick
            , uint32_t FunctionCount
            , vec_str  FunctionNames
            , uint32_t EventCount)
            : m_NanosecondsInTick(NanosecondsInTick)
            , m_FunctionCount(FunctionCount)
            , m_FunctionNames(std::move(FunctionNames))
            , m_EventCount(EventCount)
    {

    }

    SessionHeader SessionHeader::GetFakeHeader()
    {
        auto n = 100;
        vec_str FunctionNames;
        for(auto i = 0; i < n; ++i)
        {
            FunctionNames.push_back("func" + std::to_string(i));
        }
        return SessionHeader(10, n, FunctionNames, n*2);
    }
} // namespace UN
