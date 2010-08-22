#ifndef RSSD_GPGPU_CLMANAGER_H
#define RSSD_GPGPU_CLMANAGER_H

#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <fstream>
#include <map>
#include <OpenCL/cl.hpp>
#include <rssd/Core.h>
#include "rssdcl/ClBuffer.h"
#include "rssdcl/ClKernel.h"

namespace rssd {
namespace gpgpu {

///
/// @class ClManager
/// @todo Use device properties to configure optimal settings for cl::Programs.
/// @note Only threads in the same work-group can share data.
/// @note On nVidia hardware, work-groups are partitioned into 'warps', which are blocks of 32 threads.
/// @note Barriers force synchronization b/w threads in a work-group.
/// @note nVidia-Specific Notes:
///       @li Each GPU 'core' has 16KB of hardware-addressable local memory.
///       @li This local memory is organized into 4096 entries of 4 bytes/entry.
///       @li These entries are organized into 16 hardware-addressable 'banks' of memory.
///       @li This yields 256 entries/bank.
///       @li This hardware design allows for 16 simultaneous memory-accesses per cycle.
///       @li AVOID bank access conflicts!
///       @li Ideal memory-access pattern is for each thread in a half-warp to access one of these 16 banks.
///       @li You want each thread to read/load memory for one bank from __global to __local memory
///           in an aligned, coalesced memory access pattern. Thus the GPU device can transfer/load
///           memory in optimal-sized (4B * 16 banks = 64KB) chunks.
///       @li You also want each thread to write/store memory to each bank from __local to __global memory
///           in an aligned, coalesced memory access pattern.
///       @li TIP: Memory does NOT have to be aligned within __local memory, just in __global memory!
///           This fact is sometimes used to finesse data layout in attempts to prevent bank access conflicts.
///       @li TIP: Even if it will be unused, prefer to load data into shared memory if it will create
///           aligned, coalesced memory access.
///       @li TIP: Consider that input data will always be presented to the compute device as an array of
///           values. Anticipate that each thread will have read access to __global values within the range
///           [WORK_GROUP_ID * WORK_GROUP_SIZE..WORK_GROUP_SIZE) assuming an aligned, coalesced memory
///           access pattern.
///

class ClManager : public rssd::pattern::Singleton<ClManager>
{
public:
  typedef std::vector<cl::Platform> ClPlatform_v;
  typedef std::vector<cl::Device> ClDevice_v;

public:
  ClManager();
  ~ClManager();

public:
  inline ClPlatform_v& getPlatforms() { return this->_platforms; }
  inline ClDevice_v& getDevices() { return this->_devices; }
  inline cl::Program& getProgram() { return this->_program; }
  inline cl::Context& getContext() { return this->_context; }
  inline cl::CommandQueue& getCommandQueue() { return this->_command_queue; }
  inline ClKernel_v& getKernels() { return this->_kernels; }
  ClKernel* getKernel(const std::string &name);

public:
  cl_int compile(const std::string &filename);
  double profile(cl::Event &event);

protected:
  cl_int create();
  cl_int destroy();
  cl_int queryPlatforms();
  cl_int queryDevices();
  cl_int autoSelectDevice();
  cl_int createKernels(cl::Program &program);

protected:
  ClPlatform_v _platforms;
  ClDevice_v _devices;
  ClKernel_v _kernels;
  cl::Context _context;
  cl::CommandQueue _command_queue;
  cl::Program _program;
}; // class ClManager

///
/// Includes
///

// #include "ClManager-inl.h"

} // namespace gpgpu
} // namespace rssd

#endif // RSSD_GPGPU_CLMANAGER_H
