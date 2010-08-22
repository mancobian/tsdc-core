#ifndef RSSD_RESEARCH_GPGPU_CLPLATFORMINFO_H
#define RSSD_RESEARCH_GPGPU_CLPLATFORMINFO_H

#include <OpenCL/cl.hpp>
#include "PrintManager.h"

namespace rssd {
namespace gpgpu {

///
/// @struct ClPlatformInfo
///

struct ClPlatformInfo
{
public:
    ClPlatformInfo(cl::Platform &platform) :
        _platform(platform)
    {
        platform.getInfo((cl_platform_info)CL_PLATFORM_VENDOR, &this->vendor);
        platform.getInfo((cl_platform_info)CL_PLATFORM_NAME, &this->name);
        platform.getInfo((cl_platform_info)CL_PLATFORM_VERSION, &this->version);
        platform.getInfo((cl_platform_info)CL_PLATFORM_PROFILE, &this->profile);
        platform.getInfo((cl_platform_info)CL_PLATFORM_EXTENSIONS, &this->extensions);
    }

    ~ClPlatformInfo()
    {}

public:
    operator cl::Platform()
    {
        return this->_platform;
    }

    std::string toString() const
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

public:
    std::string
        name,
        vendor,
        profile,
        version,
        extensions;

protected:
    cl::Platform _platform;
}; // struct ClPlatform

} // namespace gpgpu
} // namespace rssd

#endif // RSSD_RESEARCH_GPGPU_CLPLATFORMINFO_H
