#include <ProcessInfo.hpp>

#include <iostream>

// Filesystems only for C++17 as std::filesystem starts to exist from 17 and onwards
int main()
{
    //method that will be removed as it will go to a function later;
    proc::ProcessInfo aProcess;
    aProcess.readAndDisplayProcDir();

    std::cout << "END" << std::endl;
    return 0;
}