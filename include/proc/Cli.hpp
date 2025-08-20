#pragma once

#include <filesystem>

// we should be able to present something like this : 
// +-----------------------------------------------------------------------------+
// | Modern Task Monitor - [Sort: CPU Usage] - [Filter: All]                    |
// +------+------------------+----------+------------+------------+-------------+
// | PID  | Process Name     | CPU (%)  | Memory (%) | Threads    | Uptime      |
// +------+------------------+----------+------------+------------+-------------+
// | 1234 | firefox-bin      | 42.3     | 10.5       | 73         | 00:20:54    |
// | 2103 | bash             | 0.1      | 0.3        | 1          | 03:12:04    |
// | 3892 | gnome-shell      | 5.8      | 6.2        | 20         | 06:11:47    |
// +------+------------------+----------+------------+------------+-------------+
// | Total CPU Usage: 68.4% | Memory: 6.3/16.0 GB used (39.4%)                   |
// +-----------------------------------------------------------------------------+
// [Q] Quit | [K] Kill Process | [F] Filter | [S] Sort | [R] Refresh

namespace proc
{
namespace cli
{
void display(const std::filesystem::path& exportedFile);
}
}