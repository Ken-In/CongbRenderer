#include "fileManager.h"


namespace FLOAD
{
    bool checkFileValidity(const std::string& filePath)
    {
        struct stat info;
        if(stat(filePath.c_str(), &info) != 0)
        {
            printf("Cannot access %s\n", filePath.c_str());
            return false;
        }
        else if( info.st_mode & S_IFMT ){
            printf( "%s is a valid file\n", filePath.c_str() );
            return true;
        }
        else
        {
            printf("Error! File: %s does not exist.\n", filePath.c_str());
            return false;
        }
    }

    std::string getFileExtension(const std::string& filePath)
    {
        size_t indexLocaton = filePath.rfind('.', filePath.length());
        if(indexLocaton != std::string::npos)
        {
            return filePath.substr(indexLocaton + 1, filePath.length() - indexLocaton);
        }
       return "";
    }
}
