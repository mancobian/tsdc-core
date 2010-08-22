#ifndef RSSD_RESEARCH_GPGPU_CLDEVICEINFO_H
#define RSSD_RESEARCH_GPGPU_CLDEVICEINFO_H

#include <OpenCL/cl.hpp>
#include "PrintManager.h"

namespace rssd {
namespace gpgpu {

///
/// @struct ClDeviceInfo
///

struct ClDeviceInfo
{
public:
    ClDeviceInfo(cl::Device &device) :
        _device(device)
    {
        device.getInfo((cl_device_info)CL_DEVICE_NAME, &this->name); // string
        device.getInfo((cl_device_info)CL_DEVICE_TYPE, &this->type); // cl_device_type
        device.getInfo((cl_device_info)CL_DEVICE_VERSION, &this->version); // string
        device.getInfo((cl_device_info)CL_DEVICE_VENDOR, &this->vendor); // string
        device.getInfo((cl_device_info)CL_DEVICE_VENDOR_ID, &this->vendor_id); // cl_uint
        device.getInfo((cl_device_info)CL_DEVICE_PROFILE, &this->profile); // string
        device.getInfo((cl_device_info)CL_DEVICE_EXTENSIONS, &this->extensions); // string
#if 0
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
#endif
    }

    ~ClDeviceInfo()
    {}

public:
    std::string toString() const
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
#if 0
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
#endif
            << "\tExtensions: " << this->extensions << '\n';
        return ss.str();
    }

public:
    cl_bool
        available,
        compiler_available;
    cl_uint
        vendor_id,
        max_clock_frequency,
        max_compute_units,
        max_work_item_dimensions,
        max_work_group_size,
        max_samplers;
    cl_ulong
        local_mem_size,
        max_mem_alloc_size;
    cl_device_type type;
    cl_device_mem_cache_type local_mem_type;
    std::string
        name,
        version,
        vendor,
        profile,
        extensions,
        driver_version;

protected:
    cl::Device _device;
}; // struct ClDeviceInfo

#if 0
    F(cl_device_info, CL_DEVICE_TYPE, cl_device_type) \
    F(cl_device_info, CL_DEVICE_VENDOR_ID, cl_uint) \
    F(cl_device_info, CL_DEVICE_MAX_COMPUTE_UNITS, cl_uint) \
    F(cl_device_info, CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS, cl_uint) \
    F(cl_device_info, CL_DEVICE_MAX_WORK_GROUP_SIZE, ::size_t) \
    F(cl_device_info, CL_DEVICE_MAX_WORK_ITEM_SIZES, VECTOR_CLASS< ::size_t>) \
    F(cl_device_info, CL_DEVICE_PREFERRED_VECTOR_WIDTH_CHAR, cl_uint) \
    F(cl_device_info, CL_DEVICE_PREFERRED_VECTOR_WIDTH_SHORT, cl_uint) \
    F(cl_device_info, CL_DEVICE_PREFERRED_VECTOR_WIDTH_INT, cl_uint) \
    F(cl_device_info, CL_DEVICE_PREFERRED_VECTOR_WIDTH_LONG, cl_uint) \
    F(cl_device_info, CL_DEVICE_PREFERRED_VECTOR_WIDTH_FLOAT, cl_uint) \
    F(cl_device_info, CL_DEVICE_PREFERRED_VECTOR_WIDTH_DOUBLE, cl_uint) \
    F(cl_device_info, CL_DEVICE_MAX_CLOCK_FREQUENCY, cl_uint) \
    F(cl_device_info, CL_DEVICE_ADDRESS_BITS, cl_bitfield) \
    F(cl_device_info, CL_DEVICE_MAX_READ_IMAGE_ARGS, cl_uint) \
    F(cl_device_info, CL_DEVICE_MAX_WRITE_IMAGE_ARGS, cl_uint) \
    F(cl_device_info, CL_DEVICE_MAX_MEM_ALLOC_SIZE, cl_ulong) \
    F(cl_device_info, CL_DEVICE_IMAGE2D_MAX_WIDTH, ::size_t) \
    F(cl_device_info, CL_DEVICE_IMAGE2D_MAX_HEIGHT, ::size_t) \
    F(cl_device_info, CL_DEVICE_IMAGE3D_MAX_WIDTH, ::size_t) \
    F(cl_device_info, CL_DEVICE_IMAGE3D_MAX_HEIGHT, ::size_t) \
    F(cl_device_info, CL_DEVICE_IMAGE3D_MAX_DEPTH, ::size_t) \
    F(cl_device_info, CL_DEVICE_IMAGE_SUPPORT, cl_uint) \
    F(cl_device_info, CL_DEVICE_MAX_PARAMETER_SIZE, ::size_t) \
    F(cl_device_info, CL_DEVICE_MAX_SAMPLERS, cl_uint) \
    F(cl_device_info, CL_DEVICE_MEM_BASE_ADDR_ALIGN, cl_uint) \
    F(cl_device_info, CL_DEVICE_MIN_DATA_TYPE_ALIGN_SIZE, cl_uint) \
    F(cl_device_info, CL_DEVICE_SINGLE_FP_CONFIG, cl_device_fp_config) \
    F(cl_device_info, CL_DEVICE_GLOBAL_MEM_CACHE_TYPE, cl_device_mem_cache_type) \
    F(cl_device_info, CL_DEVICE_GLOBAL_MEM_CACHELINE_SIZE, cl_uint)\
    F(cl_device_info, CL_DEVICE_GLOBAL_MEM_CACHE_SIZE, cl_ulong) \
    F(cl_device_info, CL_DEVICE_GLOBAL_MEM_SIZE, cl_ulong) \
    F(cl_device_info, CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE, cl_ulong) \
    F(cl_device_info, CL_DEVICE_MAX_CONSTANT_ARGS, cl_uint) \
    F(cl_device_info, CL_DEVICE_LOCAL_MEM_TYPE, cl_device_local_mem_type) \
    F(cl_device_info, CL_DEVICE_LOCAL_MEM_SIZE, cl_ulong) \
    F(cl_device_info, CL_DEVICE_ERROR_CORRECTION_SUPPORT, cl_bool) \
    F(cl_device_info, CL_DEVICE_PROFILING_TIMER_RESOLUTION, ::size_t) \
    F(cl_device_info, CL_DEVICE_ENDIAN_LITTLE, cl_bool) \
    F(cl_device_info, CL_DEVICE_AVAILABLE, cl_bool) \
    F(cl_device_info, CL_DEVICE_COMPILER_AVAILABLE, cl_bool) \
    F(cl_device_info, CL_DEVICE_EXECUTION_CAPABILITIES, cl_device_exec_capabilities) \
    F(cl_device_info, CL_DEVICE_QUEUE_PROPERTIES, cl_command_queue_properties) \
    F(cl_device_info, CL_DEVICE_PLATFORM, cl_platform_id) \
    F(cl_device_info, CL_DEVICE_NAME, STRING_CLASS) \
    F(cl_device_info, CL_DEVICE_VENDOR, STRING_CLASS) \
    F(cl_device_info, CL_DRIVER_VERSION, STRING_CLASS) \
    F(cl_device_info, CL_DEVICE_PROFILE, STRING_CLASS) \
    F(cl_device_info, CL_DEVICE_VERSION, STRING_CLASS) \
    F(cl_device_info, CL_DEVICE_EXTENSIONS, STRING_CLASS)
#endif

} // namespace gpgpu
} // namespace rssd

#endif // RSSD_RESEARCH_GPGPU_CLDEVICEINFO_H
