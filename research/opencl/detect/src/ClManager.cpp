#include "ClManager.h"
#include <iostream>
#include <sstream>
#include "PrintManager.h"

///
/// @struct ClPlatformInfo
///

ClPlatformInfo::ClPlatformInfo(cl::Platform &platform)
{
    platform.getInfo((cl_platform_info)CL_PLATFORM_VENDOR, &this->vendor);
    platform.getInfo((cl_platform_info)CL_PLATFORM_NAME, &this->name);
    platform.getInfo((cl_platform_info)CL_PLATFORM_VERSION, &this->version);
    platform.getInfo((cl_platform_info)CL_PLATFORM_PROFILE, &this->profile);
    platform.getInfo((cl_platform_info)CL_PLATFORM_EXTENSIONS, &this->extensions);
}

std::string ClPlatformInfo::toString() const
{
    std::string title("Platform: ");
    title.append(this->name);

    std::stringstream ss;
    ss << PrintManager::format(PrintManager::Category::TITLE, title)
        << '\n'
        << "Version: " << this->version << '\n'
        << "Profile: " << this->profile << '\n'
        << "Vendor: " << this->vendor << '\n'
        << "Extensions: " << this->extensions << '\n';
    return ss.str();
}

///
/// @struct ClDeviceInfo
///

ClDeviceInfo::ClDeviceInfo(cl::Device &device)
{
    device.getInfo((cl_device_info)CL_DEVICE_NAME, &this->name); // string
    device.getInfo((cl_device_info)CL_DEVICE_TYPE, &this->type); // cl_device_type
    device.getInfo((cl_device_info)CL_DEVICE_VERSION, &this->version); // string
    device.getInfo((cl_device_info)CL_DEVICE_VENDOR, &this->vendor); // string
    device.getInfo((cl_device_info)CL_DEVICE_VENDOR_ID, &this->vendor_id); // cl_uint
    device.getInfo((cl_device_info)CL_DEVICE_PROFILE, &this->profile); // string
    device.getInfo((cl_device_info)CL_DEVICE_EXTENSIONS, &this->extensions); // string
    device.getInfo((cl_device_info)CL_DEVICE_LOCAL_MEM_TYPE, &this->local_mem_type); // cl_device_mem_cache_type
    device.getInfo((cl_device_info)CL_DEVICE_LOCAL_MEM_SIZE, &this->local_mem_size); // cl_ulong
    device.getInfo((cl_device_info)CL_DEVICE_MAX_CLOCK_FREQUENCY, &this->max_clock_frequency); // cl_uint
    device.getInfo((cl_device_info)CL_DEVICE_MAX_MEM_ALLOC_SIZE, &this->max_mem_alloc_size); // cl_ulong
    device.getInfo((cl_device_info)CL_DEVICE_MAX_SAMPLERS, &this->max_samplers); // cl_uint
    device.getInfo((cl_device_info)CL_DEVICE_AVAILABLE, &this->available); // cl_bool
    device.getInfo((cl_device_info)CL_DEVICE_COMPILER_AVAILABLE, &this->compiler_available); // cl_bool
    device.getInfo((cl_device_info)CL_DEVICE_MAX_COMPUTE_UNITS, &this->max_compute_units); // cl_uint
    device.getInfo((cl_device_info)CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS, &this->max_work_item_dimensions); // cl_uint
    device.getInfo((cl_device_info)CL_DEVICE_MAX_WORK_GROUP_SIZE, &this->max_work_group_size); // cl_uint
    device.getInfo((cl_device_info)CL_DRIVER_VERSION, &this->driver_version); // string
}

std::string ClDeviceInfo::toString() const
{
    std::string title("Device: ");
    title.append(this->name);

    std::stringstream ss;
    ss << PrintManager::format(PrintManager::Category::TITLE, title, "\t")
        << '\n'
        << "\tType: " << this->type << '\n'
        << "\tVersion: " << this->version << '\n'
        << "\tVendor: " << this->vendor << " (id=" << this->vendor_id << ")\n"
        << "\tProfile: " << this->profile << '\n'
        << "\tLocal Mem Type: " << this->local_mem_type << '\n'
        << "\tLocal Mem Size: " << this->local_mem_size << '\n'
        << "\tMax Clock Frequency: " << this->max_clock_frequency << '\n'
        << "\tMax Mem Alloc Size: " << this->max_mem_alloc_size << '\n'
        << "\tMax Samplers: " << this->max_samplers << '\n'
        << "\tAvailable: " << this->available << '\n'
        << "\tCompiler Available: " << this->compiler_available << '\n'
        << "\tMax Compute Units: " << this->max_compute_units << '\n'
        << "\tMax Compute Work Item Dimensions: " << this->max_work_item_dimensions << '\n'
        << "\tMax Work Group Size: " << this->max_work_group_size << '\n'
        << "\tDriver Version: " << this->driver_version << '\n'
        << "\tExtensions: " << this->extensions << '\n';
    return ss.str();
}

    size_t
        max_clock_frequency,
        max_compute_units;
    std::string
        name,
        driver_version;

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
    if (this->detectPlatforms() == 0)
        return 1;
    if (this->detectDevices() == 0)
        return 2;
    return CL_SUCCESS;
}

cl_int ClManager::detectPlatforms()
{
    // Clear out all old platforms
    this->_platforms.clear();

    // Get list of available OpenCL platforms
    cl_int result = cl::Platform::get(&this->_platforms);
    if ((result != CL_SUCCESS) || (this->_platforms.empty()))
    {
        std::cout << "ERROR (id=" << result << "): No valid OpenCL platform(s) detected." << std::endl;
        return 0;
    }

    // Parse and store info for each available CL platform
    ClPlatform_v::iterator
        iter = this->_platforms.begin(),
        end = this->_platforms.end();
    for (; iter != end; ++iter)
    {
        ClPlatformInfo platform_info(*iter);
        this->_platform_infos.push_back(platform_info);
        std::cout << platform_info.toString() << std::endl;
    }
    return this->_platforms.size();
}

cl_int ClManager::detectDevices()
{
    // Local vars
    cl_int result = CL_SUCCESS;
    ClDevice_v devices;

    // Clear out all old devices
    this->_devices.clear();

    // Get and store info for every device on each CL platform
    ClPlatform_v::iterator
        iter = this->_platforms.begin(),
        end = this->_platforms.end();
    for (; iter != end; ++iter)
    {
        result = iter->getDevices(CL_DEVICE_TYPE_ALL, &devices);
        this->_devices.insert(
            this->_devices.end(),
            devices.begin(),
            devices.end());
        devices.clear();
    }

    // Parse and display info for each detected CL device
    ClDevice_v::iterator
        device_iter = this->_devices.begin(),
        device_end = this->_devices.end();
    for (; device_iter != device_end; ++device_iter)
    {
        ClDeviceInfo device_info(*device_iter);
        std::cout << device_info.toString() << std::endl;
    }
    return CL_SUCCESS;
}
