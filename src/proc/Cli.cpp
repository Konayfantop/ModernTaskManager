#include <Cli.hpp>
#include <ProcessInfo.hpp>
#include <string>
#include <ExportedFileWrapper.hpp>

namespace proc
{
namespace cli
{

static constexpr char kUpperAndDownTableFormat[] = "+-----------------------------------------------------------------------------+\n";
static constexpr char kTitleTableFormat[] = "| Modern Task Monitor - [Sort: CPU Usage] - [Filter: All]                    |\n";
static constexpr char kBoundariesInBetween[] = "+------+------------------+----------+------------+------------+-------------+\n";
static constexpr char kColumnNames[] = "| PID  | Process Name     | CPU (%)  | Memory (%) | Threads    | Uptime      |\n";
static constexpr char kTotalSumMetrics[] = "| Total CPU Usage: % | Memory:  GB used (%)                   |\n";
static constexpr char kMenuDisplay[] = "[Q] Quit | [K] Kill Process | [F] Filter | [S] Sort | [R] Refresh\n";
static constexpr int kStep = 5;

void display(const std::filesystem::path& exportedFile)
{
    ExportedFileWrapper wrapper(exportedFile);
    wrapper.getPidsByStep(5);
    std::string cliDisplay;
    while(1)
    {
        cliDisplay = kUpperAndDownTableFormat;
        cliDisplay += kTitleTableFormat;
        cliDisplay += kBoundariesInBetween;
        cliDisplay += kColumnNames;
        cliDisplay += kBoundariesInBetween;
        
        const PidStatus_t pidMetrics = wrapper.getPidsByStep(kStep);
        for(const PidStatus_t::value_type& pidWithMetrics : pidMetrics)
        {
            std::string PidMetricRow("| " + std::to_string(pidWithMetrics.first) + " | ");
            
        }

        cliDisplay += kBoundariesInBetween;

        // calculate the sum of these metrics

        cliDisplay += kUpperAndDownTableFormat;
    }
}



}
}