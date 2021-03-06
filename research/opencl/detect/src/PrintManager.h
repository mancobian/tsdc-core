#include <string>

#ifndef RSSD_RESEARCH_OPENCL_DETECT_PRINTMANAGER_H
#define RSSD_RESEARCH_OPENCL_DETECT_PRINTMANAGER_H

class PrintManager
{
public:
    struct Category
    {
        enum Type
        {
            UNKNOWN = 0,
            TITLE,
            COUNT
        }; // enum Type
    }; // struct Category

public:
    PrintManager();
    virtual ~PrintManager();

public:
    static std::string createTitle(const std::string &title);
    static std::string format(
        const size_t type,
        const std::string &title,
        const std::string &prefix = std::string());

protected:
    static std::string formatTitle(
        const std::string &title,
        const std::string &prefix = std::string());
}; // class PrintManager

#endif // RSSD_RESEARCH_OPENCL_DETECT_PRINTMANAGER_H
