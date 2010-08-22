#include <utility>
#include <iostream>
#include <fstream>
#include <unistd.h>
#include <cassert>
#include <iostream>
#include <fstream>
#include <OpenCL/cl.hpp>
#include "rssdcl/ClManager.h"
#include "rssdcl/PrintManager.h"
#include "relax/Shared.hpp"
#include "relax/Cpu.hpp"
#include "relax/Gpu.hpp"

using namespace rssd;
using namespace rssd::gpgpu;
using namespace rssd::relax;
using namespace rssd::relax::cpu;
using namespace rssd::relax::gpu;

int main(int argc, char **argv)
{
  bool
    is_cpu = false,
    is_gpu = false;
  size_t num_markers = 60;
  int result = 0;

  for (int i=1; i<argc; ++i)
  {
    if (strcmp(argv[i], "--cpu") == 0)
    {
      is_cpu = true;
    }
    else if (strcmp(argv[i], "--gpu") == 0)
    {
      is_gpu = true;
    }
    else if (strcmp(argv[i], "--markers") == 0)
    {
      num_markers = atoi(argv[++i]);
    }
  } // end for(...)
  std::cout << std::endl;

  if (is_cpu)
    if ((result = cpu_main(num_markers)) != 0)
      return result;
  if (is_gpu)
    if ((result = gpu_main(num_markers)) != 0)
      return result;
} // end main(...)

#if 0
void printTitle()
{
  std::cout << '\n' << PrintManager::format(PrintManager::Category::TITLE, "OPENCL DETECT") << std::endl;
}

int main(/* int argc, char **argv */)
{
  static const char* KERNEL_NAME = "vector_add";

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
    cl_manager.setWorkGroupSize(128);
    cl_manager.setGlobalRange(128000000);
    cl_manager.compile("Main.cl");
    cl::Kernel kernel = cl_manager.getKernel(KERNEL_NAME);
    assert (kernel);

    // Create memory buffers
    /// @todo Find a way to handle buffers/params for OpenCL kernels via ClManager.
    cl::Buffer bufferA = cl::Buffer(
        cl_manager.getContext(),
        CL_MEM_READ_ONLY,
        GLOBAL_INDEX_DIMENSIONS * sizeof(int));
    cl::Buffer bufferB = cl::Buffer(
        cl_manager.getContext(),
        CL_MEM_READ_ONLY,
        GLOBAL_INDEX_DIMENSIONS * sizeof(int));
    cl::Buffer bufferC = cl::Buffer(
        cl_manager.getContext(),
        CL_MEM_WRITE_ONLY,
        GLOBAL_INDEX_DIMENSIONS * sizeof(int));

    // Copy lists A and B to the memory buffers
    cl_manager.getCommandQueue().enqueueWriteBuffer(
        bufferA,
        CL_TRUE,
        0,
        cl_manager.getGlobalRange() * sizeof(int), A);
    cl_manager.getCommandQueue().enqueueWriteBuffer(
        bufferB,
        CL_TRUE,
        0,
        cl_manager.getGlobalRange() * sizeof(int), B);

    // Run the kernel on the specified ND range
    cl::KernelFunctor kfunc = kernel->bind(
        cl_manager.getCommandQueue(),
        cl::NDRange(cl_manager.getGlobalRange()),
        cl::NDRange(cl_manager.getWorkGroupSize()));
    cl::Event kevent = kfunc(bufferA, bufferB, bufferC);
    kevent.wait();

#if 0
    // Read buffer C into a local list
    int *C = new int[cl_manager.getGlobalRange()];
    cl_manager.getCommandQueue().enqueueReadBuffer(bufferC, CL_TRUE, 0, cl_manager.getGlobalRange()* sizeof(int), C);
#if 0
    for(int i = 0; i < cl_manager.getGlobalRange(); i ++)
      std::cout << A[i] << " + " << B[i] << " = " << C[i] << std::endl;
#endif
    delete[] C;
#endif

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
#endif
