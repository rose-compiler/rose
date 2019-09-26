#include <string>
#include <set>

// DQ (11/19/2018): Commented this out to fix thisi issue.
// DQ (9/7/2018): We should not have a using namespace directive in a header file.
// using namespace std;

class IncludedFilesUnparser : public AstSimpleProcessing 
   {
     private:
          SgProject* projectNode;
          std::string workingDirectory;
          std::string unparseRootPath;
          static const std::string defaultUnparseFolderName;

       // Keeps track of including paths that include files to unparse into files that would not be unparsed. This is needed to determine where to store the unparsed included files.
          std::map<std::string, std::set<std::string> > includingPathsMap;

       // Keeps track of all #include directives that include a *not* unparsed file into a *not* unparsed file.
          std::set<PreprocessingInfo*> notUnparsedPreprocessingInfos;

       // maps each file that needs to be unparsed to the destination file
          std::map<std::string, std::string> unparseMap;

       // Keeps all paths to which files are unparsed. This is needed to avoid name collisions.
          std::set<std::string> unparsePaths;

       // Keeps all paths that should be used with -I command line option for the compiler. Note that the order of -I paths matters. We should avoid situations when includes that have
       // "up folder" jumps look outside the created folder hierarchy. Therefore, the -I paths are sorted from the deepest "up folder" hierarchy to the least deep.
          std::list<std::pair<int, std::string> > includeCompilerPaths;

       // Keeps scopes that have to be unparsed to produce the included file (i.e. this scope contains the included file).
       // But note that not all of the scopes in this map might require unparsing.
          std::map<std::string, SgScopeStatement*> unparseScopesMap;

       // DQ (9/7/2018): Adding map to connect filenames to there associated SgSourceFile IR nodes (built in the frontend).
       // Note that the SgSourceFile IR nodes are now built in the frontend so that we can support the token based unparsing
       // which requires that the mapping of the token stream be done on the AST before any modifications (transformations).
       // This refactoring of work to support the unparsing of headers using the token based unparsing has the the cause of
       // the recent work asociated with the header file unparsing.
          std::map<std::string, SgSourceFile*> unparseSourceFileMap;

          std::set<std::string> modifiedFiles;
          std::set<std::string> allFiles;
          std::set<std::string> filesToUnparse;
          std::set<std::string> newFilesToUnparse; //this is a temporary storage that needs to be accessible across several methods    

          void printDiagnosticOutput();
          void prepareForNewIteration();
          void initializeFilesToUnparse();
          void collectAdditionalFilesToUnparse();
          bool isConflictingIncludePath(const std::string& includePath);
          void applyFunctionToIncludingPreprocessingInfos(const std::set<std::string>& includedFiles, void (IncludedFilesUnparser::*funPtr)(const std::string& includedFile, PreprocessingInfo* includingPreprocessingInfo));
          void collectNewFilesToUnparse(const std::string& includedFile, PreprocessingInfo* includingPreprocessingInfo);
          void collectIncludingPathsFromUnaffectedFiles(const std::string& includedFile, PreprocessingInfo* includingPreprocessingInfo);
          void updatePreprocessingInfoPaths(const std::string& includedFile, PreprocessingInfo* includingPreprocessingInfo);
          void collectNotUnparsedPreprocessingInfos(const std::string& includedFile, PreprocessingInfo* includingPreprocessingInfo);
          void populateUnparseMap();
          void collectIncludeCompilerPaths();
          void collectNotUnparsedFilesThatRequireUnparsingToAvoidFileNameCollisions();
          void addIncludeCompilerPath(int upFolderCount, const std::string& includeCompilerPath);
          void addToUnparseScopesMap(const std::string& fileName, SgNode* startNode);

       // DQ (11/19/2018): We need to copy some unmodified files to the new unparseHeadersDirectory, so that they will 
       // be picked up where we can't explicitly list the original header file location). This list must be constructed 
       // when we are using the token based unparsing, because we cannot modify the include paths.
          std::set<std::string> filesToCopy;

       // DQ (11/19/2018): This function initializes the data member: filesToCopy.
          void collectAdditionalListOfHeaderFilesToCopy();
          void collectNewFilesToCopy(const std::string& includedFile, PreprocessingInfo* includingPreprocessingInfo);

     protected:
          void visit(SgNode* astNode);

     public:
         ~IncludedFilesUnparser();
          IncludedFilesUnparser(SgProject* projectNode);
          bool isInputFile(const std::string& absoluteFileName);

       // void unparse();
          void figureOutWhichFilesToUnparse();

          std::string getUnparseRootPath();
          std::map<std::string, std::string> getUnparseMap();
          std::map<std::string, SgScopeStatement*> getUnparseScopesMap();
          std::list<std::string> getIncludeCompilerOptions();

       // DQ (9/7/2018): Added to support retrival of SgSourceFile built in the frontend.
          std::map<std::string, SgSourceFile*> getUnparseSourceFileMap();

          std::set<std::string> getFilesToCopy();
   };
