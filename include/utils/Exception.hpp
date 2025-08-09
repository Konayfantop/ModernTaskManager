#pragma once

#include <cstring>
#include <exception>
#include <string>
namespace utils
{

// pseudo CRTP , using template tag for tagging in the code and make the dev understand the severity of the errors
// Harmless -> exception throwing is part of the plan
// Serious -> exception that is enough to disrupt activities
// Moderate -> exception which indicates the skipping of a given operation
template<class Derived>
class SeverityException : public std::exception
{
public:
    SeverityException(std::string&& msg) { _msg = std::move(msg); }

    const char* what() const noexcept override 
    {
        return _msg.c_str();
    }

private:
    std::string _msg;
};

class HarmlessException : public SeverityException<HarmlessException> { };
class SeriousException : public SeverityException<SeriousException> { };
class ModerateException : public SeverityException<ModerateException> { };
}