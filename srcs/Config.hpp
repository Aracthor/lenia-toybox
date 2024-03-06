#pragma once

struct Config
{
    int width = 800;
    int height = 600;
    int framerate = 60;
};

Config parse_command_line(int argc, char** argv);
