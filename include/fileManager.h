#pragma once

#include <string>

namespace FLOAD
{
    bool checkFileValidity(const std::string &filePath);
    std::string getFileExtension(const std::string &filePath);
}
