#include "satire_file_utils.h"

#include <iostream>
#include <sstream>

template <class SContainer>
std::string parts(const SContainer &ss)
{
    std::stringstream result;
    typename SContainer::const_iterator i = ss.begin();
    while (i != ss.end())
    {
        result << *i++;
        if (i != ss.end())
            result << ", ";
    }
    return result.str();
}

std::vector<std::string>
SATIrE::pathNameComponents(std::string pathName)
{
    std::vector<std::string> result;
    std::string::size_type p = 0, end = 0;
    if (pathName.length() > 0)
    {
        if (pathName[0] == '/')
        {
            result.push_back("/");
            end = 1;
        }
        while (end != std::string::npos)
        {
            p = pathName.find_first_not_of('/', end);
            if (p == std::string::npos)
                break;
            end = pathName.find('/', p);
            result.push_back(pathName.substr(p, end-p));
        }
    }

    return result;
}

std::string
SATIrE::pathName(const std::vector<std::string> &components)
{
    std::stringstream result;

    std::vector<std::string>::const_iterator i = components.begin();
    if (i != components.end())
    {
        if (*i == "/") // leading slash: absolute path
        {
            result << '/';
            ++i;
        }
        result << *i++;
        while (i != components.end())
        {
            result << '/' << *i++;
        }
    }

    return result.str();
}

std::string
SATIrE::prefixedFileName(std::string fileName, std::string prefix)
{
    std::vector<std::string> comps = pathNameComponents(fileName);

    if (prefix != "")
    {
        if (comps.size() > 0)
        {
            std::string baseName = comps.back();
            comps.pop_back();
            comps.push_back(prefix + '_' + baseName);
            fileName = pathName(comps);
        }
    }

    return fileName;
}

#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>

#include <cerrno>

bool
directoryExists(std::string dirName)
{
    bool result = false;
    DIR *d = opendir(dirName.c_str());
    if (d != NULL)
        result = true;
    closedir(d);
    return result;
}

void
SATIrE::createDirectoryHierarchy(const std::vector<std::string> &components)
{
    std::vector<std::string>::const_iterator d;

    std::string path = "";
    for (d = components.begin(); d != components.end(); ++d)
    {
        path += *d;
        if (!directoryExists(path))
        {
            errno = 0;
            int cond = mkdir(path.c_str(), 0777);
            if (cond != 0)
            {
                std::string msg = "error: failed to create directory " + path;
                perror(msg.c_str());
                std::exit(EXIT_FAILURE);
            }
        }
        path += '/';
    }
}

bool
SATIrE::openFileForWriting(std::ofstream &file, std::string fileName)
{
    std::vector<std::string> comps = pathNameComponents(fileName);

    comps.pop_back(); // comps without last element is the path to the file
    createDirectoryHierarchy(comps);
    file.open(fileName.c_str());

    return file;
}
