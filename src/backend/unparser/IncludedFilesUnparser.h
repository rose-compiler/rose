#include <string>
#include <set>

using namespace std;

class IncludedFilesUnparser : public AstSimpleProcessing {
private:
    SgProject* projectNode;
    string workingDirectory;
    string unparseRootPath;
    static const string defaultUnparseFolderName;
    
    //Keeps track of including paths that include files to unparse into files that would not be unparsed. This is needed to determine where to store the unparsed included files.
    map<string, set<string> > includingPathsMap; 
    //Keeps track of all #include directives that include a *not* unparsed file into a *not* unparsed file.
    set<PreprocessingInfo*> notUnparsedPreprocessingInfos;
    map<string, string> unparseMap; //maps each file that needs to be unparsed to the destination file
    set<string> unparsePaths; //Keeps all paths to which files are unparsed. This is needed to avoid name collisions.
    
    //Keeps all paths that should be used with -I command line option for the compiler. Note that the order of -I paths matters. We should avoid situations when includes that have
    //"up folder" jumps look outside the created folder hierarchy. Therefore, the -I paths are sorted from the deepest "up folder" hierarchy to the least deep.
    list<pair<int, string> > includeCompilerPaths; 
    
    //Keeps scopes that have to be unparsed to produce the included file (i.e. this scope contains the included file).
    //But note that not all of the scopes in this map might require unparsing.
    map<string, SgScopeStatement*> unparseScopesMap;
    
    set<string> modifiedFiles;
    set<string> allFiles;
    set<string> filesToUnparse;
    set<string> newFilesToUnparse; //this is a temporary storage that needs to be accessible across several methods    

    void printDiagnosticOutput();
    void prepareForNewIteration();
    void initializeFilesToUnparse();
    void collectAdditionalFilesToUnparse();
    bool isConflictingIncludePath(const string& includePath);
    void applyFunctionToIncludingPreprocessingInfos(const set<string>& includedFiles, void (IncludedFilesUnparser::*funPtr)(const string& includedFile, PreprocessingInfo* includingPreprocessingInfo));
    void collectNewFilesToUnparse(const string& includedFile, PreprocessingInfo* includingPreprocessingInfo);
    void collectIncludingPathsFromUnaffectedFiles(const string& includedFile, PreprocessingInfo* includingPreprocessingInfo);
    void updatePreprocessingInfoPaths(const string& includedFile, PreprocessingInfo* includingPreprocessingInfo);
    void collectNotUnparsedPreprocessingInfos(const string& includedFile, PreprocessingInfo* includingPreprocessingInfo);
    void populateUnparseMap();
    void collectIncludeCompilerPaths();
    void collectNotUnparsedFilesThatRequireUnparsingToAvoidFileNameCollisions();
    void addIncludeCompilerPath(int upFolderCount, const string& includeCompilerPath);
    void addToUnparseScopesMap(const string& fileName, SgNode* startNode);
    
protected:
    void visit(SgNode* astNode);

public:
    ~IncludedFilesUnparser();
    IncludedFilesUnparser(SgProject* projectNode);
    bool isInputFile(const string& absoluteFileName);
    void unparse();    
    string getUnparseRootPath();
    map<string, string> getUnparseMap();
    map<string, SgScopeStatement*> getUnparseScopesMap();
    list<string> getIncludeCompilerOptions();
};
