#define BOOST_FILESYSTEM_VERSION 2

#include <boost/filesystem.hpp>

#include <string>

using namespace std;
using namespace boost::filesystem;

class FileHelper {
public:

    static const string pathDelimiter;

    static void ensureParentFolderExists(const string& path) {
        ensureFolderExists(getParentFolder(path));
    }
    
    static void ensureFolderExists(const string& folder){
        path boostPath(folder);
        create_directories(boostPath);
    }
    
    static void eraseFolder(const string& folder) {
        path boostPath(folder);
        remove_all(boostPath);
    }
    
    static string concatenatePaths(const string& path1, const string& path2) {
        if (path1.size() == 0) {
            return path2;
        }
        if (path2.size() == 0) {
            return path1;
        }
        return path1 + pathDelimiter + path2;
    }

    static string getParentFolder(const string& aPath) {
        path boostPath(aPath);
        return boostPath.parent_path().string();
    }

    static string getFileName(const string& aPath) {
        path boostPath(aPath);
        return boostPath.filename();
    }

    static string makeAbsoluteNormalizedPath(const string& path, const string& workingDirectory) {
        if (!isAbsolutePath(path)) {
            //relative path, so prepend the working directory and then normalize
            return normalizePath(concatenatePaths(workingDirectory, path));
        } else {
            return normalizePath(path);
        }
    }

    static bool isAbsolutePath(const string& path) {
        return path.compare(0, 1, pathDelimiter) == 0;
    }
    
    //Expects both paths to be absolute.
    static bool areEquivalentPaths(const string& path1, const string& path2) {
        //Note: Do not use boost::filesystem::equivalent since the compared paths might not exist, which will cause an error.        
        return normalizePath(path1).compare(normalizePath(path2)) == 0;
    }

    //Assumes that both arguments are absolute and normalized.
    //Argument toPath can be either a folder or a file.
    static string getRelativePath(const string& fromFolder, const string& toPath) {
        string relativePath;
        path boostFromFolder(fromFolder);
        path boostToPath(toPath);
        path::const_iterator fromFolderIterator = boostFromFolder.begin();
        path::const_iterator toPathIterator = boostToPath.begin();
        //Move over the common part of the paths.
        while (fromFolderIterator != boostFromFolder.end() && toPathIterator != boostToPath.end() && 
                (*fromFolderIterator).compare(*toPathIterator) == 0) {
            fromFolderIterator++;
            toPathIterator++;
        }
        //All remaining path elements of fromFolder are translated into moves up.
        while (fromFolderIterator != boostFromFolder.end()) {
            relativePath += ".." + pathDelimiter;
            fromFolderIterator++;
        }
        //All remaining path elements of toPath are appended to the relative path.
        if (toPathIterator != boostToPath.end()) {
            relativePath += *toPathIterator; //The first path element comes without the leading path delimiter
            toPathIterator++;
            while (toPathIterator != boostToPath.end()) {
                relativePath += pathDelimiter + *toPathIterator;
                toPathIterator++;
            }                
        } else if (relativePath.length() > 0) { //If any moves up were added, remove the trailing path delimiter
            relativePath.erase(--relativePath.end());
        } else { //If nothing was added to the relative path, then fromFolder and toPath are equal and relative path is the current folder.
            relativePath = ".";
        }
        return relativePath;
    }
    
    static bool fileExists(const string& fullFileName) {
        return exists(fullFileName);
    }

    static string normalizePath(const string& aPath) {
        path boostPath(aPath);
        string normalizedPath = boostPath.normalize().string();
        return normalizedPath;
    }

    static string getNormalizedContainingFileName(PreprocessingInfo* preprocessingInfo) {
        return normalizePath(preprocessingInfo -> get_file_info() -> get_filenameString());
    }

    //Either path1 includes path2 or vice versa
    static string pickMoreGeneralPath(const string& path1, const string& path2) {
        string textualPart1 = getTextualPart(path1);
        string textualPart2 = getTextualPart(path2);
        //The longer textual part should be more general, assert that
        string moreGeneralPath = textualPart1.size() > textualPart2.size() ? textualPart1 : textualPart2;
        ROSE_ASSERT(endsWith(moreGeneralPath, textualPart1));
        ROSE_ASSERT(endsWith(moreGeneralPath, textualPart2));
        return moreGeneralPath;
    }

    static string getTextualPart(const string& path) {
        string normalizedPath = normalizePath(path);
        //Remove all leading "../" and "./" (and they can be only leading, since the path is normalized).
        size_t pos = normalizedPath.rfind(".." + pathDelimiter);
        if (pos != string::npos) {
            normalizedPath = normalizedPath.substr(pos + 3);
        }
        pos = normalizedPath.rfind("." + pathDelimiter);
        if (pos != string::npos) {
            normalizedPath = normalizedPath.substr(pos + 2);
        }
        return normalizedPath;
    }

    //Count how many leading "../" are in a path (after its normalization) 
    static int countUpsToParentFolder(const string& path) {
        string normalizedPath = normalizePath(path);
        string upPath = ".." + pathDelimiter;
        int counter = 0;
        size_t pos = normalizedPath.find(upPath);
        while (pos != string::npos) {
            counter++;
            pos = normalizedPath.find(upPath, pos + 3);
        }
        return counter;
    }

    static bool endsWith(const string& str1, const string& str2) { //checks that str1 ends with str2
        if (str1.size() < str2.size()) {
            return false;
        }
        return str1.compare(str1.size() - str2.size(), str2.size(), str2) == 0;
    }

};
