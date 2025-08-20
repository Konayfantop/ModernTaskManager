#include <LogTrace.hpp>
#include <ProcessInfo.hpp>
#include <ExportedFileWrapper.hpp>
#include <fstream>
#include <sstream>
#include <string>

namespace proc
{
namespace
{
void setSecondsAndMs(PidStats::timezone& tmz, const std::string& timeValue)
{
    std::string token;
    std::istringstream ssSecondsAndMs(timeValue);

    std::getline(ssSecondsAndMs, token, '.');
    tmz._seconds = std::stoi(token);

    std::getline(ssSecondsAndMs, token, '.');
    tmz._ms = std::stoi(token);
}

PidStats::timezone getTimeZoneFromFile(const std::string& entityValue)
{
    PidStats::timezone tmz;
    std::istringstream ssEntityValue(entityValue);
    for(int i=0; i<3; ++i)
    {
        std::string timeValue;
        std::getline(ssEntityValue, timeValue, ':');

        // reminder : the format is hh:mm:ss.ms , so we have to parse the ss.ms
        switch(i)
        {
            case 0 : tmz._hours = std::stoi(timeValue); break;
            case 1 : tmz._minutes = std::stoi(timeValue); break;
            case 2 : setSecondsAndMs(tmz, timeValue); break;
        }
    }
    return tmz;
}
}


ExportedFileWrapper::ExportedFileWrapper(const std::filesystem::path& exportedFilePath)
{
    std::ifstream exportedFile(exportedFilePath);
    if(!exportedFile)
    {
        ERROR("Exported file not found. Nothing to wrap");
        return;
    }

    std::string fileLine;
    PidStatus_t::iterator debugIter;
    while(std::getline(exportedFile, fileLine))
    {
        DEBUG("Decoding line : " << fileLine);

        std::string token;
        std::istringstream ss(fileLine);
        PidStatus_t::iterator pidsIter;
        for(int i=0; i<5; ++i)
        {
            std::string systemEntity;
            std::string entityValue;
            std::getline(ss, systemEntity, ' ');
            std::getline(ss, entityValue, ' ');
            
            systemEntity.pop_back(); // remove the ':'
            debugIter = pidsIter;
            switch(i)
            {
                case 0 : pidsIter = _pids.emplace(std::stoi(entityValue), PidStats()).first; break;
                case 1 : pidsIter->second._cpu = std::stod(entityValue); break; 
                case 2 : pidsIter->second._memory = std::stod(entityValue); break;
                case 3 : pidsIter->second._threads = std::stoi(entityValue); break;
                case 4 : pidsIter->second._timezone = getTimeZoneFromFile(entityValue); break;
            }
        }
    }
    _pidsIter = _pids.begin();
}

void ExportedFileWrapper::toDebug()
{
    for(const PidStatus_t::value_type& pidToStats : _pids)
    {
        PidStats::timezone timezone = pidToStats.second._timezone;
        INFO(
            pidToStats.first << 
            " : CPU -> " <<  pidToStats.second._cpu <<
            ", Memory -> " << pidToStats.second._memory <<
            ", Threads -> " << pidToStats.second._threads <<
            ", TimeZone -> " << timezone._hours << ":" << timezone._minutes << ":" << timezone._seconds << "." << timezone._ms);
    }
}

PidStatus_t ExportedFileWrapper::getPidsByStep(const uint step)
{
    PidStatus_t pidsByStep;

    for(uint i=0; i<step && _pidsIter != _pids.end(); ++i)
    {
        pidsByStep.insert(*_pidsIter);
        ++_pidsIter;
    }
    return pidsByStep;
}

PidStatus_t& ExportedFileWrapper::getPids()
{
    return _pids;
}

PidStatus_t::iterator ExportedFileWrapper::getCurrentIter()
{
    return _pidsIter;
}

bool ExportedFileWrapper::isIterPointingEnd()
{
    return _pidsIter == _pids.end();
}

void ExportedFileWrapper::resetIter()
{
    _pidsIter = _pids.begin();
}

}