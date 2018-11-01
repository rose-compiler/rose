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

list<string> IncludedFilesUnparser::getIncludeCompilerOptions() {
    list<string> includeCompilerOptions;
    for (list<pair<int, string> >::const_iterator it = includeCompilerPaths.begin(); it != includeCompilerPaths.end(); it++) {
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

#if 0
     printf ("In IncludedFilesUnparser::figureOutWhichFilesToUnparse(): \n");
#endif

#if 0
  // DQ (10/23/2018): Output report of AST nodes marked as modified!
     SageInterface::reportModifiedStatements("In figureOutWhichFilesToUnparse()",projectNode);
#endif

     workingDirectory = FileHelper::normalizePath((* projectNode -> get_fileList().begin()) -> getWorkingDirectory());
     string userSpecifiedUnparseRootFolder = projectNode -> get_unparseHeaderFilesRootFolder();
     if (userSpecifiedUnparseRootFolder.empty()) 
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

#if 0
     printf ("In IncludedFilesUnparser::figureOutWhichFilesToUnparse(): Calling traversal over AST to detect modified statements! \n");
#endif

  // collect immediately affected files as well as all traversed files
     traverse(projectNode, preorder);

#if 0
     printf ("DONE: In IncludedFilesUnparser::figureOutWhichFilesToUnparse(): Calling traversal over AST to detect modified statements! \n");
#endif

     initializeFilesToUnparse();

  // A more efficient way would be to do it incrementally rather than repeating the whole iteration. But the probability of more than 
  // one iteration is extremely low, so an average overhead is very insignificant.
     do {

          prepareForNewIteration();

          collectAdditionalFilesToUnparse();

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
    
  // Update including paths for the unparsed files according to unparseMap
     applyFunctionToIncludingPreprocessingInfos(allFiles, &IncludedFilesUnparser::updatePreprocessingInfoPaths);

     for (list<pair<int, string> >::const_iterator it = includeCompilerPaths.begin(); it != includeCompilerPaths.end(); it++)
        {
          FileHelper::ensureFolderExists(it -> second);
        }

#if 0
     printDiagnosticOutput();
#endif

#if 0
  // DQ (10/23/2018): Output report of AST nodes marked as modified!
     SageInterface::reportModifiedStatements("Leaving figureOutWhichFilesToUnparse()",projectNode);
#endif

#if 0
     printf ("Leaving IncludedFilesUnparser::figureOutWhichFilesToUnparse(): \n");
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

void IncludedFilesUnparser::prepareForNewIteration() {
    filesToUnparse.insert(newFilesToUnparse.begin(), newFilesToUnparse.end());
    newFilesToUnparse.clear();
    includingPathsMap.clear();
    notUnparsedPreprocessingInfos.clear();
    unparseMap.clear();
    unparsePaths.clear();
    includeCompilerPaths.clear();
    //The unparse root path is always included (though could be redundant if no included files need unparsing).
    addIncludeCompilerPath(0, unparseRootPath);
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
  // All modified files have to be unparsed.
     filesToUnparse = modifiedFiles;
    
  // All input files are also unparsed by default.
     SgFilePtrList inputFilesList = projectNode -> get_fileList();
     for (SgFilePtrList::const_iterator inputFilePtr = inputFilesList.begin(); inputFilePtr != inputFilesList.end(); inputFilePtr++)
        {
          filesToUnparse.insert(FileHelper::normalizePath((*inputFilePtr) -> getFileName())); //normalize just in case it is not normalized by default as expected
        }
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
     IncludeDirective includeDirective(includingPreprocessingInfo -> getString());
     if (includeDirective.isQuotedInclude() || FileHelper::isAbsolutePath(includeDirective.getIncludedPath())) 
        {
          string normalizedIncludingFileName = FileHelper::getNormalizedContainingFileName(includingPreprocessingInfo);
          if (filesToUnparse.find(normalizedIncludingFileName) == filesToUnparse.end()) 
             {
               filesToUnparse.insert(normalizedIncludingFileName);
               newFilesToUnparse.insert(normalizedIncludingFileName);
             }
        }
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
#if 0
     printf ("In IncludedFilesUnparser::visit(): node = %p = %s = %s isModified = %s \n",node,node->class_name().c_str(),SageInterface::get_name(node).c_str(),node->get_isModified() ? "true" : "false");
#endif

#if 0
     if (isSgGlobal(node) != NULL)
        {
          printf ("In IncludedFilesUnparser::visit(): (SgGlobal): node = %p = %s = %s isModified = %s \n",node,node->class_name().c_str(),SageInterface::get_name(node).c_str(),node->get_isModified() ? "true" : "false");
        }
#endif

     SgSourceFile* sourceFile = isSgSourceFile(node);
     if (sourceFile != NULL)
        {
#if 0
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
          ROSE_ASSERT(headerFile != NULL);

#if 0
          printf ("Building unparseSgSourceFileMap: headerFile = %p headerFile->getFileName() = %s \n",headerFile,headerFile->getFileName().c_str());
#endif

          unparseSourceFileMap.insert(pair<string,SgSourceFile*>(headerFile->getFileName(),headerFile));
        }

     Sg_File_Info* fileInfo = node -> get_file_info();

     if (fileInfo != NULL)
        {
          string normalizedFileName = FileHelper::normalizePath(fileInfo -> get_filenameString());
          bool isTransformation     = fileInfo -> isTransformation();
          bool isCompilerGenerated  = fileInfo -> isCompilerGenerated();

          if (!isTransformation && !isCompilerGenerated)
             {
            // avoid infos that do not have real file names
               if (fileInfo -> get_file_id() >= 0)
                  {
                 // TODO: Investigate why it can be less than 0 (e.g. -2 with file name being NULL_FILE).
#if 0
                    printf ("In IncludedFilesUnparser::visit(): !isTransformation && !isCompilerGenerated: node = %p = %s normalizedFileName = %s \n",node,node->class_name().c_str(),normalizedFileName.c_str());
#endif
                    set<string>::const_iterator setEntry = allFiles.find(normalizedFileName);
                 // TODO: This is assuming that if a header file is included in multiple places, it is sufficient to unparse just one
                 // instance, i.e. modifications to all places are identical. This needs to be generalized.                    
                    if (setEntry == allFiles.end())
                       {
                      // This is a new file, process it.
#if 0
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
                  }
             }

          if (node -> get_isModified()) 
             {
#if 0
               printf ("In IncludedFilesUnparser::visit(): node -> get_isModified(): node = %p = %s  \n",node,node->class_name().c_str());
#endif
               if (SgProject::get_verbose() > 0)
                  {
                    cout << "Found a modified node: "    << node -> class_name() << endl;
                    cout << "   In file: "               << normalizedFileName << endl;
                    cout << "   Is transformation: "     << isTransformation << endl;
                    cout << "   Is compiler generated: " << isCompilerGenerated << endl;
                  }

            // DQ (9/24/2018): If this is not a statement, then mark the enclosing statement as modified.
               if (isSgStatement(node) == NULL)
                  {
                    SgStatement* enclosingStatement = TransformationSupport::getStatement(node);
                    ROSE_ASSERT(enclosingStatement != NULL);
#if 0
                    printf ("Found non-statement = %p = %s as modified, marking enclosing statement = %p = %s \n",
                         node,node->class_name().c_str(),enclosingStatement,enclosingStatement->class_name().c_str());
#endif
                 // DQ (9/24/2018): Mark this statement as a transformation, note that it IS a transformantion instead of contains
                 // a transformation because the statement is the current lowest level of grainularity in the token based unparsing.
                 // enclosingStatement->set_modified(true);
                 // enclosingStatement->set_containsTransformation(true);
                    enclosingStatement->setTransformation();
                  }

               if (!isTransformation && !isCompilerGenerated)
                  {
                 // avoid infos that do not have real file names
#if 0
                    printf ("In IncludedFilesUnparser::visit(): node -> get_isModified(): !isTransformation && !isCompilerGenerated: normalizedFileName = %s \n",normalizedFileName.c_str());
#endif
                    modifiedFiles.insert(normalizedFileName);
                  }

#if 0
               printf ("In IncludedFilesUnparser::visit(): node -> get_isModified(): output endl \n");
#endif
            // cout << endl << endl;
             }
        }
   }


