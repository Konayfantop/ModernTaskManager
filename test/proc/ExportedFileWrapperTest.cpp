#include <filesystem>
#include <gtest/gtest.h>
#include <ExportedFileWrapper.hpp>

namespace proc
{

class ExportedFileWrapperTest : public ::testing::Test
{
public:
    std::filesystem::path setTestingPath()
    {
        std::filesystem::path testPath = std::filesystem::current_path();
        testPath = std::filesystem::path(testPath.parent_path() / "test/data/ExportedFileWrapper");

        return testPath;
    }
};

TEST_F(ExportedFileWrapperTest, checkCtor_mapPopulatedOk)
{
    std::filesystem::path exportedFilePath = std::filesystem::path(setTestingPath() / "dummyExportedFile.txt");
    ExportedFileWrapper wrapper(exportedFilePath);

    ASSERT_EQ(11u, wrapper.getPids().size());
}

TEST_F(ExportedFileWrapperTest, checkRetrievalByStep_mapPopulatedOk)
{
    std::filesystem::path exportedFilePath = std::filesystem::path(setTestingPath() / "dummyExportedFivePids.txt");
    ExportedFileWrapper wrapper(exportedFilePath);

    ASSERT_EQ(5u, wrapper.getPids().size());
    ASSERT_EQ(5u, wrapper.getPidsByStep(5).size());
}

TEST_F(ExportedFileWrapperTest, checkRetrievalByStepOne_mapPopulatedOk)
{
    std::filesystem::path exportedFilePath = std::filesystem::path(setTestingPath() / "dummyExportedFileOnePid.txt");
    ExportedFileWrapper wrapper(exportedFilePath);

    ASSERT_EQ(1u, wrapper.getPids().size());
    ASSERT_EQ(1u, wrapper.getPidsByStep(1).size());
    ASSERT_TRUE(wrapper.isIterPointingEnd());
    wrapper.resetIter();
    ASSERT_EQ(14500u, wrapper.getCurrentIter()->first);
}

TEST_F(ExportedFileWrapperTest, checkRetrievalByStep3_mapPopulatedOk)
{
    std::filesystem::path exportedFilePath = std::filesystem::path(setTestingPath() / "dummyExportedFivePids.txt");
    ExportedFileWrapper wrapper(exportedFilePath);

    ASSERT_EQ(5u, wrapper.getPids().size());
    ASSERT_EQ(3u, wrapper.getPidsByStep(3).size());
    ASSERT_FALSE(wrapper.isIterPointingEnd());
    ASSERT_EQ(15345u, wrapper.getCurrentIter()->first);
}

TEST_F(ExportedFileWrapperTest, checkRetrievalByStep8MoreThanShould_mapPopulatedOk)
{
    std::filesystem::path exportedFilePath = std::filesystem::path(setTestingPath() / "dummyExportedFivePids.txt");
    ExportedFileWrapper wrapper(exportedFilePath);

    ASSERT_EQ(5u, wrapper.getPids().size());
    ASSERT_EQ(5u, wrapper.getPidsByStep(8).size());
    ASSERT_TRUE(wrapper.isIterPointingEnd());
    wrapper.resetIter();
    ASSERT_EQ(14270u, wrapper.getCurrentIter()->first);
}

}