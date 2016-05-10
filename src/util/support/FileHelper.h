// Consider using $ROSE/src/util/FileSystem.h since that one is documented and uses a proper path type and supports both
// version 2 and version 3 of boost::filesystem.

// UNDER NO CIRCUMSTANCES SHOULD BOOST_FILESYSTEM_VERSION BE SET!!!
//
// The boost::filesystem version is not dependent on which compiler we're using, but rather which version
// of boost is installed.  Hard-coding a boost version number based on the compiler version has a couple of problems:
//  1. We don't know whether that filesystem version is available on a user's machine since ROSE supports multiple
//     versions of boost (e.g., filesystem 3 is not available before boost 1.44)
//  2. It pollutes things for the user, who might not want the version we select here (e.g., most users of recent
//     versions of boost will almost certainly want version 3, not the version 2 we select).
// Therefore, we should never select a filesystem version explicitly here, but rather be prepared to handle any version
// that is installed.  If ROSE cannot support a particular version of boost::filesystem on a particular architecture with a
// particular file then that should be documented where we state which versions of boost are supported, and possibly
// checked during configuration. [Matzke 11/17/2014]: 

#include <FileSystem.h>

#include <boost/filesystem.hpp>
#include <string>

class FileHelper {
public:

    // This is initialized in src/frontend/SageIII/sage_support/sage_support.cpp, not FileHelper.C
    static const std::string pathDelimiter;

    static void ensureParentFolderExists(const std::string& path) {
        ensureFolderExists(getParentFolder(path));
    }
    
    static void ensureFolderExists(const std::string& folder){
        boost::filesystem::path boostPath(folder);
        create_directories(boostPath);
    }
    
    static void eraseFolder(const std::string& folder) {
        boost::filesystem::path boostPath(folder);
        remove_all(boostPath);
    }
    
    static std::string concatenatePaths(const std::string& path1, const std::string& path2) {
        if (path1.size() == 0) {
            return path2;
        }
        if (path2.size() == 0) {
            return path1;
        }
        return path1 + pathDelimiter + path2;
    }

    static std::string getParentFolder(const std::string& aPath) {
        boost::filesystem::path boostPath(aPath);
        return boostPath.parent_path().string();
    }

    static std::string getFileName(const std::string& aPath) {
        return rose::FileSystem::toString(boost::filesystem::path(aPath).filename());
    }

    static std::string makeAbsoluteNormalizedPath(const std::string& path, const std::string& workingDirectory) {
        if (!isAbsolutePath(path)) {
            //relative path, so prepend the working directory and then normalize
            return normalizePath(concatenatePaths(workingDirectory, path));
        } else {
            return normalizePath(path);
        }
    }

    static bool isAbsolutePath(const std::string& path) {
        return path.compare(0, 1, pathDelimiter) == 0;
    }
    
    //Expects both paths to be absolute.
    static bool areEquivalentPaths(const std::string& path1, const std::string& path2) {
        //Note: Do not use boost::filesystem::equivalent since the compared paths might not exist, which will cause an error.        
        return normalizePath(path1).compare(normalizePath(path2)) == 0;
    }

    static std::string getIncludedFilePath(const std::list<std::string>& prefixPaths, const std::string& includedPath) {
        for (std::list<std::string>::const_iterator prefixPathPtr = prefixPaths.begin(); prefixPathPtr != prefixPaths.end(); prefixPathPtr++) {
            std::string potentialPath = concatenatePaths(*prefixPathPtr, includedPath);
            if (fileExists(potentialPath)) {
                return potentialPath;
            }
        }
        //The included file was not found, so return an empty string.
        return "";
    }

    //Assumes that both arguments are absolute and normalized.
    //Argument toPath can be either a folder or a file.
    static std::string getRelativePath(const std::string& fromFolder, const std::string& toPath) {
        return rose::FileSystem::toString(rose::FileSystem::makeRelative(toPath, fromFolder));
    }
    
    static bool fileExists(const std::string& fullFileName) {
        return boost::filesystem::exists(fullFileName);
    }

    static bool isNotEmptyFolder(const std::string& fullFolderName) {
        return boost::filesystem::exists(fullFolderName) && !boost::filesystem::is_empty(fullFolderName);
    }
    
    static std::string normalizePath(const std::string& aPath) {
        boost::filesystem::path boostPath(aPath);
        std::string normalizedPath = boostPath.normalize().string();
        return normalizedPath;
    }

    static std::string getNormalizedContainingFileName(PreprocessingInfo* preprocessingInfo) {
        return normalizePath(preprocessingInfo -> get_file_info() -> get_filenameString());
    }

    //Either path1 includes path2 or vice versa
    static std::string pickMoreGeneralPath(const std::string& path1, const std::string& path2) {
        std::string textualPart1 = getTextualPart(path1);
        std::string textualPart2 = getTextualPart(path2);
        //The longer textual part should be more general, assert that
        std::string moreGeneralPath = textualPart1.size() > textualPart2.size() ? textualPart1 : textualPart2;
        ROSE_ASSERT(endsWith(moreGeneralPath, textualPart1));
        ROSE_ASSERT(endsWith(moreGeneralPath, textualPart2));
        return moreGeneralPath;
    }

    static std::string getTextualPart(const std::string& path) {
        std::string normalizedPath = normalizePath(path);
        //Remove all leading "../" and "./" (and they can be only leading, since the path is normalized).
        size_t pos = normalizedPath.rfind(".." + pathDelimiter);
        if (pos != std::string::npos) {
            normalizedPath = normalizedPath.substr(pos + 3);
        }
        pos = normalizedPath.rfind("." + pathDelimiter);
        if (pos != std::string::npos) {
            normalizedPath = normalizedPath.substr(pos + 2);
        }
        return normalizedPath;
    }

    //Count how many leading "../" are in a path (after its normalization) 
    static int countUpsToParentFolder(const std::string& path) {
        std::string normalizedPath = normalizePath(path);
        std::string upPath = ".." + pathDelimiter;
        int counter = 0;
        size_t pos = normalizedPath.find(upPath);
        while (pos != std::string::npos) {
            counter++;
            pos = normalizedPath.find(upPath, pos + 3);
        }
        return counter;
    }

    static bool endsWith(const std::string& str1, const std::string& str2) { //checks that str1 ends with str2
        return boost::ends_with(str1, str2);
    }

};
