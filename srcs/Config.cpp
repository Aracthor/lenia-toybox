#include "Config.hpp"

#include <algorithm>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>

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

bool isNumber(const char* str)
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

bool isDecimalNumber(const char* str)
{
    if (*str == '-')
        str++;
    int dots = 0;
    while (*str)
    {
        if (*str == '.')
        {
            dots++;
            if (dots > 1)
                return false;
        }
        else if (!std::isdigit(*str))
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

class BooleanProcessor : public IParameterProcessor
{
public:
    BooleanProcessor(bool& data)
        : m_data(data)
    {
    }

    void Process(ArgIterator& argIterator) override { m_data = true; }

private:
    bool& m_data;
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
        if (!isNumber(nextArg))
            throw std::invalid_argument(std::string("Expected number, but got: ") + nextArg);
        m_data = std::atoi(nextArg);
    }

private:
    int& m_data;
};

class FloatProcessor : public IParameterProcessor
{
public:
    FloatProcessor(float& data)
        : m_data(data)
    {
    }

    void Process(ArgIterator& argIterator) override
    {
        const char* nextArg = *argIterator;
        argIterator++;
        if (!isDecimalNumber(nextArg))
            throw std::invalid_argument(std::string("Expected decimal number, but got: ") + nextArg);
        m_data = std::atof(nextArg);
    }

private:
    float& m_data;
};

class KernelRingProcessor : public IParameterProcessor
{
public:
    KernelRingProcessor(Config::Kernel& kernel, size_t index)
        : m_kernel(kernel)
        , m_index(index)
    {
    }

    void Process(ArgIterator& argIterator) override
    {
        const char* nextArg = *argIterator;
        argIterator++;
        if (!isDecimalNumber(nextArg))
            throw std::invalid_argument(std::string("Expected decimal number, but got: ") + nextArg);
        m_kernel.SetWeight(m_index, std::atof(nextArg));
    }

private:
    Config::Kernel& m_kernel;
    const size_t m_index;
};

class StringProcessor : public IParameterProcessor
{
public:
    StringProcessor(std::string& data)
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
    std::string& m_data;
};

class AlgorithmProcessor : public IParameterProcessor
{
public:
    AlgorithmProcessor(Algorithm::Type& data)
        : m_data(data)
    {
    }

    void Process(ArgIterator& argIterator) override
    {
        const std::string nextArg = *argIterator;
        argIterator++;
        m_data = Algorithm::FromName(nextArg);
    }

private:
    Algorithm::Type& m_data;
};

class ConfigParameter
{
public:
    ConfigParameter(const char* flag, const char* name, IParameterProcessor* processor)
        : m_flag(flag)
        , m_name(name)
        , m_processor(processor)
    {
    }

    IParameterProcessor* Processor() { return m_processor.get(); }
    bool Matches(const std::string& arg) const { return m_flag == arg; }

    void DisplayHelpLine() const { std::cout << m_flag << ": " << m_name << std::endl; }

private:
    const std::string m_flag;
    const char* m_name;
    std::unique_ptr<IParameterProcessor> m_processor;
};

void DisplayHelp(const char* exePath, const ConfigParameter* parameters, int parametersCount)
{
    std::cout << "Usage: " << exePath << " [parameters]" << std::endl;
    std::cout << "Parameters list:" << std::endl;
    for (int i = 0; i < parametersCount; i++)
        parameters[i].DisplayHelpLine();
}

} // namespace

namespace Algorithm
{
Type FromName(const std::string& name)
{
    const std::unordered_map<std::string, Algorithm::Type> algorithms = {
        {"larger-than-life", Algorithm::LargerThanLife},
        {"primordia", Algorithm::Primordia},
        {"lenia", Algorithm::Lenia},
    };

    auto it = algorithms.find(name);
    if (it == algorithms.end())
        throw std::invalid_argument(std::string("Unknown algorithm: ") + name);
    return it->second;
}
} // namespace Algorithm

void Config::Kernel::SetWeight(size_t index, float weight)
{
    ringWeights[index] = weight;
}

void Config::Kernel::RemoveRing(size_t index)
{
    ringWeights[index] = {};
}

Config parse_command_line(int argc, char** argv)
{
    Config config;

    ArgIterator iterator(argc, argv);
    ConfigParameter parameters[] = {
        {"-w", "canvas width", new IntegerProcessor(config.width)},
        {"-h", "canvas height", new IntegerProcessor(config.height)},
        {"-f", "framerate", new IntegerProcessor(config.framerate)},
        {"-s", "startup filename", new StringProcessor(config.startupFileName)},
        {"-p", "pause at start", new BooleanProcessor(config.pause)},

        {"-a", "algorithm", new AlgorithmProcessor(config.algorithm)},
        {"-t", "timestamp", new FloatProcessor(config.timestamp)},
        {"-r", "kernel range", new IntegerProcessor(config.range)},
        {"--survival-min", "min value for survival", new FloatProcessor(config.survivalRangeMin)},
        {"--survival-max", "max value for survival", new FloatProcessor(config.survivalRangeMax)},
        {"--birth-min", "min value for birth", new FloatProcessor(config.birthRangeMin)},
        {"--birth-max", "max value for birth", new FloatProcessor(config.birthRangeMax)},
        {"--kernel-center", "Kernel gauss center", new FloatProcessor(config.kernelGaussCenter)},
        {"--kernel-width", "Kernel gauss width", new FloatProcessor(config.kernelGaussWidth)},
        {"--kernel-count", "Kernel count", new IntegerProcessor(config.kernelCount)},
        {"--growth-center-1", "Kernel growth center", new FloatProcessor(config.kernels[0].growthGaussCenter)},
        {"--growth-center-2", "Kernel growth center", new FloatProcessor(config.kernels[1].growthGaussCenter)},
        {"--growth-center-3", "Kernel growth center", new FloatProcessor(config.kernels[2].growthGaussCenter)},
        {"--growth-width-1", "Kernel growth width", new FloatProcessor(config.kernels[0].growthGaussWidth)},
        {"--growth-width-2", "Kernel growth width", new FloatProcessor(config.kernels[1].growthGaussWidth)},
        {"--growth-width-3", "Kernel growth width", new FloatProcessor(config.kernels[2].growthGaussWidth)},
        {"--ring-weight-1-1", "First Ring weight", new KernelRingProcessor(config.kernels[0], 0)},
        {"--ring-weight-1-2", "Second Ring weight", new KernelRingProcessor(config.kernels[0], 1)},
        {"--ring-weight-1-3", "Third Ring weight", new KernelRingProcessor(config.kernels[0], 2)},
        {"--ring-weight-2-1", "First Ring weight", new KernelRingProcessor(config.kernels[1], 0)},
        {"--ring-weight-2-2", "Second Ring weight", new KernelRingProcessor(config.kernels[1], 1)},
        {"--ring-weight-2-3", "Third Ring weight", new KernelRingProcessor(config.kernels[1], 2)},
        {"--ring-weight-3-1", "First Ring weight", new KernelRingProcessor(config.kernels[2], 0)},
        {"--ring-weight-3-2", "Second Ring weight", new KernelRingProcessor(config.kernels[2], 1)},
        {"--ring-weight-3-3", "Third Ring weight", new KernelRingProcessor(config.kernels[2], 2)},
    };

    while (iterator)
    {
        const std::string arg = *iterator;
        iterator++;
        if (arg == "--help")
        {
            DisplayHelp(argv[0], parameters, std::size(parameters));
            config.run = false;
        }
        else
        {
            auto IsFlag = [arg](const ConfigParameter& parameter) { return parameter.Matches(arg); };
            auto it = std::find_if(std::begin(parameters), std::end(parameters), IsFlag);
            if (it == std::end(parameters))
                throw std::invalid_argument(std::string("Unknown argument ") + arg);
            it->Processor()->Process(iterator);
        }
    }

    return config;
}
