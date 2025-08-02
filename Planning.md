🌐 What Will the User See?
They open a terminal and launch your program, say:

bash
Copy
Edit
./task-monitor
They are greeted with:

pgsql
Copy
Edit
+-----------------------------------------------------------------------------+
| Modern Task Monitor - [Sort: CPU Usage] - [Filter: All]                    |
+------+------------------+----------+------------+------------+-------------+
| PID  | Process Name     | CPU (%)  | Memory (%) | Threads    | Uptime      |
+------+------------------+----------+------------+------------+-------------+
| 1234 | firefox-bin      | 42.3     | 10.5       | 73         | 00:20:54    |
| 2103 | bash             | 0.1      | 0.3        | 1          | 03:12:04    |
| 3892 | gnome-shell      | 5.8      | 6.2        | 20         | 06:11:47    |
+------+------------------+----------+------------+------------+-------------+
| Total CPU Usage: 68.4% | Memory: 6.3/16.0 GB used (39.4%)                   |
+-----------------------------------------------------------------------------+
[Q] Quit | [K] Kill Process | [F] Filter | [S] Sort | [R] Refresh

The user can press:

↑/↓ to scroll through processes

K to kill a process by selecting it

F to filter by name

S to change sorting order (CPU, memory, etc.)

All in a fast-refreshing, interactive terminal UI. Slick.

🔩 Under the Hood (System + Architecture)
📂 Data Sources (Linux)
What	Where
Process list	/proc/ (scan /proc/[pid])
CPU usage	/proc/stat
Memory usage	/proc/meminfo, /proc/[pid]/status
Uptime	/proc/uptime or from /proc/[pid]/stat
Threads	/proc/[pid]/task/
Command	/proc/[pid]/cmdline
Kill process	kill(pid, SIGTERM)

You’ll build classes that wrap these files and expose:

cpp
Copy
Edit

You’ll build classes that wrap these files and expose:

cpp
Copy
Edit
class ProcessInfo {
  int pid;
  std::string name;
  float cpuUsage;
  float memoryUsage;
  int numThreads;
  std::chrono::duration uptime;
};
🧵 Multithreaded Engine
You’ll use:

1 thread for UI rendering

1 or more threads for data collection

Polling /proc every 0.5–1s

Calculating diffs (for CPU usage)

Managing internal cache of process info

Everything must be thread-safe, which is a chance to:

Use mutexes, locks, or concurrent queues

Apply RAII patterns for safety

Use std::future or std::async optionally

🧪 Testing System
Unit test components like:

/proc parsers

sorting/filtering logic

formatting functions (e.g., bytesToHumanReadable)

Use GoogleTest and CI with GitHub Actions.

📦 Build System & Tooling
Use:

CMake (modern style with targets)

clang-tidy (style checks)

valgrind or ASAN for memory checks

Optional: code coverage tools


🧭 Project Stages
Phase	Goal
✅ Week 1	Parse /proc, get list of PIDs
✅ Week 2	Per-process info (CPU, mem, name, threads)
✅ Week 3	Basic TUI with ncurses or FTXUI
✅ Week 4	Live update every 1s, with sorting/filtering
✅ Week 5	Multithreaded data collection engine
✅ Week 6	Kill/resume/stop processes
✅ Week 7	Refactor, write unit tests, doc
✅ Week 8	Polish, push to GitHub, write README, make demo GIF!



-----------------------------------------------------------------------WEEK 1 ---------------------------------------------------------------


📆 Week 1: Parse /proc and Get List of Running Processes
🎯 Goal
By the end of this week, you will have a working C++ program that:

Lists all currently running processes on your Linux system.

Displays at least the PID, process name, and command line.

Uses RAII, modern C++, and good error handling.

Is clean, testable, and can be extended in later weeks.

📆 Week 1: Parse /proc and Get List of Running Processes
🎯 Goal
By the end of this week, you will have a working C++ program that:

Lists all currently running processes on your Linux system.

Displays at least the PID, process name, and command line.

Uses RAII, modern C++, and good error handling.

Is clean, testable, and can be extended in later weeks.


🧠 Why This Matters
This is the foundation of the entire task manager. Every other feature (CPU usage, memory usage, etc.) depends on parsing and understanding /proc.

You'll get hands-on practice with:

File IO (std::ifstream)

C++17 filesystem (std::filesystem)

Linux internals (/proc/[pid]/)

Clean OOP design (ProcessInfo, ProcParser)

Modular design for future testing

🔍 What is /proc/?
/proc is a virtual filesystem in Linux. It contains:

System-wide info: /proc/meminfo, /proc/stat, /proc/uptime

Per-process info: /proc/[PID]/

Each directory under /proc/ that has a numeric name (e.g., 1234, 876) corresponds to a running process. Inside each:

cmdline: the full command

comm: short name (executable name)

status: human-readable stats

stat: machine-readable CPU time, etc.


🧱 Deliverables
1. A Struct to Hold Process Info
cpp
Copy
Edit
struct ProcessInfo {
    int pid;
    std::string name;
    std::string cmdline;
};
2. A Class to Parse /proc/
cpp
Copy
Edit
class ProcParser {
public:
    std::vector<ProcessInfo> getProcessList();
};
Uses std::filesystem to iterate over /proc/

Filters directories that are numeric

For each, reads:

/proc/[pid]/comm → name

/proc/[pid]/cmdline → full command line

3. A Simple main.cpp to Test
cpp
Copy
Edit
int main() {
    ProcParser parser;
    auto processes = parser.getProcessList();

    for (const auto& proc : processes) {
        std::cout << "PID: " << proc.pid << " | "
                  << "Name: " << proc.name << " | "
                  << "Cmd: " << proc.cmdline << '\n';
    }
    return 0;
}
🧪 Tests (Optional this week, required next week)
You can optionally write unit tests for:

Valid /proc/[pid]/comm parsing

Handling non-readable /proc/[pid]/cmdline (some kernel/system processes will have empty cmdline)

🛠️ Tools & Features to Use This Week
Tool	Why
std::filesystem	To iterate over /proc/, check if a name is a number
std::ifstream	To read from /proc/[pid]/comm and /cmdline
CMake	Organize your project cleanly from the start
Git	Commit each feature (class, parser, test)
clang-format	Keep code style clean

📂 Suggested Folder Structure
cpp
Copy
Edit
task-monitor/
├── src/
│   ├── main.cpp
│   ├── ProcParser.cpp
│   └── ProcParser.hpp
├── include/
│   └── ProcessInfo.hpp
├── tests/
│   └── test_proc_parser.cpp (optional this week)
├── CMakeLists.txt
└── README.md


⚠️ Common Pitfalls
Some /proc/[pid]/ entries might vanish while you're scanning — process died. Handle std::ifstream gracefully.

cmdline is null-separated, not space-separated. You'll need to replace '\0' with ' ' for display.

Some processes have no permissions to read — skip them silently or show [unreadable].

✅ Summary of Week 1 Checklist
Task	Done?
Set up CMake + main.cpp	⬜
Create ProcParser class	⬜
List all numeric subfolders in /proc	⬜
Read /comm and /cmdline for each	⬜
Print them to terminal	⬜
Optional: add unit test	⬜
Optional: GitHub commit with README	⬜

