#include <ProcessInfo.hpp>
#include <Validator.hpp>
#include <LogTrace.hpp>
#include <Cli.hpp>

// Filesystems only for C++17 as std::filesystem starts to exist from 17 and onwards
int main()
{
    //method that will be removed as it will go to a function later;
    proc::ProcessInfo aProcess;
    aProcess.readAndDisplayProcDir();

    const std::filesystem::path exportedFile(aProcess.getOldPath().parent_path() / "export/ProcessesStatus.txt");
    if(!utils::validator::validateExportedFile(exportedFile))
    {
        ERROR("Validation failed. Check your file for potential corruptions");
        return 1;
    }

    // proc::cli::display(exportedFile);
    
    return 0;
}