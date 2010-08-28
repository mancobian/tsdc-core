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
    static const char* CALCULATE_MARKER_DISTANCES_KERNEL = "calculateMarkerDistances1d";
    static const char* CALCULATE_MARKER_PAIR_COMPATIBILITIES_KERNEL = "calculateMarkerPairCompatibilties";
    static const char* RELAX_MAIN_KERNEL = "relaxMain";
    uint32_t param_index = 0;
    marker_v
      objects,
      labels,
      reference_labels;
    ClBuffer_l
      local_buffers,
      global_buffers;

    // Create singletons
    new TimerManager();
    new ClManager();

    // Create high-resolution timer
    PosixTimer *timer = dynamic_cast<PosixTimer*>(TimerManager::getPointer()->create(
        PosixTimerFactory::getPointer()->getType()));

    // Configure random number generator
    rng::MAX_RANGE = 1000.0f;
    rng::MIN_RANGE = -1000.0f;

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

    // Create label distance input buffers
    createMarkerDistances(
      CALCULATE_MARKER_DISTANCES_KERNEL,
      labels,
      timer);

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

cl_uint rssd::relax::gpu::createMarkerDistances(
  const std::string &kernel_name,
  marker_v &markers,
  Timer *timer)
{
  // Local vars
  const uint32_t NUM_MARKERS = markers.size();
  uint32_t param_index = 0;
  cl_uint result = CL_SUCCESS;

  // Get kernel
  std::cout << "Kernel: " << kernel_name << std::endl;
  ClKernel *kernel = ClManager::getPointer()->getKernel(kernel_name);

  // Create buffers
  uint32_t markers_buffer_id = kernel->createBuffer<cl_float>(
    param_index++,
    ClBuffer::Type::MEMORY,
    NUM_MARKERS * 4,
    CL_MEM_READ_ONLY/* | CL_MEM_ALLOC_HOST_PTR*/);

  uint32_t differences_buffer_id = kernel->createBuffer<cl_float>(
    param_index++,
    ClBuffer::Type::MEMORY,
    ((NUM_MARKERS * NUM_MARKERS) * 4),
    CL_MEM_WRITE_ONLY);

  uint32_t distances_buffer_id = kernel->createBuffer<cl_float>(
    param_index++,
    ClBuffer::Type::MEMORY,
    NUM_MARKERS * NUM_MARKERS,
    CL_MEM_WRITE_ONLY);

  // Create scratch space (__local) buffer(s)
  /*uint32_t cached_buffers_id = */kernel->createBuffer<cl_float>(
    param_index++,
    ClBuffer::Type::LOCAL,
    NUM_MARKERS * 4);

  // Populate the labels buffer
  ClBuffer *markers_buffer = kernel->getBuffer(markers_buffer_id);
  marker_t::extract(
    markers,
    marker_t::property::POSITION,
    markers_buffer);

  // Calculate inter-label distances
  std::cout << "Local size: " << markers.size() / 2 << std::endl;
  std::cout << "Global size: " << markers.size() << std::endl;
  BEGIN_TRACE("CALCULATE LABEL DISTANCES", timer);
  {
    // Bind buffers to kernel
    cl::NDRange local_size(markers.size()/2);
    cl::NDRange global_size(markers.size());
    kernel->sendInput(
      local_size,
      global_size);

    // Execute kernel
    cl::Event kernel_event = kernel->getFunctor()(
      kernel->getBuffer(markers_buffer_id)->getCl(),
      kernel->getBuffer(differences_buffer_id)->getCl(),
      kernel->getBuffer(distances_buffer_id)->getCl());
    kernel_event.wait();

    // Read results
    kernel->receiveOutput();
  }
  END_TRACE("CALCULATE LABEL DISTANCES", timer);

  // Write distances for display
  kernel->getBuffer(distances_buffer_id)->print<cl_float>();

  // Write differences for display
  kernel->getBuffer(differences_buffer_id)->print<cl_float>();
  return result;
}

#if 0
cl_uint rssd::relax::gpu::createEntryPointBuffers(
  const std::string &kernel_name,
  const marker_v &labels,
  const marker_v &objects,
  ClBuffer_l &local_buffers,
  ClBuffer_l &global_buffers,
  uint32_t &param_index)
{
  // Local vars
  cl_uint result = CL_SUCCESS;

  // Get kernel
  ClKernel *kernel = ClManager::getPointer()->getKernel(kernel_name);

  // Create label distance input buffers
  createMarkerDistanceBuffers(
    CALCULATE_MARKER_DISTANCES_KERNEL,
    labels,
    param_index,
    local_buffers,
    global_buffers);

  // Create object distance input buffers
  createMarkerDistanceBuffers(
    CALCULATE_MARKER_DISTANCES_KERNEL,
    objects,
    param_index,
    local_buffers,
    global_buffers);

  // Create marker pair compatibility buffers
  createMarkerPairCompatibilityBuffers(
    CALCULATE_MARKER_PAIR_COMPATIBILITIES_KERNEL,
    labels,
    objects,
    param_index,
    local_buffers,
    global_buffers);

  // Store created buffers in out param
  local_buffers.push_back(kernel->getBuffer(cached_buffers_id));
  return result;
}

cl_uint createMarkerPairCompatibilityBuffers(
  const std::string &kernel_name,
  const marker_v &lhs_markers,
  const marker_v &rhs_markers,
  ClBuffer_l &local_buffers,
  ClBuffer_l &global_buffers,
  uint32_t &param_index)
{
  // Local vars
  const uint32_t NUM_RESULTS =
    (lhs_markers.size() * lhs_markers.size())
    * (rhs_markers.size() * rhs_markers.size());
  cl_uint result = CL_SUCCESS;

  // Get kernel
  ClKernel *kernel = ClManager::getPointer()->getKernel(kernel_name);

  // Create local buffer
  uint32_t results_buffer_id = kernel->createBuffer<cl_float>(
    param_index++,
    ClBuffer::Type::MEMORY,
    NUM_RESULTS,
    CL_MEM_READ_ONLY);


  // Store created buffers in out param
  global_buffers.push_back(kernel->getBuffer(results_buffer_id));
  return result;
}
#endif

