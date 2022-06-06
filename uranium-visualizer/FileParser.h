#pragma once
#include "ProfilingSession.h"

namespace UN
{
    enum class ParsingProblemKind
    {
        None,
        Warning,
        Error
    };

    struct ParsingProblem
    {
        std::string Message;
        ParsingProblemKind Kind;

        [[nodiscard]] std::string toString() const;
        [[nodiscard]] static ParsingProblem warning(std::string message);
        [[nodiscard]] static ParsingProblem error(std::string message);
    };

    class FileParser
    {
        FILE* m_File      = nullptr;
        size_t m_FileSize = 0;
        std::vector<ParsingProblem>& m_Problems;

        explicit FileParser(std::vector<ParsingProblem>& problems)
            : m_Problems(problems)
        {
        }

        template<class T>
        bool tryReadFromFile(T* result, size_t& pointer, size_t length = 1)
        {
            if (pointer + sizeof(T) * length > m_FileSize)
            {
                return false;
            }
            pointer += sizeof(T) * length;
            fread(result, sizeof(T), length, m_File);
            return true;
        }

        void checkProblems(ProfilingSession& session);

    public:
        [[nodiscard]] static FileParser open(const char* filename, std::vector<ParsingProblem>& problems);
        [[nodiscard]] ProfilingSession parse();
        void close();
    };
} // namespace UN
