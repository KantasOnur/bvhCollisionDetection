#include "Game.h"
#include <iostream>
#include <cuda_runtime.h>
#include <device_launch_parameters.h>
#include <optix.h>
#include <optix_function_table_definition.h>
#include <optix_stubs.h>

extern "C" {
    _declspec(dllexport) DWORD NvOptimusEnablement = 1;
    _declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}

int main(void)
{

    if (optixInit() != OPTIX_SUCCESS)
    {
        std::cout << "failed to initialize optix" << std::endl;
        exit(1);
    }
    Game game;
    game.run();
    return 0;
}