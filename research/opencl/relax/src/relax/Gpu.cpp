#include "Gpu.hpp"
#include <complex> // Move to rssd/Core.h
#include <rssd/Core.h>
#include "rssdcl/ClManager.h"

using namespace std;
using namespace rssd;
using namespace rssd::gpgpu;
using namespace rssd::relax;
using namespace rssd::relax::gpu;

///
/// Functions
///

int rssd::relax::gpu::gpu_main(const size_t num_markers)
{
  try
  {
    // Local vars
    static const char* CL_FILENAME = "/Users/arris/Code/rssd/research/opencl/relax/src/relax/Gpu.cl";
    // static const char* INTER_LABEL_DISTANCE_CALCULATOR_KERNEL = "calculateInterLabelDistances";
    static const char* INTER_LABEL_DISTANCE_CALCULATOR_KERNEL = "test";

    // Create singletons
    new TimerManager();
    new ClManager();

    // Create high-resolution timer
    PosixTimer *timer = dynamic_cast<PosixTimer*>(TimerManager::getPointer()->create(
        PosixTimerFactory::getPointer()->getType()));

    // Configure random number generator
    rng::MAX_RANGE = 1000.0f;
    rng::MIN_RANGE = -1000.0f;

    // Local vars
    marker_v
      objects,
      labels,
      reference_labels;

    // Generate test data
    BEGIN_TRACE("GENERATE DATA", timer);
    {
      if (!generateData(
        objects,
        labels,
        reference_labels,
        num_markers))
        return -1;
    }
    END_TRACE("GENERATE DATA", timer);

    // Compile OpenCL kernels
    ClManager::getPointer()->compile(CL_FILENAME);

    // Get inter-label calculator kernel
    ClKernel *kernel = ClManager::getPointer()->getKernel(INTER_LABEL_DISTANCE_CALCULATOR_KERNEL);
    uint32_t labels_buffer_id = kernel->createBuffer(
      num_markers*4*4,  /// @note Buffer size in bytes.
      CL_MEM_READ_ONLY | CL_MEM_ALLOC_HOST_PTR);
    uint32_t distances_buffer_id = kernel->createBuffer(
      num_markers*4*4,  /// @note Buffer size in bytes.
      CL_MEM_WRITE_ONLY);

    // Calculate inter-label distances
    BEGIN_TRACE("CALCULATE LABEL DISTANCES", timer);
    {
      // Populate the labels buffer
      marker_t::extract(
        labels,
        marker_t::property::POSITION,
        kernel->getBuffer(labels_buffer_id));

      // Bind buffers to kernel
      uint32_t local_size = 1;
      uint32_t global_size = num_markers;
      kernel->sendInput(
        local_size,
        global_size);

      // Execute kernel
      cl::Event kernel_event = kernel->getFunctor()(
        kernel->getBuffer(labels_buffer_id)->getCl(),
        kernel->getBuffer(distances_buffer_id)->getCl());
      kernel_event.wait();

      // Read results
      kernel->receiveOutput();
    }
    END_TRACE("CALCULATE LABEL DISTANCES", timer);

    // Write input for display
    kernel->getBuffer(distances_buffer_id)->print<float>();

    // Destroy singletons
    delete ClManager::getPointer();
    delete TimerManager::getPointer();
  }
  catch (const cl::Error &error)
  {
    cout << "OpenCL Error" << endl;
    cout << "\tWhat: " << error.what() << endl;
    cout << "\tInfo: " << error.err() << endl;
  }
  catch (std::exception &error)
  {
    cout << "Standard Error" << endl;
    cout << "\tWhat: " << error.what() << endl;
  }
  return 0;
}
