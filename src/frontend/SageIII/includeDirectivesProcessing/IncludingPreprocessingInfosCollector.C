#include <rose.h>

#include <iostream>

#include "FileHelper.h"
#include "IncludingPreprocessingInfosCollector.h"
#include "IncludeDirective.h"

using namespace std;


//It is needed because otherwise, the default destructor breaks something.

IncludingPreprocessingInfosCollector::~IncludingPreprocessingInfosCollector() {
    //do nothing
}

IncludingPreprocessingInfosCollector::IncludingPreprocessingInfosCollector(SgProject* projectNode, const map<string, set<string> >& includedFilesMap) {
     this -> projectNode = projectNode;
     this -> includedFilesMap = includedFilesMap;
 }

map<string, set<PreprocessingInfo*> > IncludingPreprocessingInfosCollector::collect() {
    traverse(projectNode, preorder);
    
    matchIncludedAndIncludingFiles();
    
    return includingPreprocessingInfosMap;
}

//Every pair IncludingFile -> IncludedFile that is produced by -H compiler option should have a matching pair IncludedFile -> PreprocessingInfo of IncludingFile in the map
//includingPreprocessingInfosMap. This way, ROSE ensures that it found all included files that are actually included by the compiler (-H option), though this is not a guarantee
//of correctness.
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
    if (SgProject::get_verbose() >= 1){
        cout << "\nIncluded and including file maps matched successfully!\n";
    }
}

void IncludingPreprocessingInfosCollector::addIncludingPreprocessingInfoToMap(PreprocessingInfo* preprocessingInfo) {
    string normalizedIncludedFile = FileHelper::normalizePath(projectNode -> findIncludedFile(preprocessingInfo));
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
                    addIncludingPreprocessingInfoToMap(preprocessingInfo);
                }
            }
        }
    }
}