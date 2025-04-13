#pragma once

#include <string>
#include <vector>

namespace Algorithm
{
enum Type
{
    LargerThanLife,
    Primordia,
    Lenia,
};

Type FromName(const std::string& name);
} // namespace Algorithm

struct Config
{
    bool run = true;

    int width = 800;
    int height = 600;
    int framerate = 60;
    std::string startupFileName;
    bool pause = false;
    bool profile = false;

    Algorithm::Type algorithm = Algorithm::Lenia;
    int range = 15;
    float timestamp = 10.f;

    // larger-than-life and primordia
    float survivalRangeMin = 2.f;
    float survivalRangeMax = 3.f;
    float birthRangeMin = 3.f;
    float birthRangeMax = 3.f;

    // lenia
    float kernelGaussCenter = 0.5f;
    float kernelGaussWidth = 0.15f;
    struct Kernel
    {
        float growthGaussCenter = 0.135f;
        float growthGaussWidth = 0.014f;
        std::vector<float> ringWeights = {1.f};
    };
    int kernelCount = 1;
    Kernel kernels[3];
};

Config parse_command_line(int argc, char** argv);
