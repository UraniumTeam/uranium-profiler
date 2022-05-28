#pragma once
#include "ProfilingSession.h"

namespace UN
{
    class FileParser
    {
    public:
        FileParser();

        static ProfilingSession GetProfilingSession(const char* filePath);
    };
} // namespace UN
