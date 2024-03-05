#include "Application.hpp"

#include <cstdlib>

int main()
{
    srandom(std::time(nullptr));
    Application app;
    return app.Run();
}
