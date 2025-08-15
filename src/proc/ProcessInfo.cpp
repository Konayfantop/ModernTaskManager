#include "Exception.hpp"
#include <ProcessInfo.hpp>
#include <LogTrace.hpp>

#include <cctype>
#include <cmath>
#include <cstddef>
#include <cstdlib>
#include <ctime>
#include <exception>
#include <filesystem>
#include <ostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <fstream>
#include <unordered_set>
#include <unistd.h>

namespace proc
{

ProcessInfo::ProcessInfo()
{
    _oldPath = std::filesystem::current_path();
    if(!std::filesystem::equivalent(std::filesystem::current_path(), kProcPath))
    {
        INFO("Current path is not the one intended, switching... ");
        std::filesystem::current_path(kProcPath);
    }
    NOTIFY("Current path : " << std::filesystem::current_path());
}

// the path entry is "/proc/xxxx" DONE
uint ProcessInfo::getPidNum(const std::filesystem::directory_entry& entry)
{
    std::stringstream ss;
    ss << entry;
    std::size_t lastSlashIndex =  ss.str().find_last_of('/');
    if(lastSlashIndex != std::string::npos)
    {
        const std::string potentialPidNum = ss.str().substr(lastSlashIndex+1);
        if(isdigit(potentialPidNum[0]))
        {
            return std::stoi(ss.str().substr(lastSlashIndex+1));
        }
        throw utils::SeverityException<utils::HarmlessException>("Found a non-pid virtual directory. Skipping...");
    }

    throw utils::SeverityException<utils::ModerateException>("Pid name of the current dir path: " + ss.str() + ", cannot be extracted. Skipping...");
}

// DONE
std::unordered_map<uint, std::string> ProcessInfo::fillStatMap(const std::filesystem::path& statDir)
{
    std::unordered_map<uint, std::string> statMap;
    static const std::unordered_set<uint> kPosThatMatter{14,15,20,22,24};

    std::ifstream statFile(statDir.string());
    if(!statFile.is_open())
    {
        throw utils::SeverityException<utils::ModerateException>("Unable to open stat file. In specific in dir: " + statDir.string() + ". Skipping..." );
    }

    std::string singleLineStatFile;
    std::getline(statFile, singleLineStatFile);

    INFO("Stat file found for: " << statDir << " with content: " << singleLineStatFile << ". Parsing..."); // TODO : Can be set as debug

    std::istringstream ss(singleLineStatFile);
    uint tokenPos{1u};
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

    // TODO : Do we really have to check here this ?? Maybe needless if -> better think about it !
    if(statMap.size() != 5)
    {
        throw utils::SeverityException<utils::ModerateException>("Some entries were missed during parsing. Calculations will be undone. Skipping...");
    }
    return statMap;
}

// DONE
// CPU-time calculation : 
// total_time = utime (14) + stime (15);
// seconds = uptime /proc/uptime - (starttime (22) / CLK_TCK sysconf(_SC_CLK_TCK) );
// cpu_usage = 100 * ((total_time / CLK_TCK sysconf(_SC_CLK_TCK) ) / seconds);
double ProcessInfo::calculateCpu(const std::unordered_map<uint, std::string>& statMap, const double& uptime)
{
    double total_time = std::stod(statMap.at(14u)) + std::stod(statMap.at(15u));
    double seconds = uptime - (std::stod(statMap.at(22u)) / static_cast<double>(sysconf(_SC_CLK_TCK))); // convert the starttime (it is calculated by clock ticks to seconds)
    return 100 * ((total_time / static_cast<double>(sysconf(_SC_CLK_TCK))) / seconds);
}

//DONE
double ProcessInfo::getMeminfo(const std::filesystem::path& meminfoPath)
{
    std::ifstream meminfoFile(meminfoPath);
    if(!meminfoFile.is_open())
    {
        throw utils::SeverityException<utils::SeriousException>("Meminfo file cannot be opened or wasn't found. Memory consumption won't be calculated");
    }

    // No complexity at all, 99% MemTotal entry would be first
    std::string line;
    while(std::getline(meminfoFile, line))
    {
        std::size_t memtotalIndex = line.find("MemTotal");
        if(memtotalIndex != std::string::npos)
        {
            break;
        }
    }

    std::istringstream ss(line);
    while(std::getline(ss, line, ' '))
    {
        if(std::isdigit(line[0]))
        {
            return std::stod(line);
        }
    }

    throw utils::SeverityException<utils::SeriousException>("MemTotal doesn't exist or it wasn't found");
}

// DONE
// grep MemTotal /proc/meminfo
// # Example output: MemTotal:       16312036 kB
// mem_usage = (rss (24) * page_size sysconf(_SC_PAGESIZE)) / total_memory_bytes (content from files is in kB) * 100.0;
double ProcessInfo::calculateMemory(const std::unordered_map<uint, std::string>& statMap, const double meminfo)
{
    return ((std::stod(statMap.at(24u)) * static_cast<double>(sysconf(_SC_PAGESIZE))) / (meminfo * 1024)) * 100.0;
}

// DONE
//process_uptime = uptime /proc/uptime (1) - (starttime (22) / CLK_TCK sysconf(_SC_CLK_TCK));
PidStats::timezone ProcessInfo::calculateProcessUptime(const std::unordered_map<uint, std::string>& statMap, const double uptime)
{
    PidStats::timezone processTimezone;

    const double processTimeInSeconds = uptime - (std::stod(statMap.at(22u)) / sysconf(_SC_CLK_TCK)); // in seconds
    processTimezone._hours = processTimeInSeconds / 3600;
    processTimezone._minutes = (processTimeInSeconds - (processTimezone._hours*3600)) / 60;   
    const double secondsRemaining = processTimeInSeconds - static_cast<double>(processTimezone._hours*3600) - static_cast<double>(processTimezone._minutes*60);
    processTimezone._seconds = secondsRemaining; // on-purpose truncating the decimal  as we want integer seconds
    processTimezone._ms = std::round((secondsRemaining - processTimezone._seconds) * 1000); // it's ok if we take at least a 3-digit ms

    // if the time has even 0ms after calculation, the pids may be either workers from kernel or zombie processes :
    // 1) workers : they exist in /proc/ but they never occupy anything and they are lost in an instant
    // 2) zombie pids : they have finished executing and they are terminated but they still occupy a pid entry
    if(processTimezone._hours == processTimezone._minutes
        && processTimezone._minutes == processTimezone._seconds
        && processTimezone._seconds == processTimezone._ms
        && processTimezone._ms == 0)
    {
        throw utils::SeverityException<utils::HarmlessException>("Worker or Zombie process, all the metrics will be fake");
    }

    return processTimezone;
}

// DONE
double ProcessInfo::getGenericUptime(const std::filesystem::path& uptimePath)
{
    std::ifstream uptimeFile(uptimePath);
    std::string singleLine;
    std::getline(uptimeFile, singleLine);

    std::size_t separatorIndex = singleLine.find_first_of(' ');
    if(separatorIndex == std::string::npos)
    {
        throw utils::SeverityException<utils::SeriousException>("Unrecoverable error. Cannot extract generic process uptime from /proc/uptime. Cannot calculate anything");
    }

    // rawUptime because the other value we truncated was the uptime during idle procedure
    return std::stod(singleLine.substr(0, separatorIndex));
}

// DONE
void ProcessInfo::exportInFile()
{
    std::filesystem::path projectPathFileExport = _oldPath.parent_path() / "export/ProcessesStatus.txt";

    INFO("Exporting process data in a file called: ProcessesStatus.txt" << projectPathFileExport);

    std::ofstream processesStatus(projectPathFileExport);
    if(!processesStatus)
    {
        throw utils::SeverityException<utils::SeriousException>("ERROR: Cannot open the file to export Pid statuses");
    }

    std::ostringstream ss;
    for(const auto& [pidNum, stats] : _pidStatus)
    {
        ss << "Pid: " << pidNum << " cpu: " << refineDouble(stats._cpu) << " memory: " << refineDouble(stats._memory) << " threads: " << stats._threads << " time: " << 
            stats._timezone._hours << ":" << stats._timezone._minutes << ":" << stats._timezone._seconds << "." << stats._timezone._ms << std::endl;
    }
    processesStatus << ss.str();

    processesStatus.close();
}

void ProcessInfo::readAndDisplayProcDir()
{
    // uptime is the same for every process out there -> in seconds
    double uptime;
    double meminfo;
    try
    {
        uptime = getGenericUptime(std::filesystem::path(std::filesystem::current_path() / "uptime"));
        meminfo = getMeminfo(std::filesystem::path(std::filesystem::current_path() / "meminfo"));
    }
    catch(const utils::SeriousException& e)
    {
        ERROR("ERROR : /proc/uptime decoding issue : " << e.what() );
        return;
    }

    for (const std::filesystem::directory_entry& entry : std::filesystem::directory_iterator(std::filesystem::current_path()))
    {
        try
        {
            // each process is being defined as a directory
            if(entry.is_directory())
            {
                PidStats pidStats;
                const uint pidNum = getPidNum(entry);
                
                const std::unordered_map<uint, std::string> statMap = fillStatMap(
                    std::filesystem::path(std::filesystem::current_path() / std::to_string(pidNum) / "stat"));
                
                pidStats._cpu = calculateCpu(statMap, uptime);
                pidStats._memory = calculateMemory(statMap, meminfo);
                pidStats._threads = static_cast<uint>(std::stoi(statMap.at(20)));
                pidStats._timezone = calculateProcessUptime(statMap, uptime);

                _pidStatus.emplace(pidNum, pidStats);
            }
        }
        catch(const utils::HarmlessException& e)
        {
            NOTIFY("Harmless exception caught : " << e.what());
        }
        catch(const utils::ModerateException& e)
        {
            WARNING("Pid name cannot be extracted. Pid in dir path : " << entry << ", won't be included, because : " << e.what() << ". Skipping...");
        }
        catch(const utils::SeriousException& e)
        {
            ERROR("Unrecoverable error occured : " << e.what() << ", process will stop right away");
            return;
        }
        catch(const std::exception& e)
        {
            WARNING("Pid name cannot be extracted due to a non-runtime error (worth checking), " << e.what() << ". Skipping...");
        }
    }

    INFO("Process has been completed successfully (with some skips ?) and a total of: " << _pidStatus.size() << " processes. Exporting...");

    // after the extraction process, an exportation one begins right after to keep them in a file(so that we won't have to recalculate every time)
    exportInFile();
}

std::string ProcessInfo::debugProcContent()
{
    INFO("MALAKA");
    return "MALAKA";
}
}