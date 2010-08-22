#ifndef RSSD_RESEARCH_OPENCL_DETECT_CLMANAGER_H
#define RSSD_RESEARCH_OPENCL_DETECT_CLMANAGER_H

#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <fstream>
#include <OpenCL/cl.hpp>
#include "ClDeviceInfo.h"
#include "ClPlatformInfo.h"

namespace rssd {
namespace gpgpu {

class ClManager
{
public:
    struct ErrorCode
    {
        enum Type
        {
            SUCCESS = 0,
            FAILED_TO_DETECT_PLATFORM,
            FAILED_TO_DETECT_DEVICE,
            COUNT
        }; // enum Type
    }; // struct ErrorCode

public:
    typedef cl::vector<cl::Platform> ClPlatform_v;
    typedef cl::vector<cl::Device> ClDevice_v;
    typedef cl::vector<cl::Kernel> ClKernel_v;
    typedef cl::vector<ClPlatformInfo> ClPlatformInfo_v;
    typedef cl::vector<ClDeviceInfo> ClDeviceInfo_v;

public:
    ClManager();
    ~ClManager();

public:
    cl::Context& getContext() { return this->_context; }
    cl::CommandQueue& getCommandQueue() { return this->_command_queue; }
    cl::Program& getProgram() { return this->_program; }

public:
    cl_int detect();
    cl_int autoSelectDevice(const bool enable_profiling = true);
    cl_int compile(const std::string &filename);
    cl_int compile(std::ifstream &source);
    cl::Kernel* getKernel(const std::string &name);
    std::string toString() const;

protected:
    cl_int getPlatforms(const bool print = true);
    cl_int getDevices(const bool print = true);

protected:
    ClPlatform_v _platforms;
    ClDevice_v _devices;
    ClKernel_v _kernels;
    cl::Context _context;
    cl::CommandQueue _command_queue;
    cl::Program _program;
}; // class ClManager

} // namespace gpgpu
} // namespace rssd

#endif // RSSD_RESEARCH_OPENCL_DETECT_CLMANAGER_H
