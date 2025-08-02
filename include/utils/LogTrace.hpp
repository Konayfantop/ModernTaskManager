#pragma once

#include <iostream>

static constexpr char ANSI_START[] = "\033[";
static constexpr char RED[] = "1;31m";
static constexpr char GREEN[] = "1;32m";
static constexpr char YELLOW[] = "1;33m";
static constexpr char MAGENTA[] = "1;35m";
static constexpr char CYAN[] = "1;36m";
static constexpr char RESET[] = "0m";

namespace
{
#define __LOGGING__SHORTEN__LOCATION__(path) \
{ \
    std::string str(path); \
    std::size_t lastSlash = str.find_last_of("/\\"); \
    if(lastSlash != std::string::npos) \
    { \
        str = str.substr(lastSlash+1); \
    } \
    std::cout << ANSI_START << MAGENTA << str << "#" << __LINE__ << ": "; \
}

#define __LOGGING__(log, color) \
{ \
    __LOGGING__SHORTEN__LOCATION__(__FILE__); \
    std::cout <<  ANSI_START << color << log << ANSI_START << RESET << std::endl; \
}

#define __LOGGING__UNIFIED(log, color) \
{ \
    __LOGGING__SHORTEN__LOCATION__(__FILE__) \
    std::cout << ANSI_START << color << log << ANSI_START << RESET << std::endl; \
}
}

#define WARNING(log) \
    __LOGGING__(log, YELLOW);

#define ERROR(log) \
    __LOGGING__(log, RED);

#define INFO(log) \
    __LOGGING__(log, MAGENTA);

#define INFO_LOG_UNIFIED(log) \
    __LOGGING__UNIFIED(log, MAGENTA);

#define NOTIFY(log) \
    __LOGGING__(log, CYAN);

namespace utils
{

}