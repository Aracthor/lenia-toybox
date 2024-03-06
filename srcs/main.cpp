#include "Application.hpp"

#include "Config.hpp"

#include <cstdlib>
#include <exception>
#include <iostream>

int main(int argc, char** argv)
{
    try
    {
        srandom(std::time(nullptr));
        const Config config = parse_command_line(argc, argv);
        Application app(config);
        return app.Run();
    }
    catch (const std::exception& exception)
    {
        std::cerr << "Fatal error: " << exception.what() << std::endl;
        return 2;
    }
}
