#include <string>
#include <set>
#include <map>
#include <list>

using namespace std;

class IncludingPreprocessingInfosCollector : public AstSimpleProcessing {
private:
    SgProject* projectNode;
    list<string> quotedIncludesSearchPaths;
    list<string> bracketedIncludesSearchPaths;
    map<string, set<string> > includedFilesMap;

    map<string, set<PreprocessingInfo*> > includingPreprocessingInfosMap;

    string getIncludedFilePath(const list<string>& prefixPaths, const string& includedPath);
    string findIncludedFile(const string& currentFolder, const string& includedPath, bool isQuotedInclude);
    void addIncludingPreprocessingInfoToMap(PreprocessingInfo* preprocessingInfo, const string& includedPath, bool isQuotedInclude);
    void matchIncludedAndIncludingFiles();
    
protected:
    void visit(SgNode* astNode);

public:
    IncludingPreprocessingInfosCollector(SgProject* projectNode, const pair<list<string>, list<string> >& includedFilesSearchPaths, const map<string, set<string> >& includedFilesMap);
    map<string, set<PreprocessingInfo*> > collect();
};
