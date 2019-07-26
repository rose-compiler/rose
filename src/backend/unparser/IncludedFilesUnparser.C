#include <sage3basic.h>

#include <iostream>

#include "CollectionHelper.h"
#include "IncludeDirective.h"

#include "IncludedFilesUnparser.h"

using namespace std;

const string IncludedFilesUnparser::defaultUnparseFolderName = "_rose_unparsed_headers_";

//It is needed because otherwise, the default destructor breaks something.

IncludedFilesUnparser::~IncludedFilesUnparser() {
    //do nothing
}

IncludedFilesUnparser::IncludedFilesUnparser(SgProject* projectNode) {
    this -> projectNode = projectNode;
}

string IncludedFilesUnparser::getUnparseRootPath() {
    return unparseRootPath;
}

map<string, string> IncludedFilesUnparser::getUnparseMap() {
    return unparseMap;
}

map<string, SgScopeStatement*> IncludedFilesUnparser::getUnparseScopesMap() {
    return unparseScopesMap;
}

map<string, SgSourceFile*> IncludedFilesUnparser::getUnparseSourceFileMap()
   {
  // DQ (9/7/2018): Added to support retrival of SgSourceFile built in the frontend.
     return unparseSourceFileMap;
   }


set<string> IncludedFilesUnparser::getFilesToCopy()
   {
  // DQ (11/19/2018): Added access function.
     return filesToCopy;
   }

list<string> IncludedFilesUnparser::getIncludeCompilerOptions()
   {
     list<string> includeCompilerOptions;
     for (list<pair<int, string> >::const_iterator it = includeCompilerPaths.begin(); it != includeCompilerPaths.end(); it++)
        {
          includeCompilerOptions.push_back("-I" + it -> second);
        }
     return includeCompilerOptions;
   }


// void IncludedFilesUnparser::unparse()
void
IncludedFilesUnparser::figureOutWhichFilesToUnparse()
   {
  // This function does not unparse any files, but identified which included files will 
  // require unparsing (in addition to the original input source file).

#if 1
     printf ("In IncludedFilesUnparser::figureOutWhichFilesToUnparse(): \n");
#endif

#if 0
  // DQ (10/23/2018): Output report of AST nodes marked as modified!
     SageInterface::reportModifiedStatements("In figureOutWhichFilesToUnparse()",projectNode);
#endif

     workingDirectory = FileHelper::normalizePath((* projectNode -> get_fileList().begin()) -> getWorkingDirectory());
     string userSpecifiedUnparseRootFolder = projectNode -> get_unparseHeaderFilesRootFolder();
     if (userSpecifiedUnparseRootFolder.empty() == true) 
        {
       // No folder specified, use the default location.
          unparseRootPath = FileHelper::concatenatePaths(workingDirectory, defaultUnparseFolderName);
        }
       else
        {
          if (FileHelper::isAbsolutePath(userSpecifiedUnparseRootFolder))
             {
               unparseRootPath = userSpecifiedUnparseRootFolder;
             }
            else
             {
               unparseRootPath = FileHelper::concatenatePaths(workingDirectory, userSpecifiedUnparseRootFolder);            
             }

       // Check that the specified location does not exist or is empty. This is necessary to avoid data loss since this folder will be erased.
          if (FileHelper::isNotEmptyFolder(unparseRootPath))
             {
            // DQ (1/29/2018): This case happens when running ROSE from the command line and maybe we should automate the removal of this directory.
#if 1
               printf ("\n\n");
               printf ("******************************************************************************************************** \n");
               printf ("Note: the unparseRootPath directory should be removed before running ROSE with the header file unparsing \n");
               printf ("   --- unparseRootPath = %s \n",unparseRootPath.c_str());
               printf ("******************************************************************************************************** \n");
               printf ("\n\n");
#endif
               cout << "Please make sure that the root folder for header files unparsing does not exist or is empty:" << unparseRootPath << endl;
               ROSE_ASSERT(false);
             }
        }

  // Should be erased completely at every run to avoid name collisions with previous runs.
     FileHelper::eraseFolder(unparseRootPath);

#if 1
     printf ("In IncludedFilesUnparser::figureOutWhichFilesToUnparse(): Calling traversal over AST to detect modified statements! \n");
#endif

  // collect immediately affected files as well as all traversed files

  // DQ (11/28/2018): I think the order of the traversal should be postorder instead of preorder, because we sometimes mark the 
  // enclosing statement tn as modified.
  // traverse(projectNode, preorder);
  // traverse(projectNode, postorder);
     traverse(projectNode, preorder);

#if 1
     printf ("DONE: In IncludedFilesUnparser::figureOutWhichFilesToUnparse(): Calling traversal over AST to detect modified statements! \n");
#endif

#if 0
     printf ("Exiting as a test! \n");
     ROSE_ASSERT(false);
#endif

     initializeFilesToUnparse();

  // A more efficient way would be to do it incrementally rather than repeating the whole iteration. But the probability of more than 
  // one iteration is extremely low, so an average overhead is very insignificant.
     do {

          prepareForNewIteration();

       // DQ (11/19/2018): When using the token-based unpasing we don't modify include paths in the source or header files 
       // and so we don't need to unparse as large of a set of header files as when the unparse_tokens option is NOT used.
          if (projectNode->get_unparse_tokens() == false)
             {
               collectAdditionalFilesToUnparse();
             }
            else
             {
            // We need to for a list of header files to copy, since we can't use the original source directory location as 
            // an include path because then we can't pick up the unparsed header files.
#if 1
               printf ("$$$$$$$$$$$$ Skipping call to collectAdditionalFilesToUnparse() when projectNode->get_unparse_tokens() == true $$$$$$$$$$$$ \n");
#endif
               collectAdditionalListOfHeaderFilesToCopy();
#if 0
               printf ("Exiting as a test! \n");
               ROSE_ASSERT(false);
#endif
             }

          applyFunctionToIncludingPreprocessingInfos(filesToUnparse, &IncludedFilesUnparser::collectIncludingPathsFromUnaffectedFiles);

          populateUnparseMap();

          collectIncludeCompilerPaths();

          applyFunctionToIncludingPreprocessingInfos(allFiles, &IncludedFilesUnparser::collectNotUnparsedPreprocessingInfos);

          collectNotUnparsedFilesThatRequireUnparsingToAvoidFileNameCollisions();

          if (SgProject::get_verbose() > 0)
             {
               CollectionHelper::printSet(newFilesToUnparse, "\nAdditional files to unparse due to path conflicts:", "");
               cout << endl << endl;
             }
        }
     while (!newFilesToUnparse.empty());


  // DQ (11/13/2018): If we are unparsing from the token stream, then we can't be modifying the include directives. 
  // This is also an issue because the #include directives are a part of the white space, and thus transformations 
  // of then can cause them to be unparsed twice (e.g. test9 in UnparseHeader_tests).  Also, modicication of the
  // include directives can trigger unparsing from the AST (which would not otherwise be required).

  // Update including paths for the unparsed files according to unparseMap
  // applyFunctionToIncludingPreprocessingInfos(allFiles, &IncludedFilesUnparser::updatePreprocessingInfoPaths);
     if (projectNode->get_unparse_tokens() == false)
        {
          applyFunctionToIncludingPreprocessingInfos(allFiles, &IncludedFilesUnparser::updatePreprocessingInfoPaths);
        }

     for (list<pair<int, string> >::const_iterator it = includeCompilerPaths.begin(); it != includeCompilerPaths.end(); it++)
        {
          FileHelper::ensureFolderExists(it -> second);
        }

#if 1
     printDiagnosticOutput();
#endif

#if 0
  // DQ (10/23/2018): Output report of AST nodes marked as modified!
     SageInterface::reportModifiedStatements("Leaving figureOutWhichFilesToUnparse()",projectNode);
#endif

#if 1
     printf ("Leaving IncludedFilesUnparser::figureOutWhichFilesToUnparse(): \n");
#endif
#if 0
     printf ("Exiting as a test! \n");
     ROSE_ASSERT(false);
#endif
   }

void
IncludedFilesUnparser::printDiagnosticOutput()
   {
     if (SgProject::get_verbose() >= 0)
        {
          printf ("In IncludedFilesUnparser::printDiagnosticOutput(): Output internal data \n");
          printf ("################################################## \n");

          CollectionHelper::printSet(allFiles, "\nAll files:", "");
          CollectionHelper::printSet(modifiedFiles, "\nModified files:", "");

          CollectionHelper::printSet(filesToUnparse, "\nFiles to unparse:", "");

          CollectionHelper::printSet(filesToCopy, "\nCopy files:", "");

          CollectionHelper::printMapOfSets(includingPathsMap, "\nIncluding paths map:", "Included file:", "Including path:");

          for (map<string, string>::const_iterator it = unparseMap.begin(); it != unparseMap.end(); it++)
             {
               cout << "Unparsed file:" << it -> first << "\nDestination:" << it -> second << endl << endl;
             }

          cout << "\nInclude compiler paths:" << endl;
          for (list<pair<int, string> >::const_iterator it = includeCompilerPaths.begin(); it != includeCompilerPaths.end(); it++)
             {
               cout << it -> first << ":" << it -> second << endl;
             }

          cout << endl << endl;

          printf ("################################################## \n");
          printf ("Leaving IncludedFilesUnparser::printDiagnosticOutput(): Output internal data \n");
        }    
   }

void IncludedFilesUnparser::prepareForNewIteration() 
   {
#if 0
     printf ("In prepareForNewIteration(): newFilesToUnparse.size() = %zu \n",newFilesToUnparse.size());
     set<string>::iterator k = newFilesToUnparse.begin();
     while (k != newFilesToUnparse.end())
       {
         printf ("newFilesToUnparse = %s \n",(*k).c_str());
         k++;
       }
#endif

     filesToUnparse.insert(newFilesToUnparse.begin(), newFilesToUnparse.end());
     newFilesToUnparse.clear();
     includingPathsMap.clear();
     notUnparsedPreprocessingInfos.clear();
     unparseMap.clear();
     unparsePaths.clear();
     includeCompilerPaths.clear();
  // The unparse root path is always included (though could be redundant if no included files need unparsing).
     addIncludeCompilerPath(0, unparseRootPath);

#if 0
     printf ("In prepareForNewIteration(): filesToUnparse.size() = %zu \n",filesToUnparse.size());
     set<string>::iterator i = filesToUnparse.begin();
     while (i != filesToUnparse.end()) 
       {
         printf ("filesToUnparse = %s \n",(*i).c_str());
         i++;
       }
#endif
#if 0
     printf ("In prepareForNewIteration(): newFilesToUnparse.size() = %zu \n",newFilesToUnparse.size());
     set<string>::iterator j = newFilesToUnparse.begin();
     while (j != newFilesToUnparse.end()) 
       {
         printf ("newFilesToUnparse = %s \n",(*j).c_str());
         j++;
       }
#endif
#if 0
     printf ("Exiting as a test! \n");
     ROSE_ASSERT(false);
#endif

   }

bool IncludedFilesUnparser::isInputFile(const string& absoluteFileName) {
    const SgFilePtrList& fileList = projectNode -> get_fileList();
    for (size_t i = 0; i < fileList.size(); ++i) {
        if (absoluteFileName.compare(fileList[i] -> getFileName()) == 0) {
            return true;
        }
    }
    return false;
}

void IncludedFilesUnparser::collectNotUnparsedFilesThatRequireUnparsingToAvoidFileNameCollisions() {
    newFilesToUnparse.clear();
    for (set<PreprocessingInfo*>::const_iterator preprocessingInfoPtr = notUnparsedPreprocessingInfos.begin(); 
            preprocessingInfoPtr != notUnparsedPreprocessingInfos.end(); preprocessingInfoPtr++) {
        IncludeDirective includeDirective((*preprocessingInfoPtr) -> getString());
        const string& includePath  = includeDirective.getIncludedPath();
        if (isConflictingIncludePath(includePath)) {
            newFilesToUnparse.insert(FileHelper::getNormalizedContainingFileName(*preprocessingInfoPtr));            
        }
    }
}

bool IncludedFilesUnparser::isConflictingIncludePath(const string& includePath) {
    for (list<pair<int, string> >::const_iterator includeCompilerPathsIterator = includeCompilerPaths.begin(); includeCompilerPathsIterator != includeCompilerPaths.end(); includeCompilerPathsIterator++) {
        const string& potentialIncludedFilePath = FileHelper::concatenatePaths(includeCompilerPathsIterator -> second, includePath);
        if (FileHelper::fileExists(potentialIncludedFilePath)) {
            //This is a conflict with an existing file.
            return true;
        }
        for (set<string>::const_iterator unparsePathPtr = unparsePaths.begin(); unparsePathPtr != unparsePaths.end(); unparsePathPtr++) {
            const string& unparsedIncludedFilePath = FileHelper::concatenatePaths(unparseRootPath, *unparsePathPtr);
            if (FileHelper::areEquivalentPaths(potentialIncludedFilePath, unparsedIncludedFilePath)) {
                //This is a conflict with a file that will be unparsed.
                return true;
            }
        }
    }
    return false;
}

//TODO: Probably this would not handle correctly cases like #include <../subdir/../A.h> because the normalized representation would be
// <../A.h> and thus, "subdir" would not be created and the file would not be found by the preprocessor. Check and fix, if needed.
void IncludedFilesUnparser::collectIncludeCompilerPaths() {
    for (map<string, set<string> >::const_iterator mapEntry = includingPathsMap.begin(); mapEntry != includingPathsMap.end(); mapEntry++) {
        string fileToUnparse = mapEntry -> first;
        map<string, string>::const_iterator unparseMapEntry = unparseMap.find(fileToUnparse);
        ROSE_ASSERT(unparseMapEntry != unparseMap.end());
        string commonPath = unparseMapEntry -> second;
        const set<string>& includingPaths = mapEntry -> second;
        for (set<string>::const_iterator includingPathPtr = includingPaths.begin(); includingPathPtr != includingPaths.end(); includingPathPtr++) {
            string textualPathPart = FileHelper::getTextualPart(*includingPathPtr);
            size_t startPos = commonPath.rfind(textualPathPart);
            ROSE_ASSERT(startPos != string::npos);
            if (startPos != 0) {
                startPos--; //If did not match the whole commonPath, consider that path delimiter should also be removed
            }
            string includeCompilerPath = commonPath.substr(0, startPos);
            int upFolderCount = FileHelper::countUpsToParentFolder(*includingPathPtr);
            for (int i = 0; i < upFolderCount; i++) {
                includeCompilerPath = FileHelper::concatenatePaths(includeCompilerPath, defaultUnparseFolderName);
            }
            addIncludeCompilerPath(upFolderCount, FileHelper::concatenatePaths(unparseRootPath, includeCompilerPath));
        }
    }
}

void IncludedFilesUnparser::addIncludeCompilerPath(int upFolderCount, const string& includeCompilerPath) {
    list<pair<int, string> >::iterator includeCompilerPathsIterator;
    //First, check if this include path is already present
    for (includeCompilerPathsIterator = includeCompilerPaths.begin(); includeCompilerPathsIterator != includeCompilerPaths.end(); includeCompilerPathsIterator++) {
        if (includeCompilerPath.compare(includeCompilerPathsIterator -> second) == 0) {
            if (includeCompilerPathsIterator -> first >= upFolderCount) {
                return; //This path is present with an equal or greater priority, nothing to do
            } else {
                //This path is present with a lower priority, so remove it and proceed in a regular way.
                includeCompilerPaths.erase(includeCompilerPathsIterator);
                break;
            }
        }
    }
    //If the path is not already present with a sufficiently high priority, insert it at a position corresponding to its priority.
    pair<int, string> newIncludeCompilerPathsEntry(upFolderCount, includeCompilerPath);
    includeCompilerPathsIterator = includeCompilerPaths.begin();
    while (includeCompilerPathsIterator != includeCompilerPaths.end()) {
        if (includeCompilerPathsIterator -> first <= upFolderCount) {
            includeCompilerPaths.insert(includeCompilerPathsIterator, newIncludeCompilerPathsEntry);
            break;
        }
        includeCompilerPathsIterator++;
    }
    if (includeCompilerPathsIterator == includeCompilerPaths.end()) {
        //Iterated till the end, which means that the right place to insert was not found, therefore append to the end.
        includeCompilerPaths.push_back(newIncludeCompilerPathsEntry);
    }
}

void IncludedFilesUnparser::updatePreprocessingInfoPaths(const string& includedFile, PreprocessingInfo* includingPreprocessingInfo) {
    string normalizedIncludingFileName = FileHelper::getNormalizedContainingFileName(includingPreprocessingInfo);
    if (filesToUnparse.find(normalizedIncludingFileName) != filesToUnparse.end()) { //update include paths only in the unparsed files
        map<string, string>::const_iterator includedFileUnparseMapEntry = unparseMap.find(includedFile);
        string replacementIncludeString;
        if (includedFileUnparseMapEntry != unparseMap.end()) {
            //Included file is unparsed, make the include directive bracketed and relative to the unparse root.
            replacementIncludeString = "<" + includedFileUnparseMapEntry -> second + ">";
        } else {
            //Included file is not unparsed, make the include directive quoted and relative to the unparsed including file's containing folder.
            string includingFileUnparseFolder;
            if (isInputFile(normalizedIncludingFileName)) {
                //TODO: Currently, all input files are unparsed into the working directory regardless of where they come from. If this
                //is changed (e.g. input files are unparsed in the folders of the original files), use the commented part.
                includingFileUnparseFolder = workingDirectory;

                //                //Unparsed and original input files are in the same folder, so reuse the initial path: the file name of the unparsed 
                //                //input file would be different, but this does not matter since we get its parent folder, which would be the same.
                //                includingFileUnparsePath = FileHelper::getParentFolder(normalizedIncludingFileName);
            } else {
                map<string, string>::const_iterator includingFileUnparseMapEntry = unparseMap.find(normalizedIncludingFileName);
                ROSE_ASSERT(includingFileUnparseMapEntry != unparseMap.end());
                includingFileUnparseFolder = FileHelper::getParentFolder(FileHelper::concatenatePaths(unparseRootPath, includingFileUnparseMapEntry -> second));
            }
            replacementIncludeString = "\"" + FileHelper::getRelativePath(includingFileUnparseFolder, includedFile) + "\"";
        }
        string includeString = includingPreprocessingInfo -> getString();
        if (SgProject::get_verbose() >= 1) {
            cout << "Original include string:" << includeString << endl;
        }
        IncludeDirective includeDirective(includeString);
        //Replace the original include directive with the new one, using a relative path and brackets.
        includeString.replace(includeDirective.getStartPos() - 1, includeDirective.getIncludedPath().size() + 2, replacementIncludeString);
        includingPreprocessingInfo -> setString(includeString);
        if (SgProject::get_verbose() >= 1) {
            cout << "Updated include string:" << includingPreprocessingInfo -> getString() << endl;
        }
    }
}

void IncludedFilesUnparser::populateUnparseMap()
   {
#if 0
     printf ("In populateUnparseMap(): filesToUnparse.size() = %zu \n",filesToUnparse.size());
#endif

  // First, process files that need to go to a specific location
     for (map<string, set<string> >::const_iterator mapEntry = includingPathsMap.begin(); mapEntry != includingPathsMap.end(); mapEntry++) 
        {
          string fileToUnparse = mapEntry -> first;
          const set<string>& includingPaths = mapEntry -> second;
          set<string>::const_iterator includingPathsIterator = includingPaths.begin();
          string commonPath = *includingPathsIterator;
          includingPathsIterator++;
          while (includingPathsIterator != includingPaths.end())
             {
               commonPath = FileHelper::pickMoreGeneralPath(commonPath, *includingPathsIterator);
               includingPathsIterator++;
             }

          unparseMap.insert(pair<string, string > (fileToUnparse, commonPath));
          ROSE_ASSERT(unparsePaths.find(commonPath) == unparsePaths.end()); //check that all paths are indeed unique as expected
          unparsePaths.insert(commonPath);
        }

  // Next, proceed with all other files that will be unparsed
     for (set<string>::const_iterator fileToUnparsePtr = filesToUnparse.begin(); fileToUnparsePtr != filesToUnparse.end(); fileToUnparsePtr++)
        {
          if (unparseMap.find(*fileToUnparsePtr) == unparseMap.end())
             {
            // consider only files for which the unparse path was not set yet
               string fileName = FileHelper::getFileName(*fileToUnparsePtr);
            // Ensure that the unparse path (in this case - the file name) is unique among all other unparse paths.
               string unparseFileName = fileName;
               int i = 1;
               while (unparsePaths.find(unparseFileName) != unparsePaths.end())
                  {
                    stringstream i_str;
                    i_str << i;
                    unparseFileName = "rose" + i_str.str() + "_" + fileName;
                    i++;
                  }
               unparseMap.insert(pair<string, string > (*fileToUnparsePtr, unparseFileName));
               unparsePaths.insert(unparseFileName);
             }
        }

#if 0
     printf ("Leaving populateUnparseMap(): filesToUnparse.size() = %zu \n",filesToUnparse.size());
#endif
   }


void IncludedFilesUnparser::collectNotUnparsedPreprocessingInfos(const string& includedFile, PreprocessingInfo* includingPreprocessingInfo)
   {
     string normalizedIncludingFileName = FileHelper::getNormalizedContainingFileName(includingPreprocessingInfo);
     if (filesToUnparse.find(includedFile) == filesToUnparse.end() && filesToUnparse.find(normalizedIncludingFileName) == filesToUnparse.end())
        {
       // If both the included and the including files are NOT unparsed, collect the including PreprocessingInfo.        
          notUnparsedPreprocessingInfos.insert(includingPreprocessingInfo);
        }
   }

void IncludedFilesUnparser::collectIncludingPathsFromUnaffectedFiles(const string& includedFile, PreprocessingInfo* includingPreprocessingInfo)
   {
     string normalizedIncludingFileName = FileHelper::getNormalizedContainingFileName(includingPreprocessingInfo);
     if (filesToUnparse.find(normalizedIncludingFileName) == filesToUnparse.end())
        {
          IncludeDirective includeDirective(includingPreprocessingInfo -> getString());
          map<string, set<string> >::iterator mapEntry = includingPathsMap.find(includedFile);
          if (mapEntry != includingPathsMap.end())
             {
               (mapEntry -> second).insert(includeDirective.getIncludedPath());
             }
            else
             {
               set<string> includingPaths;
               includingPaths.insert(includeDirective.getIncludedPath());
               includingPathsMap.insert(pair<string, set<string> >(includedFile, includingPaths));
             }
        }
   }


void
IncludedFilesUnparser::initializeFilesToUnparse()
   {
#if 1
     printf ("In initializeFilesToUnparseMap(): filesToUnparse.size() = %zu \n",filesToUnparse.size());
#endif

  // All modified files have to be unparsed.
     filesToUnparse = modifiedFiles;
    
#if 1
     printf ("In initializeFilesToUnparseMap(): initialized with modifiedFiles: filesToUnparse.size() = %zu \n",filesToUnparse.size());
#endif

  // All input files are also unparsed by default.
     SgFilePtrList inputFilesList = projectNode -> get_fileList();
     for (SgFilePtrList::const_iterator inputFilePtr = inputFilesList.begin(); inputFilePtr != inputFilesList.end(); inputFilePtr++)
        {
          filesToUnparse.insert(FileHelper::normalizePath((*inputFilePtr) -> getFileName())); //normalize just in case it is not normalized by default as expected
        }

#if 1
     printf ("Leaving initializeFilesToUnparseMap(): filesToUnparse.size() = %zu \n",filesToUnparse.size());
#endif
   }

void
IncludedFilesUnparser::collectAdditionalFilesToUnparse()
   {
  // Recursively add to filesToUnparse set any file that includes using quotes (or an absolute path) at least one of the files that is already in filesToUnparse set.
     set<string> workingSet = filesToUnparse;
     while (!workingSet.empty())
        {
          newFilesToUnparse.clear();
          applyFunctionToIncludingPreprocessingInfos(workingSet, &IncludedFilesUnparser::collectNewFilesToUnparse);
          workingSet = newFilesToUnparse;
        }
   }

void IncludedFilesUnparser::collectNewFilesToUnparse(const string& includedFile, PreprocessingInfo* includingPreprocessingInfo) 
   {
#if 0
     printf ("In collectNewFilesToUnparse(): filesToUnparse.size() = %zu \n",filesToUnparse.size());
#endif

     IncludeDirective includeDirective(includingPreprocessingInfo -> getString());
     if (includeDirective.isQuotedInclude() || FileHelper::isAbsolutePath(includeDirective.getIncludedPath())) 
        {
          string normalizedIncludingFileName = FileHelper::getNormalizedContainingFileName(includingPreprocessingInfo);
#if 0
          printf ("In collectNewFilesToUnparse(): normalizedIncludingFileName = %s \n",normalizedIncludingFileName.c_str());
#endif
          if (filesToUnparse.find(normalizedIncludingFileName) == filesToUnparse.end()) 
             {
               filesToUnparse.insert(normalizedIncludingFileName);
               newFilesToUnparse.insert(normalizedIncludingFileName);
             }
        }

#if 0
     printf ("Leaving collectNewFilesToUnparse(): filesToUnparse.size() = %zu \n",filesToUnparse.size());
#endif
   }


void
IncludedFilesUnparser::collectAdditionalListOfHeaderFilesToCopy()
   {
  // Recursively add to filesToUnparse set any file that includes using quotes (or an absolute path) at least one of the files that is already in filesToUnparse set.
     set<string> workingSet = filesToUnparse;
     while (!workingSet.empty())
        {
          newFilesToUnparse.clear();
          applyFunctionToIncludingPreprocessingInfos(workingSet, &IncludedFilesUnparser::collectNewFilesToCopy);
          workingSet = newFilesToUnparse;
        }
   }


void IncludedFilesUnparser::collectNewFilesToCopy(const string& includedFile, PreprocessingInfo* includingPreprocessingInfo) 
   {
#if 1
     printf ("In collectNewFilesToCopy(): filesToCopy.size() = %zu \n",filesToCopy.size());
#endif

     IncludeDirective includeDirective(includingPreprocessingInfo -> getString());
     if (includeDirective.isQuotedInclude() || FileHelper::isAbsolutePath(includeDirective.getIncludedPath())) 
        {
          string normalizedIncludingFileName = FileHelper::getNormalizedContainingFileName(includingPreprocessingInfo);
#if 1
          printf ("In collectNewFilesToCopy(): normalizedIncludingFileName = %s \n",normalizedIncludingFileName.c_str());
#endif
          if (filesToCopy.find(normalizedIncludingFileName) == filesToCopy.end()) 
             {
               filesToCopy.insert(normalizedIncludingFileName);
            // newFilesToUnparse.insert(normalizedIncludingFileName);
             }
        }

  // Go through the list of allFiles, and identify any that are not in the list of files to unparse, and add them to the list of files to copy.
     set<string>::iterator i = allFiles.begin();
     while (i != allFiles.end())
        {
#if 1
          printf ("Checking allFiles: (*i) = %s \n",(*i).c_str());
#endif
          if (modifiedFiles.find(*i) == modifiedFiles.end() && filesToCopy.find(*i) == filesToCopy.end())
             {
#if 1
               printf ("In collectNewFilesToCopy(): adding remaining file to filesToCopy (*i) = %s \n",(*i).c_str());
#endif

            // We need to exclude the ROSE preinclude file from being added to the list of files to copy.
            // filesToCopy.insert(*i);
            // string filenameWithOutPath = FileHelper::normalizePath(*i);
               string filenameWithOutPath = FileHelper::getFileName(*i);
#if 1
               printf ("In collectNewFilesToCopy(): filtering ROSE preinclude file: filenameWithOutPath = %s \n",filenameWithOutPath.c_str());
#endif
               if (filenameWithOutPath != "rose_edg_required_macros_and_functions.h")
                  {
                    filesToCopy.insert(*i);
                  }
                 else
                  {
#if 1
                    printf ("@@@@@@@@ Filtered file: *i = %s \n",(*i).c_str());
#endif
                  }
             }

          i++;
        }

#if 1
     printf ("Leaving collectNewFilesToUnparse(): filesToCopy.size() = %zu \n",filesToCopy.size());
#endif

#if 0
     printf ("Exiting as a test! \n");
     ROSE_ASSERT(false);
#endif
   }



void
IncludedFilesUnparser::applyFunctionToIncludingPreprocessingInfos(
   const set<string>& includedFiles, 
   void (IncludedFilesUnparser::*funPtr)(const string& includedFile, PreprocessingInfo* includingPreprocessingInfo) ) 
   {
     for (set<string>::const_iterator includedFile = includedFiles.begin(); includedFile != includedFiles.end(); includedFile++) 
        {
#if 0
          printf ("In IncludedFilesUnparser::applyFunctionToIncludingPreprocessingInfos(): includedFile = %s \n",includedFile->c_str());
#endif
          const map<string, set<PreprocessingInfo*> >& includingPreprocessingInfosMap = projectNode -> get_includingPreprocessingInfosMap();
          map<string, set<PreprocessingInfo*> >::const_iterator mapEntry = includingPreprocessingInfosMap.find(*includedFile);

          if (mapEntry != includingPreprocessingInfosMap.end())
             {
            // includedFile is really included, so look for all its including preprocessing infos.
#if 0
            // DQ (9/7/2018): Output what files are not included.
               printf ("   --- mapEntry->first = %s \n",mapEntry->first.c_str());
#endif
               const set<PreprocessingInfo*>& includingPreprocessingInfos = mapEntry -> second;
               for (set<PreprocessingInfo*>::const_iterator includingPreprocessingInfoPtr = includingPreprocessingInfos.begin();
                    includingPreprocessingInfoPtr != includingPreprocessingInfos.end(); includingPreprocessingInfoPtr++) 
                  {
                    (this ->* funPtr)(*includedFile, *includingPreprocessingInfoPtr);
                  }
             }
        }

#if 0
     printf ("Exiting as a test! \n");
     ROSE_ASSERT(false);
#endif
   }

void IncludedFilesUnparser::addToUnparseScopesMap(const string& fileName, SgNode* startNode) 
   {
  // We need to find the innermost enclosing scope that is from the including file (i.e. from a file that is different 
  // from this node's file) such that we unparse the whole included file, not just the scope containing the modified stuff.
     SgNode* enclosingScope = SageInterface::getEnclosingNode<SgScopeStatement > (startNode, false);
     while (enclosingScope != NULL && fileName.compare(FileHelper::normalizePath(enclosingScope -> get_file_info() -> get_filenameString())) == 0) 
        {
          enclosingScope = SageInterface::getEnclosingNode<SgScopeStatement > (enclosingScope, false);
        }

     if (enclosingScope != NULL)
        {
       // Found the innermost enclosing scope from the including file.
          unparseScopesMap.insert(pair<string, SgScopeStatement*>(fileName, isSgScopeStatement(enclosingScope)));

          if (SgProject::get_verbose() >= 1) 
             {
               cout << "Enclosing node:" << enclosingScope -> class_name() << endl;
               cout << "Enclosing node's file:" << enclosingScope -> get_file_info() -> get_filenameString() << endl;
             }
        }
   }

void IncludedFilesUnparser::visit(SgNode* node) 
   {

#define DEBUG_INCLUDE_FILE_UNPARSER_VISIT 0

#if DEBUG_INCLUDE_FILE_UNPARSER_VISIT
     printf ("In IncludedFilesUnparser::visit(): node = %p = %s = %s isModified = %s \n",node,node->class_name().c_str(),SageInterface::get_name(node).c_str(),node->get_isModified() ? "true" : "false");
#endif

#if DEBUG_INCLUDE_FILE_UNPARSER_VISIT
     if (isSgGlobal(node) != NULL)
        {
          printf ("In IncludedFilesUnparser::visit(): (SgGlobal): node = %p = %s = %s isModified = %s \n",node,node->class_name().c_str(),SageInterface::get_name(node).c_str(),node->get_isModified() ? "true" : "false");
        }
#endif

  // DQ (6/5/2019): Use this as a predicate to control output spew.
#if DEBUG_INCLUDE_FILE_UNPARSER_VISIT
     bool isStatement = (isSgStatement(node) != NULL);
#endif

     SgSourceFile* sourceFile = isSgSourceFile(node);
     if (sourceFile != NULL)
        {
#if DEBUG_INCLUDE_FILE_UNPARSER_VISIT
          printf ("Building unparseSgSourceFileMap: sourceFile = %p sourceFile->getFileName() = %s \n",sourceFile,sourceFile->getFileName().c_str());
#endif
          unparseSourceFileMap.insert(pair<string,SgSourceFile*>(sourceFile->getFileName(),sourceFile));

       // Save the header file report.
#if 0
          SgHeaderFileReport* reportData = sourceFile->get_headerFileReport();

          if (reportData != NULL)
             {
               printf ("####################################################### \n");
               printf ("####################################################### \n");
               reportData->display("headerFileReport in IncludedFilesUnparser::visit()");
               printf ("####################################################### \n");
               printf ("####################################################### \n");
             }
            else
             {
               printf ("In IncludedFilesUnparser::visit(): reportData == NULL \n");
             }
#endif

#if 0
          printf ("Exiting as a test! \n");
          ROSE_ASSERT(false);
#endif
       }

  // DQ (9/7/2018): Looking for connections to the SgSourceFile in the SgIncludeDirectiveStatement.
  // We could just build up a map of filenames to SgSourceFile IR nodes and then use it with the
  // modifiedFiles set.
     SgIncludeDirectiveStatement* includeDirectiveStatement = isSgIncludeDirectiveStatement(node);
     if (includeDirectiveStatement != NULL)
        {
          SgHeaderFileBody* headerFileBody = includeDirectiveStatement->get_headerFileBody();
          ROSE_ASSERT(headerFileBody != NULL);
          SgSourceFile* headerFile = headerFileBody->get_include_file();

       // DQ (11/22/2018): We only build associated SgSourceFile for application header files.
       // ROSE_ASSERT(headerFile != NULL);
          if (headerFile != NULL)
             {
#if DEBUG_INCLUDE_FILE_UNPARSER_VISIT
               printf ("Building unparseSgSourceFileMap: headerFile = %p headerFile->getFileName() = %s \n",headerFile,headerFile->getFileName().c_str());
#endif

               unparseSourceFileMap.insert(pair<string,SgSourceFile*>(headerFile->getFileName(),headerFile));
             }
        }

     Sg_File_Info* fileInfo = node -> get_file_info();

     if (fileInfo != NULL)
        {
       // DQ (11/28/2018): Need to use the full filename (perhaps resolved of symbolic links) because 
       // filename that match can represent files in different directories.  Though this is more of an 
       // issue for system header file.
       // string normalizedFileName = FileHelper::normalizePath(fileInfo -> get_filenameString());
       // string normalizedFileName = FileHelper::normalizePath(fileInfo->get_physical_filename());
          int physical_file_id      = fileInfo->get_physical_file_id();
          string normalizedFileName = FileHelper::normalizePath(fileInfo->getFilenameFromID(physical_file_id));
          bool isTransformation     = fileInfo->isTransformation();
          bool isCompilerGenerated  = fileInfo->isCompilerGenerated();
          bool isModified           = node->get_isModified();

#if DEBUG_INCLUDE_FILE_UNPARSER_VISIT
          bool isShared             = fileInfo->isShared();
          if (isStatement == true)
             {
               printf ("In IncludedFilesUnparser::visit(): physical_file_id             = %d \n",physical_file_id);
               printf ("In IncludedFilesUnparser::visit(): physical fileName (computed) = %s \n",fileInfo->get_physical_filename().c_str());
               printf ("In IncludedFilesUnparser::visit(): physical fileName (raw)      = %s \n",fileInfo->getFilenameFromID(physical_file_id).c_str());
               printf ("In IncludedFilesUnparser::visit(): normalizedFileName           = %s \n",normalizedFileName.c_str());
               printf ("In IncludedFilesUnparser::visit(): isTransformation             = %s \n",isTransformation    ? "true" : "false");
               printf ("In IncludedFilesUnparser::visit(): isCompilerGenerated          = %s \n",isCompilerGenerated ? "true" : "false");
               printf ("In IncludedFilesUnparser::visit(): isModified                   = %s \n",isModified          ? "true" : "false");
               printf ("In IncludedFilesUnparser::visit(): isShared                     = %s \n",isShared            ? "true" : "false");
             }
#endif

       // if (!isTransformation && !isCompilerGenerated)
          if ( ( (isTransformation == false) && (isCompilerGenerated == false) ) || ((isTransformation == true) && (normalizedFileName != "transformation")) )
             {
            // avoid infos that do not have real file names

#if DEBUG_INCLUDE_FILE_UNPARSER_VISIT
               if (isStatement == true)
                  {
                    printf ("fileInfo->get_file_id() = %d \n",fileInfo->get_file_id());
                    printf ("fileInfo->get_physical_file_id() = %d \n",fileInfo->get_physical_file_id());
                  }
#endif
            // if (fileInfo->get_file_id() >= 0)
               if (fileInfo->get_physical_file_id() >= 0)
                  {
                 // TODO: Investigate why it can be less than 0 (e.g. -2 with file name being NULL_FILE).
                 // Note that any Sg_File_Info that is marked as transformed, will output a filename "transformed" 
                 // and a file_id that is less than zero. What we want to use is the information about the physical file
                 // which in general would have to map to whatever filename a transformations considers itself to be 
                 // associated with.  Need to implement mechanisms to automatically set this (e.g. using the physiscal 
                 // file Id of surreountings statements) and check that is is consistant as well.
#if DEBUG_INCLUDE_FILE_UNPARSER_VISIT
                    if (isStatement == true)
                       {
                         printf ("In IncludedFilesUnparser::visit(): !isTransformation && !isCompilerGenerated: node = %p = %s normalizedFileName = %s \n",node,node->class_name().c_str(),normalizedFileName.c_str());
                       }
#endif
                    set<string>::const_iterator setEntry = allFiles.find(normalizedFileName);
                 // TODO: This is assuming that if a header file is included in multiple places, it is sufficient to unparse just one
                 // instance, i.e. modifications to all places are identical. This needs to be generalized.                    
                    if (setEntry == allFiles.end())
                       {
                      // This is a new file, process it.
#if DEBUG_INCLUDE_FILE_UNPARSER_VISIT
                         printf ("In IncludedFilesUnparser::visit(): !isTransformation && !isCompilerGenerated: This is a new file, process it: file = %s \n",normalizedFileName.c_str());
#endif
                         allFiles.insert(normalizedFileName);

                      // DQ (9/5/2018): Comment added.
                      // We can't just set or use this on the SgSourceFile, since there is only one file object for a translation unit.
                      // We need to add a file name to use in the selection of statements to be unparsed, and then the unparsing needs 
                      // to be directed to that file (and back again, changing whatever is defined to be the current file). This might
                      // cause a large number of files to be open at the same time, in the worst case, but that will be fine for now.
                      // get_unparse_tokens()

                         addToUnparseScopesMap(normalizedFileName, node);
                       }
                      else
                       {
#if DEBUG_INCLUDE_FILE_UNPARSER_VISIT
                         printf ("This is NOT a new file: normalizedFileName = %s \n",normalizedFileName.c_str());
#endif
                       }
                  }
             }

#if DEBUG_INCLUDE_FILE_UNPARSER_VISIT
          if (isStatement == true)
             {
               printf ("List allFiles list (size = %zu): \n",allFiles.size());
               set<string>::iterator i = allFiles.begin();
               size_t counter = 0;
               while (i != allFiles.end())
                  {
                    printf ("   --- allFiles[%zu] = %s \n",counter,(*i).c_str());

                    i++;
                    counter++;
                  }
             }
#endif

       // DQ (6/8/2019): Ignore SgSourceFile and SgProject IR nodes.
          SgSupport* supportNode = isSgSupport(node);
#if 0
          if (supportNode != NULL)
             {
               printf ("Ignore adding SgSupport node to modified file list: supportNode = %p = %s \n",supportNode,supportNode->class_name().c_str());
             }
#endif
       // DQ (6/6/2019): I think that we want to handle statements that are either marked isModified or isTransformation.
       // NOTE: to support the header file unparsing the associated physical file where the target transformation is considered 
       // to live must be specified.
       // if (node -> get_isModified()) 
       // if (isModified == true)
       // if (isModified == true || ( (isTransformation == true) && (normalizedFileName != "transformation") ) )
          if (supportNode == NULL && (isModified == true || ( (isTransformation == true) && (normalizedFileName != "transformation") ) ) )
             {
            // DQ (6/6/2019): Modified statements are important for the token-based unparsing, since they trigger the switch 
            // from using the token stream for unparsing to using the AST for the unparsing.  However, the modified flag is
            // not important if we are not using the token based unparsing.  When we are not using the token-based unparsing
            // (or when we are unparsing directly from the AST) the status of the isTransformation flag is all that is important.

#if DEBUG_INCLUDE_FILE_UNPARSER_VISIT
               printf ("In IncludedFilesUnparser::visit(): node -> get_isModified(): node = %p = %s  \n",node,node->class_name().c_str());
#endif
               if (SgProject::get_verbose() > 0)
                  {
                    cout << "Found a modified node: "    << node -> class_name() << endl;
                    cout << "   In file: "               << normalizedFileName << endl;
                    cout << "   Is transformation: "     << isTransformation << endl;
                    cout << "   Is compiler generated: " << isCompilerGenerated << endl;
                  }
#if 0
            // DQ (6/8/2019): Adding debugging support.
               printf ("Processing this IR node as a modified statement: \n");
               printf (" --- isModified         = %s \n",isModified ? "true" : "false");
               printf (" --- isTransformation   = %s \n",isTransformation ? "true" : "false");
               printf (" --- normalizedFileName = %s \n",normalizedFileName.c_str());
#endif
            // In a preorder traversal, this is not meaningful, since I understand that the parent statement has already been visited.
            // DQ (9/24/2018): If this is not a statement, then mark the enclosing statement as modified.
            // DQ (11/28/2018): This should be an issue for a preorder traversal, so this should be reconsidered.
               if (isSgStatement(node) == NULL)
                  {
                 // DQ (6/8/2019): Added error checking.
                    ROSE_ASSERT(node != NULL);
                    SgStatement* enclosingStatement = TransformationSupport::getStatement(node);
                    if (enclosingStatement == NULL)
                       {
                         printf ("Error: enclosingStatement == NULL: computed from node = %p = %s \n",node,node->class_name().c_str());
                         ROSE_ASSERT(node->get_file_info() != NULL);
                         node->get_file_info()->display("Error: enclosingStatement == NULL: debug");
                       }
                    ROSE_ASSERT(enclosingStatement != NULL);
#if DEBUG_INCLUDE_FILE_UNPARSER_VISIT
                    printf ("Found non-statement = %p = %s as modified, marking enclosing statement = %p = %s \n",
                         node,node->class_name().c_str(),enclosingStatement,enclosingStatement->class_name().c_str());
#endif
                 // DQ (9/24/2018): Mark this statement as a transformation, note that it IS a transformantion instead of contains
                 // a transformation because the statement is the current lowest level of grainularity in the token based unparsing.
                 // enclosingStatement->set_modified(true);
                 // enclosingStatement->set_containsTransformation(true);
                    enclosingStatement->setTransformation();
                  }

            // DQ (11/28/2018): I think this is a bug fix for the recognition of transformed statements in either header files or 
            // source files within the AST.
            // if (!isTransformation && !isCompilerGenerated)
               if (isTransformation == true && isCompilerGenerated == false)
                  {
                 // avoid infos that do not have real file names
#if DEBUG_INCLUDE_FILE_UNPARSER_VISIT
                 // printf ("In IncludedFilesUnparser::visit(): node -> get_isModified(): !isTransformation && !isCompilerGenerated: normalizedFileName = %s \n",normalizedFileName.c_str());
                    printf ("In IncludedFilesUnparser::visit(): node -> get_isModified(): (isTransformation == true && isCompilerGenerated == false): normalizedFileName = %s \n",normalizedFileName.c_str());
#endif
                    modifiedFiles.insert(normalizedFileName);
                  }

#if DEBUG_INCLUDE_FILE_UNPARSER_VISIT
               printf ("In IncludedFilesUnparser::visit(): node -> get_isModified(): output endl \n");
#endif
            // cout << endl << endl;
             }

#if DEBUG_INCLUDE_FILE_UNPARSER_VISIT
          if (isStatement == true)
             {
               printf ("List modifiedFiles list (size = %zu): \n",modifiedFiles.size());
               set<string>::iterator j = modifiedFiles.begin();
               size_t modified_file_counter = 0;
               while (j != modifiedFiles.end())
                  {
                    printf ("   --- modifiedFiles[%zu] = %s \n",modified_file_counter,(*j).c_str());

                    j++;
                    modified_file_counter++;
                  }
             }
#endif
        }


#if 0
     SgFunctionDeclaration* functionDeclaration = isSgFunctionDeclaration(node);
     if (functionDeclaration != NULL && functionDeclaration == functionDeclaration->get_firstNondefiningDeclaration())
        {
          if (functionDeclaration->get_name() == "OUT_1_transformation_0")
             {
#if 0
               printf ("Exiting as a test! \n");
               ROSE_ASSERT(false);
#endif
             }
        }
#endif

#if DEBUG_INCLUDE_FILE_UNPARSER_VISIT
     if (isStatement == true)
        {
       // printf ("Leaving IncludedFilesUnparser::visit(): node = %p = %s modifiedFiles.size() = %zu \n",node,SageInterface::get_name(node).c_str(),modifiedFiles.size());
          printf ("Leaving IncludedFilesUnparser::visit(): node = %p = %s modifiedFiles.size() = %zu \n",node,node->class_name().c_str(),modifiedFiles.size());
        }
#endif
   }


