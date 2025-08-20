#pragma once

#include <ProcessInfo.hpp>
#include <filesystem>
#include <unordered_map>

namespace proc
{

class ExportedFileWrapper
{
public:
    explicit ExportedFileWrapper(const std::filesystem::path& exportedFilePath);
    PidStatus_t getPidsByStep(const uint step);
    void toDebug();
    PidStatus_t& getPids();
    PidStatus_t::iterator getCurrentIter();
    bool isIterPointingEnd();
    void resetIter();
private:
    PidStatus_t _pids;
    PidStatus_t::iterator _pidsIter;
};

}