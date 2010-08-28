#include "ClManager.h"
#include "PrintManager.h"

using namespace rssd;
using namespace rssd::gpgpu;

DECLARE_SINGLETON(ClManager);

ClManager::ClManager()
{
    this->create();
}

ClManager::~ClManager()
{
    this->destroy();
}

cl_int ClManager::create()
{
    cl_int result = CL_SUCCESS;

    result = this->queryPlatforms();
    if (result != CL_SUCCESS)
        return result;

    result = this->queryDevices();
    if (result != CL_SUCCESS)
        return result;

    result = this->autoSelectDevice();
    if (result != CL_SUCCESS)
        return result;

    return result;
}

cl_int ClManager::destroy()
{
    cl_int result = CL_SUCCESS;
    return result;
}

cl_int ClManager::queryPlatforms()
{
    // Local vars
    cl_int result = CL_SUCCESS;

    // Clear old platforms
    this->_platforms.clear();

    // Get available platforms
    result = cl::Platform::get(&this->_platforms);
    if ((result != CL_SUCCESS) || (this->_platforms.empty()))
        return result;

    // Create info object for each available CL platform
    ClPlatform_v::iterator
        iter = this->_platforms.begin(),
        end = this->_platforms.end();
    for (; iter != end; ++iter)
    {
        this->_platforms.push_back(*iter);
#if 0
        if (print)
            std::cout << ClPlatformInfo(this->_platforms.back()).toString() << std::endl;
#endif
    }
    return CL_SUCCESS;
}

cl_int ClManager::queryDevices()
{
    // Local vars
    cl_int result = CL_SUCCESS;
    ClDevice_v devices;

    // Clear out all old devices
    this->_devices.clear();

    // Get and store info for every device on each CL platform
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
#if 0
        if (print)
            std::cout << ClDeviceInfo(this->_devices.back()).toString() << std::endl;
#endif
    }
    devices.clear();
    return CL_SUCCESS;
}

cl_int ClManager::autoSelectDevice()
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
        this->_devices.front(),
        CL_QUEUE_PROFILING_ENABLE);
    return result;
}

cl_int ClManager::compile(const std::string &filename)
{
    // Read source file
    std::ifstream source_file(filename.c_str());
    std::string source_code(
        std::istreambuf_iterator<char>(source_file),
        (std::istreambuf_iterator<char>()));

    // Create program source(s)
    cl::Program::Sources source(
        1, std::make_pair(source_code.c_str(), source_code.length()+1));

    // Create program
    this->_program = cl::Program(
        this->_context,
        source);

    // Build program for these specific devices
    /// @todo Explore configuring build options.
    try
    {
        this->_program.build(
            this->_devices,
            "-cl-mad-enable -cl-fast-relaxed-math");
    }
    catch(const cl::Error &error)
    {
        std::cerr << "Building failed , "
            << error.what() << "(" << error.err() << ")" << "\nRetrieving build log"
            << "\nBuild Log Follows\n"
            << this->_program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(this->_devices.front())
            << std::endl;
        return -1;
    }

    // Create program kernel(s)
    return this->createKernels(this->_program);
}

cl_int ClManager::createKernels(cl::Program &program)
{
    // Create program kernel(s)
    std::vector<cl::Kernel> kernels;
    program.createKernels(&kernels);
    std::cout << "Num kernels: " << kernels.size() << '\n';
    for (cl_uint i=0; i<kernels.size(); ++i)
    {
      // Create kernel
      std::cout << "=> Kernel # " << i << ": " << kernels[i].getInfo<CL_KERNEL_FUNCTION_NAME>() << '\n';
      this->_kernels.push_back(
        ClKernel(
          this->_context,
          this->_command_queue,
          kernels[i]));
      // ClKernel &kernel = this->_kernels.back();
    }
    std::cout << std::endl;
    return CL_SUCCESS;
}

ClKernel* ClManager::getKernel(const std::string &name)
{
    std::string kernel_name;
    for (cl_uint i=0; i<this->_kernels.size(); ++i)
    {
        ClKernel &kernel = this->_kernels[i];
        kernel_name = kernel.get().getInfo<CL_KERNEL_FUNCTION_NAME>();
        if (name.compare(kernel_name) == 0)
            return &this->_kernels[i];
    }
    return NULL;
}

double ClManager::profile(cl::Event &event)
{
    // All times are in nanoseconds
    // cl_ulong queue = event.getProfilingInfo<CL_PROFILING_COMMAND_QUEUED>();
    // cl_ulong submit = event.getProfilingInfo<CL_PROFILING_COMMAND_SUBMIT>();
    cl_ulong start = event.getProfilingInfo<CL_PROFILING_COMMAND_START>();
    cl_ulong end = event.getProfilingInfo<CL_PROFILING_COMMAND_END>();
    return ((1.0e-9) * (end - start));
}
