#include "gtest/gtest.h"
#include <filesystem>
#include "Validator.hpp"
#include "LogTrace.hpp"
#include <sys/resource.h>

namespace utils
{
namespace validator
{

class ValidatorTest : public ::testing::Test
{};

TEST_F(ValidatorTest, checkCorruptedFileWithNegativeCpu_CaptureOutputStreamAndRejectValidation)
{
    testing::internal::CaptureStdout();

    const std::filesystem::path& currentTestInput = 
        std::filesystem::path(std::filesystem::current_path().parent_path() / "test/data/validationInput/errorLineCpu.txt");

    const bool outcome = utils::validator::validateExportedFile(currentTestInput);

    std::string logOutput = testing::internal::GetCapturedStdout();

    EXPECT_FALSE(outcome);
    EXPECT_NE(logOutput.find("System entity metric validator for : cpu, has failed. Line wasn't verified. Continue..."), std::string::npos);
}

TEST_F(ValidatorTest, checkCorruptedFileWithNegativeMemory_CaptureOutputStreamAndRejectValidation)
{
    testing::internal::CaptureStdout();

    const std::filesystem::path& currentTestInput = 
        std::filesystem::path(std::filesystem::current_path().parent_path() / "test/data/validationInput/errorLineMemory.txt");

    const bool outcome = utils::validator::validateExportedFile(currentTestInput);

    std::string logOutput = testing::internal::GetCapturedStdout();

    EXPECT_FALSE(outcome);
    EXPECT_NE(logOutput.find("System entity metric validator for : memory, has failed. Line wasn't verified. Continue..."), std::string::npos);
}

TEST_F(ValidatorTest, checkCorruptedFileWithManyThreads_RejectValidation)
{
    testing::internal::CaptureStdout();

    const std::filesystem::path& currentTestInput = 
        std::filesystem::path(std::filesystem::current_path().parent_path() / "test/data/validationInput/errorLineThreads.txt");

    const bool outcome = utils::validator::validateExportedFile(currentTestInput);

    std::string logOutput = testing::internal::GetCapturedStdout();

    EXPECT_FALSE(outcome);
    EXPECT_NE(logOutput.find("System entity metric validator for : threads, has failed. Line wasn't verified. Continue..."), std::string::npos);
}

TEST_F(ValidatorTest, checkCorruptedFileWithTimeZero_CaptureOutputStreamAndRejectValidation)
{
    testing::internal::CaptureStdout();

    const std::filesystem::path& currentTestInput = 
        std::filesystem::path(std::filesystem::current_path().parent_path() / "test/data/validationInput/errorLineTime.txt");

    const bool outcome = utils::validator::validateExportedFile(currentTestInput);

    std::string logOutput = testing::internal::GetCapturedStdout();

    EXPECT_FALSE(outcome);
    EXPECT_NE(logOutput.find("Impossible that the elapsed time of an active-process is zero. Continue..."), std::string::npos);
}

TEST_F(ValidatorTest, checkProperFileWithTimeZero_ValidationSuccess)
{
    const std::filesystem::path& currentTestInput = 
        std::filesystem::path(std::filesystem::current_path().parent_path() / "test/data/validationInput/correctLine.txt");

    const bool outcome = utils::validator::validateExportedFile(currentTestInput);

    EXPECT_TRUE(outcome);
}

}
}