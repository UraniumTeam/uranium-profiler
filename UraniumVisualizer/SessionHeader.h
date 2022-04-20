#pragma once

#include <vector>
#include <string>

namespace UN
{
using vec_str = std::vector<std::string>;

    class SessionHeader
    {
    private:
        double   m_NanosecondsInTick = 0;
        uint32_t m_FunctionCount = 0;
        vec_str  m_FunctionNames;
        uint32_t m_EventCount = 0;

    public:
        explicit SessionHeader(
                double NanosecondsInTick
                , uint32_t FunctionCount
                , vec_str  FunctionNames
                , uint32_t EventCount);

        [[nodiscard]] inline double NanosecondsInTick() const
        {
            return m_NanosecondsInTick;
        }

        [[nodiscard]] inline uint32_t FunctionCount() const
        {
            return m_FunctionCount;
        }

        [[nodiscard]] inline const vec_str FunctionNames() const
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
