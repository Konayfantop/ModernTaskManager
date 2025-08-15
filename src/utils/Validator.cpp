#include "Validator.hpp"
#include "LogTrace.hpp"

#include <sys/resource.h>
#include <fstream>
#include <string>
#include <functional>
#include <variant>

// BEWARE : 
// Unittesting is being based on the errorlogs, changing the error output logs will mean that the tests should be patched up as well

namespace utils
{
namespace validator
{

std::function<bool(const std::string&)> pidValueCheck = [](const std::string& value) -> bool 
{
     if(std::stoi(value) < 1 || std::stoi(value) > 4194304)
    {
        ERROR("Pid value is either 0 (reserved for swapper) or too great even for modern systems. Continue...");
        return false;
    }
    return true;
};

std::function<bool(const std::string&)> cpuOrMemoryCheck = [](const std::string& value) -> bool
{
    std::size_t aPercentagePos = value.find("%");
    if(aPercentagePos == std::string::npos)
    {
        ERROR("Missing percentage in the value exported. Chances are that the value may be corrupted. Continue...");
        return false;
    }

    const double numericValue = std::stod(value.substr(0, aPercentagePos));
    if(numericValue < 0.0 || numericValue > 100.0)
    {
        ERROR("Unrealistic metric, impossible being negative or 100 -the only pid which occupies the system entity-. Continue...");
        return false;
    }
    return true;
};

std::function<bool(const std::string&)> threadCheck = [](const std::string& value) -> bool 
{
    struct rlimit rl;
    if(getrlimit(RLIMIT_NPROC, &rl) != 0)
    {
        WARNING("Impossible to fetch the limit of the number of the threads a process can occupy. Will assume it's normal. Continue...");
        return true;
    }
    if(std::stoi(value) > rl.rlim_cur)
    {
        ERROR("A process cannot occupy more than " << rl.rlim_cur << " threads at a time. Continue...");
        return false;
    }
    return true;
};

std::function<bool(const std::string&)> timeCheck = [](const std::string& value) -> bool 
{
    int sumTime(0);
    std::istringstream ss(value);
    for(int i=0; i<3; ++i)
    {
        std::string timeValue;
        std::getline(ss, timeValue, ':');

        // reminder : the format is hh:mm:ss.ms , so we have to parse the ss.ms
        if(i == 2)
        {
            std::string token;
            std::istringstream iss(timeValue);

            std::getline(iss, token, '.');
            sumTime += std::stoi(token);

            std::getline(iss, token, '.');
            sumTime += std::stoi(token);
        }
        else
        {
            sumTime += std::stoi(timeValue);
        }
    }
    if(sumTime == 0)
    {
        ERROR("Impossible that the elapsed time of an active-process is zero. Continue...");
        return false;
    }
    return true;
};

bool validateExportedFile(const std::filesystem::path& exportedFile)
{
    std::ifstream processesExport(exportedFile);
    if(!processesExport)
    {
        ERROR("File either wasn't found or never opened. Validator will conclude here, file could be corrupted");
        return false;
    }

    static const std::unordered_map<std::string, std::function<bool(const std::string&)>> kCatalogChecker{
        {"Pid", pidValueCheck}, 
        {"cpu", cpuOrMemoryCheck}, 
        {"memory", cpuOrMemoryCheck}, 
        {"threads", threadCheck}, 
        {"time", timeCheck}
    };

    bool validatorResult{true};
    std::string systemEntity;
    std::string value;
    std::string line;
    while(std::getline(processesExport, line))
    {
        INFO("Validating line : " << line); // DEBUG

        std::string token;
        std::istringstream streamLine(line);
        
        for(int i=0; i<5; ++i)
        {
            std::getline(streamLine, systemEntity, ' ');
            std::getline(streamLine, value, ' ');
            systemEntity.pop_back();

            if(kCatalogChecker.find(systemEntity) == kCatalogChecker.end())
            {
                ERROR("Unknown type of system metric detected : " << systemEntity << ". If this is intented update the validator. Continue...");
                validatorResult = false;
                continue;
            }

            const bool entitySystemResult = kCatalogChecker.at(systemEntity)(value);

            if(!entitySystemResult)
            {
                ERROR("System entity metric validator for : " << systemEntity << ", has failed. Line wasn't verified. Continue...");
                validatorResult = false;
            }
        }
    }
    return validatorResult;
}

}
}