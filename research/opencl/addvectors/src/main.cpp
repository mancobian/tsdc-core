#include <utility>
#include <iostream>
#include <fstream>
#include <unistd.h>
#include <cassert>
#include <iostream>
#include <fstream>
#include <OpenCL/cl.hpp>
#include "ClManager.h"
#include "PrintManager.h"

using namespace rssd;
using namespace rssd::gpgpu;

void printTitle()
{
  std::cout << '\n' << PrintManager::format(PrintManager::Category::TITLE, "OPENCL DETECT") << std::endl;
}

int main(/* int argc, char **argv */)
{
  static const char* KERNEL_NAME = "vector_add";
  static const cl_uint GLOBAL_INDEX_DIMENSIONS = 12800000;
  static const cl_uint WORK_GROUP_DIMENSIONS = 128; /// @note Must be <= 512 on nVidia cards(?)

  // Create the two input vectors
  int *A = new int[GLOBAL_INDEX_DIMENSIONS];
  int *B = new int[GLOBAL_INDEX_DIMENSIONS];
  for(cl_uint i = 0; i < GLOBAL_INDEX_DIMENSIONS; i++)
  {
    A[i] = i;
    B[i] = GLOBAL_INDEX_DIMENSIONS - i;
  }

  try
  {
    printTitle();

    // Create OpenCL manager
    ClManager cl_manager;
    cl_manager.compile("main.cl");
    cl::Kernel kernel = *cl_manager.getKernel(KERNEL_NAME);

    // Create memory buffers
    /// @todo Find a way to handle buffers/params for OpenCL kernels via ClManager.
    cl::Buffer bufferA = cl::Buffer(
        cl_manager.getContext(),
        CL_MEM_READ_ONLY,
        GLOBAL_INDEX_DIMENSIONS * sizeof(cl_uint));
    cl::Buffer bufferB = cl::Buffer(
        cl_manager.getContext(),
        CL_MEM_READ_ONLY,
        GLOBAL_INDEX_DIMENSIONS * sizeof(cl_uint));
    cl::Buffer bufferC = cl::Buffer(
        cl_manager.getContext(),
        CL_MEM_WRITE_ONLY,
        GLOBAL_INDEX_DIMENSIONS * sizeof(cl_uint));

    // Copy lists A and B to the memory buffers
    cl_manager.getCommandQueue().enqueueWriteBuffer(
        bufferA,
        CL_TRUE,
        0,
        GLOBAL_INDEX_DIMENSIONS * sizeof(cl_uint),
        A);
    cl_manager.getCommandQueue().enqueueWriteBuffer(
        bufferB,
        CL_TRUE,
        0,
        GLOBAL_INDEX_DIMENSIONS * sizeof(cl_uint),
        B);

    // Run the kernel on specific ND range
    cl::NDRange global(GLOBAL_INDEX_DIMENSIONS);
    cl::NDRange local(WORK_GROUP_DIMENSIONS);
    cl::KernelFunctor kfunc = kernel.bind(cl_manager.getCommandQueue(), global, local);
    cl::Event kevent = kfunc(bufferA, bufferB, bufferC);
    kevent.wait();

#if 0
    // Read buffer C into a local list
    int *C = new int[GLOBAL_INDEX_DIMENSIONS];
    cl_manager.getCommandQueue().enqueueReadBuffer(
        bufferC,
        CL_TRUE,
        0,
        GLOBAL_INDEX_DIMENSIONS * sizeof(int),
        C);

    for(int i = 0; i < GLOBAL_INDEX_DIMENSIONS; i ++)
      std::cout << A[i] << " + " << B[i] << " = " << C[i] << std::endl;

    delete[] C;
#endif
    delete[] A;
    delete[] B;

    std::cout
      << "Executed kernel (\""
      << KERNEL_NAME << "\") in "
      << cl_manager.profile(kevent) << " seconds."
      << std::endl;
  }
  catch(cl::Error error)
  {
    std::cout << error.what() << "(" << error.err() << ")" << std::endl;
  }
  return 0;
}
