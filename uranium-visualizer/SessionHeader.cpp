#include <SessionHeader.h>

namespace UN
{
    SessionHeader::SessionHeader(
            double NanosecondsInTick,
            uint32_t FunctionCount,
            std::vector<std::string> FunctionNames,
            uint32_t EventCount)
            : m_NanosecondsInTick(NanosecondsInTick)
            , m_FunctionCount(FunctionCount)
            , m_FunctionNames(std::move(FunctionNames))
            , m_EventCount(EventCount)
    {

    }

    SessionHeader SessionHeader::getFakeHeader()
    {
        auto n = 100;
        std::vector<std::string> FunctionNames;
        for(auto i = 0; i < n; ++i)
        {
            FunctionNames.push_back("func" + std::to_string(i));
        }
        return SessionHeader(10, n, FunctionNames, n*2);
    }
} // namespace UN
