#include <string>

using namespace std;

class IncludeDirective {
private:
    size_t startPos;
    string directiveText;
    string includedPath;
    bool isQuotedIncludeDirective;

public:
    IncludeDirective(const string& directiveText);
    string getIncludedPath();
    bool isQuotedInclude();
    size_t getStartPos();
};
