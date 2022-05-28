#include "FileParser.h"

namespace UN
{
    FileParser::FileParser(){}

    const ProfilingSession FileParser::GetProfilingSession(const char* filePath)
    {
        FILE* file;
        fopen_s(&file, filePath, "rb");

        // SessionHeader
        double nanosecondsInTick;
        uint32_t functionCount;
        std::vector<std::string> functionNames;
        uint16_t nameLength;
        std::vector<char> functionName;
        uint32_t eventCount;

        fread(&nanosecondsInTick, 8, 1, file);
        fread(&functionCount, 4, 1, file);
        for(auto i = 0; i < functionCount; ++i)
        {
            fread(&nameLength, 2, 1, file);
            functionName.resize(nameLength, 0);
            fread(&functionName, 1, nameLength, file);
            functionNames.push_back(std::string(functionName.data(), nameLength));
        }
        fread(&eventCount, 4, 1, file);

        auto h = SessionHeader(nanosecondsInTick, functionCount, functionNames, eventCount);

        // ProfilingSession
        std::vector<SessionEvent> events;
        uint32_t functionIndex;
        uint64_t cpuTicks;

        for(auto i = 0; i < h.EventCount(); ++i)
        {
            fread(&functionIndex, 4, 1, file);
            fread(&cpuTicks, 8, 1, file);
            events.push_back(SessionEvent(functionIndex, cpuTicks));
        }

        fclose(file);

        return ProfilingSession(h, events);
    }
} // namespace UN
