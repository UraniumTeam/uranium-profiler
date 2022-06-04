#include "FileParser.h"
#include <stack>

namespace UN
{
    std::string ParsingProblem::ToString() const
    {
        std::stringstream ss;
        switch (Kind)
        {
        case ParsingProblemKind::Warning:
            ss << "warning: ";
            break;
        case ParsingProblemKind::Error:
            ss << "error: ";
            break;
        default:
            break;
        }
        ss << Message;

        return ss.str();
    }

    ParsingProblem ParsingProblem::Warning(std::string message)
    {
        ParsingProblem result;
        result.Message = std::move(message);
        result.Kind    = ParsingProblemKind::Warning;
        return result;
    }

    ParsingProblem ParsingProblem::Error(std::string message)
    {
        ParsingProblem result;
        result.Message = std::move(message);
        result.Kind    = ParsingProblemKind::Error;
        return result;
    }

    FileParser FileParser::Open(const char* filename, std::vector<ParsingProblem>& problems)
    {
        FileParser result(problems);
        fopen_s(&result.m_File, filename, "rb");
        if (result.m_File == nullptr)
        {
            problems.push_back(ParsingProblem::Error("Couldn't open a file"));
            return FileParser(problems);
        }
        fseek(result.m_File, 0L, SEEK_END);
        result.m_FileSize = ftell(result.m_File);
        fseek(result.m_File, 0L, SEEK_SET);
        return result;
    }

    ProfilingSession FileParser::Parse()
    {
        size_t filePointer = 0;

        auto outOfRangeError = [&](const std::string& valueName) -> ProfilingSession {
            std::stringstream ss;
            ss << "Out of file range at value \"" << valueName << "\": "
               << "The entire session is broken.";
            m_Problems.push_back(ParsingProblem::Error(ss.str()));
            return {};
        };

        double nanosecondsInTick;
        if (!TryReadFromFile(&nanosecondsInTick, filePointer))
        {
            return outOfRangeError("Nanoseconds in tick");
        }

        // TODO: maybe change hardcoded values and make them customizable or add a way to suppress these errors
        if (nanosecondsInTick < 0.01 || nanosecondsInTick > 100)
        {
            std::stringstream ss;
            ss << "Probably invalid number of nanoseconds in tick (" << nanosecondsInTick << "): "
               << "Most likely, the entire session is broken.";
            m_Problems.push_back(ParsingProblem::Error(ss.str()));
            return {};
        }

        uint32_t functionCount;
        if (!TryReadFromFile(&functionCount, filePointer))
        {
            return outOfRangeError("Function count");
        }

        std::vector<std::string> functionNames;
        for (uint32_t i = 0; i < functionCount; ++i)
        {
            uint16_t nameLength;
            std::vector<char> functionName;
            if (!TryReadFromFile(&nameLength, filePointer))
            {
                return outOfRangeError("Function name length");
            }

            functionName.resize(nameLength, 0);
            if (!TryReadFromFile(functionName.data(), filePointer, nameLength))
            {
                return outOfRangeError("Function name");
            }
            for (auto c : functionName)
            {
                // TODO: Rust supports unicode in identifiers, so this check may not work in some cases
                if (std::isalnum(c) || c == '_')
                {
                    continue;
                }
                m_Problems.push_back(ParsingProblem::Error("A function had an invalid character: "
                                                           "The entire session is broken."));
                return {};
            }
            functionNames.emplace_back(functionName.data(), nameLength);
        }

        uint32_t eventCount;
        if (!TryReadFromFile(&eventCount, filePointer))
        {
            return outOfRangeError("Event count");
        }
        auto header = SessionHeader(nanosecondsInTick, functionCount, functionNames, eventCount);

        std::vector<SessionEvent> events;
        for (uint32_t i = 0; i < header.EventCount(); ++i)
        {
            uint32_t functionIndex;
            uint64_t cpuTicks;
            if (!TryReadFromFile(&functionIndex, filePointer))
            {
                return outOfRangeError("Event count");
            }
            if (!TryReadFromFile(&cpuTicks, filePointer))
            {
                return outOfRangeError("Event count");
            }
            events.emplace_back(functionIndex, cpuTicks);
        }

        Close();
        auto session = ProfilingSession(header, events);
        session.SortEvents();
        checkProblems(session);
        return session;
    }

    void FileParser::checkProblems(ProfilingSession& session)
    {
        std::stack<SessionEvent> eventStack;
        std::stack<SessionEvent> invalidEnds;
        std::vector<SessionEvent> validEvents;
        for (auto& event : session.Events())
        {
            if (event.EventType() == UN::EventType::Begin)
            {
                if (invalidEnds.empty() || invalidEnds.top().FunctionIndex() != event.FunctionIndex())
                {
                    eventStack.push(event);
                    validEvents.push_back(event);
                }
                else
                {
                    invalidEnds.pop();
                }
                continue;
            }

            if (eventStack.empty())
            {
                std::stringstream ss;
                ss << "The function \"" << session.Header().FunctionNames()[event.FunctionIndex()]
                   << "\" ended before it started: It will be removed.";
                m_Problems.push_back(ParsingProblem::Warning(ss.str()));
                invalidEnds.push(event);
                continue;
            }

            eventStack.pop();
            validEvents.push_back(event);
        }

        auto endInsertionTime = validEvents.back().CpuTicks();
        while (!eventStack.empty())
        {
            auto event = eventStack.top();
            eventStack.pop();
            std::stringstream ss;
            ss << "The function \"" << session.Header().FunctionNames()[event.FunctionIndex()]
               << "\" has no end in the session: The end will be inserted.";
            m_Problems.push_back(ParsingProblem::Warning(ss.str()));
            validEvents.push_back(event.MakeEnd(endInsertionTime));
            endInsertionTime += 100;
        }

        session.Events() = validEvents;
    }

    void FileParser::Close()
    {
        fclose(m_File);
    }
} // namespace UN
