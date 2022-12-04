#include <iostream>
#include <stdexcept>
#include <cstdlib>

#include "Application.h"

int main()
{

    Mango::MangoApplication application;
    try
    {
        application.Run();
    } catch (const std::exception &exception)
    {
        std::cerr << exception.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
