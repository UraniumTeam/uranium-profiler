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

        [[nodiscard]] std::string ToString() const;
        [[nodiscard]] static ParsingProblem Warning(std::string message);
        [[nodiscard]] static ParsingProblem Error(std::string message);
    };

    class FileParser
    {
        FILE* m_File = nullptr;
        size_t m_FileSize = 0;
        FileParser() = default;

        template<class T>
        bool TryReadFromFile(T* result, size_t& pointer, size_t length = 1) {
            if (pointer + sizeof(T) * length > m_FileSize) {
                return false;
            }
            pointer += sizeof(T) * length;
            fread(result, sizeof(T), length, m_File);
            return true;
        }

    public:
        [[nodiscard]] static FileParser Open(const char* filename);
        [[nodiscard]] ProfilingSession Parse(std::vector<ParsingProblem>& problems);
        void Close();
    };
} // namespace UN
