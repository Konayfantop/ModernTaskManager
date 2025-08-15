#pragma once

#include <filesystem>
#include <functional>

namespace utils
{
namespace validator
{
bool validateExportedFile(const std::filesystem::path& file);
}
}