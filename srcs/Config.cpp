#include "Config.hpp"

#include <algorithm>
#include <stdexcept>
#include <string>

namespace
{

class ArgIterator
{
public:
    ArgIterator(int argc, char** argv)
        : m_argc(argc)
        , m_argv(argv)
    {
        (*this)++;
    }

    void operator++(int)
    {
        m_argc--;
        m_argv++;
    }

    const char* operator*() const
    {
        if (m_argc <= 0)
            throw std::logic_error(std::string("Missing argument after ") + *(m_argv - 1));
        return *m_argv;
    }

    operator bool() const { return m_argc > 0; }

private:
    int m_argc;
    char** m_argv;
};

bool isnumber(const char* str)
{
    if (*str == '-')
        str++;
    while (*str)
    {
        if (!std::isdigit(*str))
            return false;
        str++;
    }
    return true;
}

class ConfigParameter
{
public:
    ConfigParameter(const char* flag, int& data)
        : m_flag(flag)
        , m_data(data)
    {
    }

    void Process(ArgIterator& argIterator)
    {
        const char* nextArg = *argIterator;
        argIterator++;
        if (!isnumber(nextArg))
            throw std::invalid_argument(std::string("Expected number, but got: ") + nextArg);
        m_data = std::atoi(nextArg);
    }

    bool Matches(const std::string& arg) const { return m_flag == arg; }

private:
    const std::string m_flag;
    int& m_data;
};

} // namespace

Config parse_command_line(int argc, char** argv)
{
    Config config;

    ArgIterator iterator(argc, argv);
    ConfigParameter parameters[] = {
        {"-w", config.width},
        {"-h", config.height},
        {"-f", config.framerate},
    };

    while (iterator)
    {
        const std::string arg = *iterator;
        iterator++;
        auto IsFlag = [arg](const ConfigParameter& parameter) { return parameter.Matches(arg); };
        auto it = std::find_if(std::begin(parameters), std::end(parameters), IsFlag);
        if (it == std::end(parameters))
            throw std::invalid_argument(std::string("Unknown argument ") + arg);
        it->Process(iterator);
    }

    return config;
}
