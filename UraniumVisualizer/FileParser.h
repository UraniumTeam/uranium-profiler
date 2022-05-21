#pragma once
#include "ProfilingSession.h"

namespace UN
{
    class FileParser
    {
    private:
        //std::string m_FilePath;
    public:
        FileParser();

//        [[nodiscard]] inline const std::string& GetFilePath() const
//        {
//            return m_FilePath;
//        }

        static const ProfilingSession GetProfilingSession(const char* filePath);
    };
} // namespace UN
