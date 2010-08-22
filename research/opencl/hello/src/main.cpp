#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <OpenCL/cl.hpp>
#include "PrintManager.h"
#include "ClManager.h"

using namespace rssd;
using namespace rssd::gpgpu;

void printTitle()
{
    std::cout << '\n' << PrintManager::format(PrintManager::Category::TITLE, "OPENCL DETECT") << std::endl;
}

double profileKernel(cl::Event &event)
{
    // All times are in nanoseconds
    // cl_ulong queue = event.getProfilingInfo<CL_PROFILING_COMMAND_QUEUED>();
    // cl_ulong submit = event.getProfilingInfo<CL_PROFILING_COMMAND_SUBMIT>();
    cl_ulong start = event.getProfilingInfo<CL_PROFILING_COMMAND_START>();
    cl_ulong end = event.getProfilingInfo<CL_PROFILING_COMMAND_END>();
    return ((1.0e-9) * (end - start));
}

double executeKernel(
    ClManager &cl_manager,
    cl::KernelFunctor &functor,
    const size_t length)
{
    // Create test input
    cl_int
        as[length],
        bs[length],
        cs[length];

    // Initialize CL test input
    for (cl_uint i = 0; i < length; ++i)
    {
        as[i] = i;
        bs[i] = length - i;
    }

    // Create CL data buffers
    cl::Buffer
        c_buffer(cl_manager.getContext(), CL_MEM_READ_ONLY, length * sizeof(cl_int)),
        b_buffer(cl_manager.getContext(), CL_MEM_READ_ONLY, length * sizeof(cl_int)),
        a_buffer(cl_manager.getContext(), CL_MEM_WRITE_ONLY, length * sizeof(cl_int));

    // Copy test input to the CL device
    cl_manager.getCommandQueue().enqueueWriteBuffer(
        b_buffer, CL_TRUE, 0, length * sizeof(cl_float), bs);
    cl_manager.getCommandQueue().enqueueWriteBuffer(
        c_buffer, CL_TRUE, 0, length * sizeof(cl_float), cs);
    cl_manager.getCommandQueue().enqueueReadBuffer(
        a_buffer, CL_TRUE, 0, length * sizeof(cl_float), as);

    // Invoke functor
    cl::Event kernel_event = functor(
        a_buffer,
        b_buffer,
        c_buffer,
        length);

    // Wait for kernel completion
    kernel_event.wait();
    return profileKernel(kernel_event);
}

int main(int argc, char **argv)
{
    static const size_t GLOBAL_INDEX_SPACE = 1280;
    static const size_t WORK_GROUP_DIMENSIONS = 128;
    static const char* CL_FILENAME = "/Users/arris/Code/rssd/research/opencl/hello/src/main.cl";
    static const char* KERNEL_NAME = "add_vectors";

    int result = 0;
    ClManager cl_manager;

    try
    {
        printTitle();
        result = cl_manager.detect();
        cl_manager.autoSelectDevice();
        cl_manager.compile(CL_FILENAME);
#if 0
        cl::Kernel *add_vectors_kernel = cl_manager.getKernel(KERNEL_NAME);
        cl::KernelFunctor add_vectors_functor = add_vectors_kernel->bind(
            cl_manager.getCommandQueue(),
            cl::NDRange(GLOBAL_INDEX_SPACE),
            cl::NDRange(WORK_GROUP_DIMENSIONS));
        double elapsed = executeKernel(
            cl_manager,
            add_vectors_functor,
            GLOBAL_INDEX_SPACE);

        std::cout
            << "Executed kernel (\""
            << KERNEL_NAME << "\") in "
            << elapsed << " seconds."
            << std::endl;
#endif
    }
    catch (const cl::Error &error)
    {
        std::cout
            << "OpenCL Error "
            << "(" << error.err() << "): "
            << error.what()
            << std::endl;
    }
    return result;
}
