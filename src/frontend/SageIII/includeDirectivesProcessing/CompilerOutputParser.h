#include <map>
#include <list>

#include "CompilerOutputReader.h"

using namespace std;

class CompilerOutputParser {
private:
    static const string topLevelParsePrefix;

    SgProject* projectNode;

    //This map keeps track of what files are included in a file. Note that all included files are combined toghether (e.g. the same file may include different files 
    //in the same compilation session depending on preprocessor values), so no particular order is available.
    map<string, set<string> > includedFilesMap;
    
    list<string> quotedIncludesSearchPaths;
    list<string> bracketedIncludesSearchPaths;

    //The following two members are needed for recursive parsing of the compiler output (with -H option)
    string workingDirectory;
    CompilerOutputReader* compilerOutputReader;


    FILE* getCompilerOutput(const vector<string>& argv, bool isVerbose);
    void addIncludedFilesToMap(const string& includingFileName, const set<string>& includedFiles);
    void parseIncludedFilesFromCompilerOutput(const string& includingFileName, const string& parsePrefix);
    void parseIncludedFilesSearchPathsFromCompilerOutput();
    void processFile(SgFile* inputFile, bool isVerbose);

public:
    CompilerOutputParser(SgProject* projectNode);
    map<string, set<string> > collectIncludedFilesMap();
    pair<list<string>, list<string> > collectIncludedFilesSearchPaths();
};