#include <rose.h>

#include <iostream>

#include "FileHelper.h"
#include "IncludingPreprocessingInfosCollector.h"
#include "IncludeDirective.h"

using namespace std;

IncludingPreprocessingInfosCollector::IncludingPreprocessingInfosCollector(SgProject* projectNode, const pair<list<string>, list<string> >& includedFilesSearchPaths,
        const map<string, set<string> >& includedFilesMap) {

    this -> projectNode = projectNode;
    quotedIncludesSearchPaths = includedFilesSearchPaths.first;
    bracketedIncludesSearchPaths = includedFilesSearchPaths.second;
    this -> includedFilesMap = includedFilesMap;
}

map<string, set<PreprocessingInfo*> > IncludingPreprocessingInfosCollector::collect() {
    traverse(projectNode, preorder);
    //TODO: Investigate/fix the way ROSE tracks include directives such that none of them are lost, and then uncomment the call to method matchIncludedAndIncludingFiles.
    //Currently, not all included directives are found in the standard header files (e.g. /usr/include/stdio.h).
    //matchIncludedAndIncludingFiles();
    return includingPreprocessingInfosMap;
}

//Every pair IncludingFile -> IncludedFile that is produced by -H compiler option should have a matching pair IncludedFile -> PreprocessingInfo of IncludingFile in the map
//includingPreprocessingInfosMap. This way, ROSE ensures that it found all included files that are actually included by the compiler (-H option), though this is not a guarantee
//of correctness. If this match fails, it means that there is a file that has #include directive that is not followed by any statements. In such scenario, ROSE does not
//capture the corresponding preprocessing information, and consequently the corresponding entry is not added to the map includingPreprocessingInfosMap.
void IncludingPreprocessingInfosCollector::matchIncludedAndIncludingFiles() {
    for (map<string, set<string> >::const_iterator mapIterator = includedFilesMap.begin(); mapIterator != includedFilesMap.end(); mapIterator++) {
        string includingFileName = mapIterator -> first;
        set<string> includedFilesSet = mapIterator -> second;
        for (set<string>::iterator includedFileNamePtr = includedFilesSet.begin(); includedFileNamePtr != includedFilesSet.end(); includedFileNamePtr++) {
            map<string, set<PreprocessingInfo*> >::const_iterator mapEntry = includingPreprocessingInfosMap.find(*includedFileNamePtr);
            ROSE_ASSERT(mapEntry != includingPreprocessingInfosMap.end());
            set<PreprocessingInfo*> includingPreprocessingInfos = mapEntry -> second;
            bool isIncludingFilePresent = false;
            for (set<PreprocessingInfo*>::const_iterator it = includingPreprocessingInfos.begin(); it != includingPreprocessingInfos.end(); it++){
                if (includingFileName.compare(FileHelper::normalizePath((*it) -> get_file_info() -> get_filenameString())) == 0){
                    isIncludingFilePresent = true;
                    break;
                }
            }
            ROSE_ASSERT(isIncludingFilePresent);
        }
    }
}

string IncludingPreprocessingInfosCollector::getIncludedFilePath(const list<string>& prefixPaths, const string& includedPath) {
    for (list<string>::const_iterator prefixPathPtr = prefixPaths.begin(); prefixPathPtr != prefixPaths.end(); prefixPathPtr++) {
        string potentialPath = FileHelper::concatenatePaths(*prefixPathPtr, includedPath);
        if (FileHelper::fileExists(potentialPath)) {
            return potentialPath;
        }
    }
    //The included file was not found, so return an empty string.
    return "";
}

//The returned file path is not normalized.

string IncludingPreprocessingInfosCollector::findIncludedFile(const string& currentFolder, const string& includedPath, bool isQuotedInclude) {
    if (FileHelper::isAbsolutePath(includedPath)) {
        //the path is absolute, so no need to search for the file
        if (FileHelper::fileExists(includedPath)) {
            return includedPath;
        }
        return ""; //file does not exist, so return an empty string
    }
    if (isQuotedInclude) {
        //start looking from the current folder, then proceed with the quoted includes search paths
        //TODO: Consider the presence of -I- option, which disables looking in the current folder for quoted includes.
        quotedIncludesSearchPaths.insert(quotedIncludesSearchPaths.begin(), currentFolder);
        string includedFilePath = getIncludedFilePath(quotedIncludesSearchPaths, includedPath);
        quotedIncludesSearchPaths.erase(quotedIncludesSearchPaths.begin()); //remove the previously inserted current folder (for other files it might be different)
        if (!includedFilePath.empty()) {
            return includedFilePath;
        }
    }
    //For bracketed includes and for not yet found quoted includes proceed with the bracketed includes search paths
    return getIncludedFilePath(bracketedIncludesSearchPaths, includedPath);
}

void IncludingPreprocessingInfosCollector::addIncludingPreprocessingInfoToMap(PreprocessingInfo* preprocessingInfo, const string& includedPath, bool isQuotedInclude) {
    string currentFolder = FileHelper::getParentFolder(preprocessingInfo -> get_file_info() -> get_filenameString());
    string normalizedIncludedFile = FileHelper::normalizePath(findIncludedFile(currentFolder, includedPath, isQuotedInclude));
    if (!normalizedIncludedFile.empty()) {
        //if the included file was found, add it to the map
        map<string, set<PreprocessingInfo*> >::iterator mapEntry = includingPreprocessingInfosMap.find(normalizedIncludedFile);
        if (mapEntry != includingPreprocessingInfosMap.end()) {
            (mapEntry -> second).insert(preprocessingInfo);
        } else {
            set<PreprocessingInfo*> includingPreprocessingInfos;
            includingPreprocessingInfos.insert(preprocessingInfo);
            includingPreprocessingInfosMap.insert(pair<string, set<PreprocessingInfo*> >(normalizedIncludedFile, includingPreprocessingInfos));
        }        
    }
}

void IncludingPreprocessingInfosCollector::visit(SgNode* node) {
    SgLocatedNode* locatedNode = isSgLocatedNode(node);
    if (locatedNode != NULL) {
        AttachedPreprocessingInfoType* preprocessingInfoType = locatedNode -> getAttachedPreprocessingInfo();
        if (preprocessingInfoType != NULL) {
            string containingFileName = FileHelper::normalizePath(locatedNode -> get_file_info() -> get_filenameString());
            for (AttachedPreprocessingInfoType::const_iterator it = preprocessingInfoType -> begin(); it != preprocessingInfoType -> end(); it++) {
                PreprocessingInfo* preprocessingInfo = *it;
                if (preprocessingInfo -> getTypeOfDirective() == PreprocessingInfo::CpreprocessorIncludeDeclaration) {
                    if (SgProject::get_verbose() >= 1){
                        cout << "Found #include directive in file: " << containingFileName << endl;
                    }
                    IncludeDirective includeDirective(preprocessingInfo -> getString());
                    addIncludingPreprocessingInfoToMap(preprocessingInfo, includeDirective.getIncludedPath(), includeDirective.isQuotedInclude());
                }
            }
        }
    }
}