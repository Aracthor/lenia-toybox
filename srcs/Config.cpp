#include "Config.hpp"

#include <algorithm>
#include <memory>
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

class IParameterProcessor
{
public:
    virtual ~IParameterProcessor() = default;

    virtual void Process(ArgIterator& argIterator) = 0;
};

class IntegerProcessor : public IParameterProcessor
{
public:
    IntegerProcessor(int& data)
        : m_data(data)
    {
    }

    void Process(ArgIterator& argIterator) override
    {
        const char* nextArg = *argIterator;
        argIterator++;
        if (!isnumber(nextArg))
            throw std::invalid_argument(std::string("Expected number, but got: ") + nextArg);
        m_data = std::atoi(nextArg);
    }

private:
    int& m_data;
};

class StringProcessor : public IParameterProcessor
{
public:
    StringProcessor(const char*& data)
        : m_data(data)
    {
    }

    void Process(ArgIterator& argIterator) override
    {
        const char* nextArg = *argIterator;
        argIterator++;
        m_data = nextArg;
    }

private:
    const char*& m_data;
};

class ConfigParameter
{
public:
    ConfigParameter(const char* flag, IParameterProcessor* processor)
        : m_flag(flag)
        , m_processor(processor)
    {
    }

    IParameterProcessor* Processor() { return m_processor.get(); }
    bool Matches(const std::string& arg) const { return m_flag == arg; }

private:
    const std::string m_flag;
    std::unique_ptr<IParameterProcessor> m_processor;
};

} // namespace

Config parse_command_line(int argc, char** argv)
{
    Config config;

    ArgIterator iterator(argc, argv);
    ConfigParameter parameters[] = {
        {"-w", new IntegerProcessor(config.width)},
        {"-h", new IntegerProcessor(config.height)},
        {"-f", new IntegerProcessor(config.framerate)},
        {"-s", new StringProcessor(config.startupFileName)},
    };

    while (iterator)
    {
        const std::string arg = *iterator;
        iterator++;
        auto IsFlag = [arg](const ConfigParameter& parameter) { return parameter.Matches(arg); };
        auto it = std::find_if(std::begin(parameters), std::end(parameters), IsFlag);
        if (it == std::end(parameters))
            throw std::invalid_argument(std::string("Unknown argument ") + arg);
        it->Processor()->Process(iterator);
    }

    return config;
}
