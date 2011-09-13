#include <string>

class CompilerOutputReader {
private:
    FILE* compilerOutput;
    char *line;
    bool hasCurrentLine;
public:
    CompilerOutputReader(FILE* compilerOutput);
    void readNextLine();
    bool hasLine();
    std::string getLine();
};
