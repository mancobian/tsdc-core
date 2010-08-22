#include "PrintManager.h"
#include <utility>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <vector>
#include <sstream>

PrintManager::PrintManager()
{
}

PrintManager::~PrintManager()
{
}

std::string PrintManager::format(
    const size_t type,
    const std::string &title,
    const std::string &prefix)
{
    switch (type)
    {
        case PrintManager::Category::TITLE:
        {
            return PrintManager::formatTitle(title, prefix);
        }
        default: break;
    }
    return std::string();
}

std::string PrintManager::formatTitle(
    const std::string &title,
    const std::string &prefix)
{
    std::string ftitle("=====[ ");
    ftitle.append(title);
    ftitle.append(" ]=====");

    std::stringstream ss;
    ss << prefix << std::setfill('=') << std::setw(ftitle.size()+1)
        << '\n'
        << prefix << ftitle
        << '\n'
        << prefix << std::setfill('=') << std::setw(ftitle.size()+1)
        << '\n';
    return ss.str();
}
