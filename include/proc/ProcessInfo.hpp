#pragma once

#include <cstdint>
#include <unordered_map>
#include <string>
#include <math.h>
#include <filesystem>

// Sequence of number and their stats based on the number of appearence eg : 
// 1415 (colord) S 1 1415 1415 0 -1 4194560 2271 2377 16 141 1 5 1 9 20 0 4 0 1266 328105984 3675 18446744073709551615 
// 1 1 0 0 0 0 0 4096 0 0 0 0 17 1 0 0 0 0 0 0 0 0 0 0 0 0 0
// `pid : 1415` , `comm : (colord)` , and so on...
// | #  | Field Name              | Description                                      |
// | -- | ----------------------- | ------------------------------------------------ |
// | 1  | `pid`                   | Process ID                                       |
// | 2  | `comm`                  | Filename of the executable (in parentheses)      |
// | 3  | `state`                 | Process state (R/S/D/Z/T/X/W)                    |
// | 4  | `ppid`                  | Parent process ID                                |
// | 5  | `pgrp`                  | Process group ID                                 |
// | 6  | `session`               | Session ID                                       |
// | 7  | `tty_nr`                | TTY the process uses                             |
// | 8  | `tpgid`                 | Process group ID of controlling terminal         |
// | 9  | `flags`                 | Task flags                                       |
// | 10 | `minflt`                | Minor faults (no page load from disk)            |
// | 11 | `cminflt`               | Minor faults (children)                          |
// | 12 | `majflt`                | Major faults (required disk access)              |
// | 13 | `cmajflt`               | Major faults (children)                          |
// | 14 | `utime`                 | User mode jiffies (clock ticks in user space)    |
// | 15 | `stime`                 | Kernel mode jiffies                              |
// | 16 | `cutime`                | User mode jiffies (children)                     |
// | 17 | `cstime`                | Kernel mode jiffies (children)                   |
// | 18 | `priority`              | Priority level (obsolete)                        |
// | 19 | `nice`                  | Nice value (lower is higher priority)            |
// | 20 | `num_threads`           | Number of threads                                |
// | 21 | `itrealvalue`           | Time before next SIGALRM                         |
// | 22 | `starttime`             | Time the process started (in jiffies since boot) |
// | 23 | `vsize`                 | Virtual memory size (in bytes)                   |
// | 24 | `rss`                   | Resident Set Size (pages in RAM)                 |
// | 25 | `rsslim`                | RSS limit                                        |
// | 26 | `startcode`             | Address above which program text can run         |
// | 27 | `endcode`               | Address below which program text can run         |
// | 28 | `startstack`            | Stack start address                              |
// | 29 | `kstkesp`               | Current ESP (stack pointer)                      |
// | 30 | `kstkeip`               | Current EIP (instruction pointer)                |
// | 31 | `signal`                | Pending signals (obsolete)                       |
// | 32 | `blocked`               | Blocked signals                                  |
// | 33 | `sigignore`             | Ignored signals                                  |
// | 34 | `sigcatch`              | Caught signals                                   |
// | 35 | `wchan`                 | Address where process is sleeping                |
// | 36 | `nswap`                 | Swapped pages (not maintained)                   |
// | 37 | `cnswap`                | Swapped pages (children)                         |
// | 38 | `exit_signal`           | Signal sent when child dies                      |
// | 39 | `processor`             | Last CPU the process ran on                      |
// | 40 | `rt_priority`           | Real-time priority                               |
// | 41 | `policy`                | Scheduling policy                                |
// | 42 | `delayacct_blkio_ticks` | Block I/O delays                                 |
// | 43 | `guest_time`            | Guest time (virtual CPU)                         |
// | 44 | `cguest_time`           | Guest time (children)                            |
// | 45 | `start_data`            | Address of data segment                          |
// | 46 | `end_data`              | Address of end of data                           |
// | 47 | `start_brk`             | Address of heap start                            |
// | 48 | `arg_start`             | Address of command line start                    |
// | 49 | `arg_end`               | Address of command line end                      |
// | 50 | `env_start`             | Address of environment start                     |
// | 51 | `env_end`               | Address of environment end                       |
// | 52 | `exit_code`             | Exit status (if process is a zombie)             |

namespace proc
{
static const std::filesystem::path kProcPath = "/proc/";

struct PidStats
{
    double _cpu;
    double _memory;
    uint _threads;
    // TODO: in C++20 use std::chrono and its' explicit members hh_mm_ss
    struct timezone
    {
        uint _hours;
        uint _minutes;
        uint _seconds;
        uint _ms;
    };
    struct timezone _timezone;
};

typedef std::unordered_map<uint, PidStats> PidStatus_t;

class ProcessInfo
{
public:
    ProcessInfo();
    ~ProcessInfo()=default;

    void readAndDisplayProcDir();
    std::string debugProcContent();
    inline const std::filesystem::path& getOldPath(){ return _oldPath; }

protected:
    uint getPidNum(const std::filesystem::directory_entry& entry);
    std::unordered_map<uint, std::string> fillStatMap(const std::filesystem::path& statDir);
    double getGenericUptime(const std::filesystem::path& uptimePath);
    double calculateCpu(const std::unordered_map<uint, std::string>& pidStat, const double& uptime);
    double calculateMemory(const std::unordered_map<uint, std::string>& pidStat, const double meminfo);
    void exportInFile();
    double getMeminfo(const std::filesystem::path& meminfoPath);
    PidStats::timezone calculateProcessUptime(const std::unordered_map<uint, std::string>& statMap, const double uptime);
    
    inline PidStatus_t& accessPidStatus(){ return _pidStatus; }
    inline const PidStatus_t& getPidStatus() { return _pidStatus; } 
    inline std::filesystem::path& accessOldPath(){ return _oldPath; }

    inline std::string refineDouble(const double value)
    {
        return value < 1 ? 
            "0." + std::to_string(static_cast<int>(value*100)) + "%" 
            : std::to_string(static_cast<int>(value)) + "." + std::to_string(static_cast<int>((value - static_cast<int>(value))*100)) + "%";
    }

private:
    PidStatus_t _pidStatus;
    std::filesystem::path _oldPath;
};

}