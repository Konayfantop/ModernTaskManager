#include "LogTrace.hpp"
#include <ProcessInfo.hpp>
#include <filesystem>
#include <gtest/gtest.h>
#include <Exception.hpp>
#include <unordered_map>

namespace proc
{

class ProcessInfoAccessor : public ProcessInfo
{
public:
    using ProcessInfo::getPidNum;
    using ProcessInfo::fillStatMap;
    using ProcessInfo::getGenericUptime;
    using ProcessInfo::calculateCpu;
    using ProcessInfo::refineDouble;
    using ProcessInfo::getMeminfo;
    using ProcessInfo::calculateMemory;
    using ProcessInfo::calculateProcessUptime;
    using ProcessInfo::exportInFile;
};

class ProcessInfoTest : public ::testing::Test
{
    void setUp() {}
    void TearDown() override{}
public:
    // This accessor actually initiates the path to /proc so think about it before removing it
    ProcessInfoAccessor processInfoAccessor;
    
    std::filesystem::path setTestingPath()
    {
        std::filesystem::path testPath = std::filesystem::path(processInfoAccessor.getOldPath());
        testPath = std::filesystem::path(testPath.parent_path() / "test/data/simulateProc/proc");
        return testPath;
    }
};

TEST_F(ProcessInfoTest, checkCtorOfAccessor_pathChanged_Ok)
{
    ASSERT_EQ(std::filesystem::path("/proc"), std::filesystem::current_path());
}

TEST_F(ProcessInfoTest, checkPidNum_return666_Ok)
{
    std::filesystem::current_path(setTestingPath());

    for(const std::filesystem::directory_entry& entry : std::filesystem::directory_iterator(std::filesystem::current_path()))
    {
        if(entry.is_directory())
        {
            try
            {
                ASSERT_EQ(666u, processInfoAccessor.getPidNum(entry));
            }
            catch(const utils::SeverityException<utils::HarmlessException>& e)
            {
                INFO("Part of the plan! Have no fear!");
            }
        }
    }
}

TEST_F(ProcessInfoTest, checkStatMap_filledUp_Ok)
{
    std::filesystem::current_path(setTestingPath());
    std::unordered_map<uint, std::string> statMap;
    
    ASSERT_NO_THROW(statMap = processInfoAccessor.fillStatMap(
        std::filesystem::path(std::filesystem::current_path() / "666" / "stat")));
    
    ASSERT_EQ(5u, statMap.size());
    ASSERT_EQ(1u, statMap.count(14u));
    ASSERT_EQ(1u, statMap.count(15u));
    ASSERT_EQ(1u, statMap.count(20u));
    ASSERT_EQ(1u, statMap.count(22u));
    ASSERT_EQ(1u, statMap.count(24u));

    ASSERT_EQ("125", statMap.at(14u));
    ASSERT_EQ("954", statMap.at(15u));
    ASSERT_EQ("3", statMap.at(20u));
    ASSERT_EQ("4685", statMap.at(22u));
    ASSERT_EQ("1696", statMap.at(24u));
}

TEST_F(ProcessInfoTest, checkStatMap_noStatMap_throwModerate)
{
    std::filesystem::current_path(setTestingPath());
    std::unordered_map<uint, std::string> statMap;
    
    ASSERT_THROW(statMap = processInfoAccessor.fillStatMap(
        std::filesystem::path(std::filesystem::current_path() / "fs(dummy_folder_to_simulate)")), utils::SeverityException<utils::ModerateException>);
}

TEST_F(ProcessInfoTest, checkGenericUptime_parsedOk)
{
    std::filesystem::current_path(setTestingPath());
    std::filesystem::path uptimePath(std::filesystem::current_path() / "uptime");
    double uptimeValue{0};

    ASSERT_NO_THROW(uptimeValue = processInfoAccessor.getGenericUptime(uptimePath));
    ASSERT_EQ(5689.13, uptimeValue);
}

TEST_F(ProcessInfoTest, checkGenericUptime_throwBecausePathIsWrong)
{
    std::filesystem::current_path(setTestingPath());
    std::filesystem::path uptimePath(std::filesystem::current_path() / "lol_this_is_wrong");
    double uptimeValue{0};

    ASSERT_THROW(processInfoAccessor.getGenericUptime(uptimePath), utils::SeverityException<utils::SeriousException>);
}

TEST_F(ProcessInfoTest, checkCalculateCpu_calculatedOk)
{
    std::filesystem::current_path(setTestingPath());

    // set up statMap and check outcome
    const std::unordered_map<uint, std::string> statMap = processInfoAccessor.fillStatMap(
        std::filesystem::path(std::filesystem::current_path() / "666" / "stat"));
    ASSERT_EQ(5u, statMap.size());
    ASSERT_EQ(1u, statMap.count(14u));
    ASSERT_EQ(1u, statMap.count(15u));
    ASSERT_EQ(1u, statMap.count(20u));
    ASSERT_EQ(1u, statMap.count(22u));
    ASSERT_EQ(1u, statMap.count(24u));
    ASSERT_EQ("125", statMap.at(14u));
    ASSERT_EQ("954", statMap.at(15u));
    ASSERT_EQ("3", statMap.at(20u));
    ASSERT_EQ("4685", statMap.at(22u));
    ASSERT_EQ("1696", statMap.at(24u));

    // set up generic uptime
    std::filesystem::current_path(setTestingPath());
    std::filesystem::path uptimePath(std::filesystem::current_path() / "uptime");
    double uptimeValue = uptimeValue = processInfoAccessor.getGenericUptime(uptimePath);
    ASSERT_EQ(5689.13, uptimeValue);

    const double cpuTime = processInfoAccessor.calculateCpu(statMap, uptimeValue);
    ASSERT_EQ(0.19123474907306975, cpuTime);
}

TEST_F(ProcessInfoTest, checkRefine_RefinedDoublesOk)
{
    ASSERT_EQ("0.19%", processInfoAccessor.refineDouble(0.19123474907306975));
    ASSERT_EQ("12.54%", processInfoAccessor.refineDouble(12.5436667854642456));
    ASSERT_EQ("0.0%", processInfoAccessor.refineDouble(0.0043));
}

TEST_F(ProcessInfoTest, checkMeminfo_parsedAndFetchedValueOk)
{
    std::filesystem::current_path(setTestingPath());
    double meminfo;

    ASSERT_NO_THROW(meminfo = processInfoAccessor.getMeminfo(std::filesystem::path(std::filesystem::current_path() / "meminfo")));
    EXPECT_EQ(8131976.0, meminfo);
}

TEST_F(ProcessInfoTest, checkMeminfo_ThrowSeriousErrorAsMemTotalInfoNotFound)
{
    std::filesystem::current_path(setTestingPath());
    double meminfo;

    ASSERT_THROW(meminfo = processInfoAccessor.getMeminfo(std::filesystem::path(std::filesystem::current_path() / "fs(dummy_folder_to_simulate)/meminfo")),
        utils::SeverityException<utils::SeriousException>);
}

TEST_F(ProcessInfoTest, checkCalculateMemory_memoryCalculatedOk)
{
    std::filesystem::current_path(setTestingPath());

    // fill up statMap but check only vital entries
    std::unordered_map<uint, std::string> statMap;
    ASSERT_NO_THROW(statMap = processInfoAccessor.fillStatMap(
        std::filesystem::path(std::filesystem::current_path() / "666" / "stat")));
    ASSERT_EQ(5u, statMap.size());
    ASSERT_EQ(1u, statMap.count(24u));
    ASSERT_EQ("1696", statMap.at(24u));

    // get the generic uptime
    std::filesystem::current_path(setTestingPath());
    std::filesystem::path meminfopath(std::filesystem::current_path() / "meminfo");
    double meminfo{0};
    ASSERT_NO_THROW(meminfo = processInfoAccessor.getMeminfo(meminfopath));
    ASSERT_EQ(8131976.0, meminfo);
    
    double memory;
    ASSERT_NO_THROW(memory = processInfoAccessor.calculateMemory(statMap, meminfo));

    INFO(static_cast<double>(sysconf(_SC_PAGESIZE)));
    ASSERT_EQ(0.08342375825998502, memory);
}

TEST_F(ProcessInfoTest, checkCalculateProcessUptime_calculatedOk)
{
    std::filesystem::current_path(setTestingPath());

    // fill up statMap but check only vital entries
    std::unordered_map<uint, std::string> statMap;
    ASSERT_NO_THROW(statMap = processInfoAccessor.fillStatMap(
        std::filesystem::path(std::filesystem::current_path() / "666" / "stat")));
    ASSERT_EQ(5u, statMap.size());
    ASSERT_EQ(1u, statMap.count(22u));
    ASSERT_EQ("4685", statMap.at(22u));

    // set up generic uptime
    std::filesystem::current_path(setTestingPath());
    std::filesystem::path uptimePath(std::filesystem::current_path() / "uptime");
    double uptimeValue = uptimeValue = processInfoAccessor.getGenericUptime(uptimePath);
    ASSERT_EQ(5689.13, uptimeValue);

    proc::PidStats::timezone timezone = processInfoAccessor.calculateProcessUptime(statMap, uptimeValue);

    EXPECT_EQ(1, timezone._hours);
    EXPECT_EQ(34, timezone._minutes);
    EXPECT_EQ(2, timezone._seconds);
    EXPECT_EQ(280, timezone._ms);
}

TEST_F(ProcessInfoTest, checkTheWholeProcess_readAndDisplayProcDir_outcomeOk)
{
    std::filesystem::current_path(setTestingPath());
    processInfoAccessor.readAndDisplayProcDir();
}

}