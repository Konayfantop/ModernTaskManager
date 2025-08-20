#include "ProcessInfo.hpp"
#include <Validator.hpp>
#include <LogTrace.hpp>
#include <ExportedFileWrapper.hpp>
#include <variant>

#include <sys/resource.h>
#include <string>
#include <functional>

// BEWARE : 
// Unittesting is being based on the errorlogs, changing the error output logs will mean that the tests should be patched up as well
// //TODO : Validation at its' simplest form, enrich/enchance if time permits
namespace utils
{
namespace validator
{
static constexpr uint kBiggestThreadNumberPossible = 4194304u;

std::function<bool(const uint)> pidValueCheck = [](const uint value) -> bool 
{
     if(value < 1 || value > kBiggestThreadNumberPossible)
    {
        ERROR("Pid value is either 0 (reserved for swapper) or too great even for modern systems. Continue...");
        return false;
    }
    return true;
};

std::function<bool(const uint)> cpuOrMemoryCheck = [](const uint value) -> bool
{
    if(value < 0.0 || value >= 100.0)
    {
        ERROR("Unrealistic metric, impossible being negative or 100 -the only pid which occupies the system entity-. Continue...");
        return false;
    }
    return true;
};

std::function<bool(const uint)> threadCheck = [](const uint value) -> bool 
{
    struct rlimit rl;
    if(getrlimit(RLIMIT_NPROC, &rl) != 0)
    {
        WARNING("Impossible to fetch the limit of the number of the threads a process can occupy. Will assume it's normal. Continue...");
        return true;
    }
    if(value > rl.rlim_cur)
    {
        ERROR("A process cannot occupy more than " << rl.rlim_cur << " threads at a time. Continue...");
        return false;
    }
    return true;
};

std::function<bool(const proc::PidStats::timezone&)> timeCheck = [](const proc::PidStats::timezone& tmz) -> bool 
{
    if(tmz._hours + tmz._minutes + tmz._seconds + tmz._seconds == 0)
    {
        ERROR("Impossible that the elapsed time of an active-process is zero. Continue...");
        return false;
    }
    return true;
};

bool validateExportedFile(const std::filesystem::path& exportedFile)
{
    proc::ExportedFileWrapper wrapper(exportedFile);
    const proc::PidStatus_t& pids = wrapper.getPids();

    using FuncVariant = std::variant<std::function<bool(const uint)>, std::function<bool(const proc::PidStats::timezone&)>>;

    static const std::unordered_map<std::string, FuncVariant> kCatalogChecker{
        {"Pid", pidValueCheck}, 
        {"cpu", cpuOrMemoryCheck}, 
        {"memory", cpuOrMemoryCheck}, 
        {"threads", threadCheck}, 
        {"time", timeCheck}
    };

    wrapper.toDebug();

    bool validatorResult{true};
    for(const proc::PidStatus_t::value_type& pid : pids)
    {
        validatorResult &= std::get<std::function<bool(const uint)>>(kCatalogChecker.at("Pid"))(pid.first);
        validatorResult &= std::get<std::function<bool(const uint)>>(kCatalogChecker.at("cpu"))(pid.second._cpu);
        validatorResult &= std::get<std::function<bool(const uint)>>(kCatalogChecker.at("memory"))(pid.second._memory);
        validatorResult &= std::get<std::function<bool(const uint)>>(kCatalogChecker.at("threads"))(pid.second._threads);
        validatorResult &= std::get<std::function<bool(const proc::PidStats::timezone&)>>(kCatalogChecker.at("time"))(pid.second._timezone);
    }

    return validatorResult;
}

}
}