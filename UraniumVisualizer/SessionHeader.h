#pragma once

#include <string>
#include <vector>

namespace UN
{
    class SessionHeader
    {
    private:
        double m_NanosecondsInTick = 0;
        uint32_t m_FunctionCount   = 0;
        std::vector<std::string> m_FunctionNames;
        uint32_t m_EventCount = 0;

    public:
        SessionHeader() = default;
        
        SessionHeader(
            double NanosecondsInTick, uint32_t FunctionCount, std::vector<std::string> FunctionNames, uint32_t EventCount);

        [[nodiscard]] inline double NanosecondsInTick() const
        {
            return m_NanosecondsInTick;
        }

        [[nodiscard]] inline uint32_t FunctionCount() const
        {
            return m_FunctionCount;
        }

        [[nodiscard]] inline const std::vector<std::string>& FunctionNames() const
        {
            return m_FunctionNames;
        }

        [[nodiscard]] inline uint32_t EventCount() const
        {
            return m_EventCount;
        }

        static SessionHeader GetFakeHeader();
    };
} // namespace UN
