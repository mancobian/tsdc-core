#include "PrintManager.h"
#include "ClManager.h"

///
/// Entry Point
///

cl_int cl_main()
{
    std::cout
        << '\n'
        << PrintManager::format(PrintManager::Category::TITLE, "OPENCL DETECT")
        << '\n';

    ClManager cl_manager;
    return cl_manager.detect();
}
