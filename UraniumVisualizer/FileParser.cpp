#include "FileParser.h"

namespace UN
{
    FileParser::FileParser() {}

    ProfilingSession FileParser::GetProfilingSession(const char* filePath)
    {
        FILE* file;
        fopen_s(&file, filePath, "rb");

        double nanosecondsInTick;
        uint32_t functionCount;
        std::vector<std::string> functionNames;
        uint16_t nameLength;
        std::vector<char> functionName;
        uint32_t eventCount;
        fread(&nanosecondsInTick, sizeof(double), 1, file);
        fread(&functionCount, sizeof(uint32_t), 1, file);
        for(uint32_t i = 0; i < functionCount; ++i)
        {
            fread(&nameLength, sizeof(uint16_t), 1, file);
            functionName.resize(nameLength, 0);
            fread(functionName.data(), sizeof(char), nameLength, file);
            functionNames.emplace_back(functionName.data(), nameLength);
        }
        fread(&eventCount, sizeof(uint32_t), 1, file);
        auto h = SessionHeader(nanosecondsInTick, functionCount, functionNames, eventCount);

        std::vector<SessionEvent> events;
        uint32_t functionIndex;
        uint64_t cpuTicks;
        for(uint32_t i = 0; i < h.EventCount(); ++i)
        {
            fread(&functionIndex, sizeof(uint32_t), 1, file);
            fread(&cpuTicks, sizeof(uint64_t), 1, file);
            events.emplace_back(functionIndex, cpuTicks);
        }

        fclose(file);
        return ProfilingSession(h, events);
    }
} // namespace UN
