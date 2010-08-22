#include "ClManager.h"
#include "PrintManager.h"

using namespace rssd;
using namespace rssd::gpgpu;

///
/// @class ClManager
///

ClManager::ClManager()
{
}

ClManager::~ClManager()
{
}

cl_int ClManager::detect()
{
    cl_int result = CL_SUCCESS;
    if ((result = this->getPlatforms()) != CL_SUCCESS)
        return result;
    if ((result = this->getDevices()) != CL_SUCCESS)
        return result;
    return result;
}

cl_int ClManager::getPlatforms(const bool print)
{
    // Clear out all old platforms
    this->_platforms.clear();

    // Get list of available OpenCL platforms
    cl_int result = cl::Platform::get(&this->_platforms);
    if ((result != CL_SUCCESS) || (this->_platforms.empty()))
        return result;

    // Create info object for each available CL platform
    ClPlatform_v::iterator
        iter = this->_platforms.begin(),
        end = this->_platforms.end();
    for (; iter != end; ++iter)
    {
        this->_platforms.push_back(*iter);
        if (print)
            std::cout << ClPlatformInfo(this->_platforms.back()).toString() << std::endl;
    }
    return CL_SUCCESS;
}

cl_int ClManager::getDevices(const bool print)
{
    // Local vars
    cl_int result = CL_SUCCESS;
    ClDevice_v devices;

    // Clear out all old devices
    this->_devices.clear();

    // Get and store info for every device on each CL platform
#if 0
    ClPlatform_v::iterator
        iter = this->_platforms.begin(),
        end = this->_platforms.end();
    for (; iter != end; ++iter)
    {
#endif
        result = this->_platforms.front().getDevices(
            CL_DEVICE_TYPE_ALL,
            &devices);

        // Create info object for each available CL platform
        ClDevice_v::iterator
            device_iter = devices.begin(),
            device_end = devices.end();
        for (; device_iter != device_end; ++device_iter)
        {
            this->_devices.push_back(*device_iter);
            if (print)
                std::cout << ClDeviceInfo(this->_devices.back()).toString() << std::endl;
        }
        devices.clear();
#if 0
    }
#endif
    return CL_SUCCESS;
}

cl_int ClManager::autoSelectDevice(const bool enable_profiling)
{
    cl_int result = CL_SUCCESS;

    // Create CL context request filter
    cl_context_properties cl_properties[3] =
    {
        CL_CONTEXT_PLATFORM,
        (cl_context_properties)(this->_platforms[0])(),
        0
    };

    // Create a CL context
    this->_context = cl::Context(
        CL_DEVICE_TYPE_GPU,
        cl_properties);

    // Get a list of CL devices
    this->_devices = this->_context.getInfo<CL_CONTEXT_DEVICES>();

    // Create a CL command queue using the default CL device
    this->_command_queue = cl::CommandQueue(
        this->_context,
        this->_devices.front());
        // CL_QUEUE_PROFILING_ENABLE);
    return result;
}

cl_int ClManager::compile(const std::string &filename)
{
    std::ifstream source(filename.c_str());
    cl_int result = this->compile(source);
    source.close();
    return result;
}

cl_int ClManager::compile(std::ifstream &source)
{
    // Local vars
    cl_int result = CL_SUCCESS;

    // Read in source code data
    std::string cl_code(
        std::istreambuf_iterator<char>(source),
        (std::istreambuf_iterator<char>()));

    // Create CL program source
    cl::Program::Sources cl_source(
        1, std::make_pair(cl_code.c_str(), cl_code.size() + 1));

    // Create program for the current CL context
    this->_program = cl::Program(
        this->_context,
        cl_source);
        // &result);
    //if (result != CL_SUCCESS)
    //    return result;

    try
    {
        // Compile program for the current CL context
        result = this->_program.build(this->_devices);
    }
    catch (const cl::Error &error)
    {
        std::cerr
            << "Error (" << error.what() << ", " << error.err() << "): "
            << "Failed to build CL kernel.\n"
            << "Details: " << this->_program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(this->_devices.front())
            << std::endl;
    }

    // Create program kernel(s)
    // result = this->_program.createKernels(&this->_kernels);
    return result;
}

cl::Kernel* ClManager::getKernel(const std::string &name)
{
    std::string kernel_name;
    for (cl_uint i=0; i<this->_kernels.size(); ++i)
    {
        cl::Kernel &kernel = this->_kernels[i];
        kernel.getInfo(CL_KERNEL_FUNCTION_NAME, &kernel_name);
        if (name.compare(kernel_name) == 0)
            return &this->_kernels[i];
    }
    return NULL;
}


