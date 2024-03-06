#pragma once

namespace Algorithm
{
enum Type
{
    LargerThanLife,
    Primordia,
    Lenia,
};
} // namespace Algorithm

struct Config
{
    int width = 800;
    int height = 600;
    int framerate = 60;
    const char* startupFileName = nullptr;
    bool pause = false;

    Algorithm::Type algorithm = Algorithm::Lenia;
    int range = 15;
    float survivalRangeMin = 2.f;
    float survivalRangeMax = 3.f;
    float birthRangeMin = 3.f;
    float birthRangeMax = 3.f;
};

Config parse_command_line(int argc, char** argv);
