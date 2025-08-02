#include <ProcessInfo.hpp>
#include <LogTrace.hpp>

#include <cstddef>
#include <exception>
#include <filesystem>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <fstream>
#include <unordered_set>

namespace proc
{

ProcessInfo::ProcessInfo()
{
    if(!std::filesystem::equivalent(std::filesystem::current_path(), kProcPath))
    {
        INFO("Current path is not the one intended, switching... ");
        std::filesystem::current_path(kProcPath);
    }
    NOTIFY("Current path : " << std::filesystem::current_path());
}

// the path entry is "/proc/xxxx"
uint ProcessInfo::getPidNum(const std::filesystem::directory_entry& entry)
{
    std::stringstream ss;
    ss << entry;
    std::size_t lastSlashIndex =  ss.str().find_last_of('/');
    if(lastSlashIndex != std::string::npos)
    {
        return std::stoul(ss.str().substr(lastSlashIndex+1));
    }

    throw std::runtime_error("Pid name of the current dir path: " + ss.str() + ", cannot be extracted. Skipping...");
}

std::unordered_map<uint, std::string> ProcessInfo::fillStatMap(const std::filesystem::path& statDir)
{
    std::unordered_map<uint, std::string> statMap;
    static const std::unordered_set<uint> kPosThatMatter{14,15,20,22,24};

    std::ifstream statFile(statDir.string());
    if(!statFile.is_open())
    {
        throw std::runtime_error("Unable to open stat file. In specific in dir: " + statDir.string() + ". Skipping..." );
    }

    std::string singleLineStatFile;
    while(1)
    {
        std::string fileLine;
        if(!std::getline(statFile, fileLine))
        {
            break;
        }
        singleLineStatFile += fileLine;
    }

    INFO("Stat file found for: " << statDir << " with content: " << singleLineStatFile << ". Parsing...");

    std::istringstream ss(singleLineStatFile);
    uint tokenPos;
    while(1)
    {
        std::string token;
        if(!std::getline(ss, token, ' '))
        {
            break;
        }
        if(kPosThatMatter.find(tokenPos) != kPosThatMatter.end())
        {
            statMap.emplace(tokenPos, token);
        }
        ++tokenPos;
    }
    return statMap;
}

// CPU-time calculation : 
// total_time = utime (14) + stime (15);
// seconds = uptime /proc/uptime - (starttime (22) / CLK_TCK sysconf(_SC_CLK_TCK) );
// cpu_usage = 100 * ((total_time / CLK_TCK sysconf(_SC_CLK_TCK) ) / seconds);
double calculateCpu(const std::unordered_map<uint, std::string>& pidStat)
{
    double total_time = std::stod(pidStat.at(14)) + std::stod(pidStat.at(15));
    double seconds = 
}

// memory : 
// grep MemTotal /proc/meminfo
// # Example output: MemTotal:       16312036 kB
// mem_usage = (rss (24) * page_size) / total_memory_bytes * 100.0;

// threads :
// field 20

// uptime : 
//process_uptime = uptime /proc/uptime (1) - (starttime (22) / CLK_TCK sysconf(_SC_CLK_TCK));
const double 

void ProcessInfo::readAndDisplayProcDir()
{
    // uptime is the same for every process out there -> in seconds
    const double uptime = getGenericUptime(std::filesystem::path(std::filesystem::current_path() / "uptime"));
    
    for (const std::filesystem::directory_entry& entry : std::filesystem::directory_iterator(std::filesystem::current_path()))
    {
        try
        {
            const double 
            // each process is being defined as a directory
            if(entry.is_directory())
            {
                PidStats pidStats;
                const uint pidNum = getPidNum(entry);
                
                const std::unordered_map<uint, std::string> statMap = fillStatMap(
                    std::filesystem::path(std::filesystem::current_path() / std::to_string(pidNum) / "stat"));
                
                const double cpuConsumption = calculateCpu(fillStatMap);

                _pidStatus.emplace(pidNum, PidStats());
            }
        }
        catch(const std::runtime_error& e)
        {
            // Warning in 
            WARNING("Pid name cannot be extracted. Pid in dir path : " << entry << ", won't be included. Skipping...");
            WARNING("For the record the error is: " << e.what());
        }
        catch(const std::exception& e)
        {
            WARNING("Pid name cannot be extracted due to a non-runtime error (worth checking). Dir path : " << entry << ", won't be included. Skipping...");
        }
    }
}

std::string ProcessInfo::debugProcContent()
{
    INFO("MALAKA");
    return "MALAKA";
}
}