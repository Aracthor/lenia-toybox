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

    Algorithm::Type algorithm = Algorithm::Lenia;
    int range = 15;
};

Config parse_command_line(int argc, char** argv);
