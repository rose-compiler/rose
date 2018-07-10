#ifndef JConfig_H
#define JConfig_H

#include <vector>
#include <fstream>
#include <iomanip>

#include "JAction.hpp"

//namespace

#define newVersion "1.0"

class JConfig
{
public:
    JConfig();
    JConfig(string fileName);

    void addAction(JAction action);
    void addReplaceVarType(string handle, string var_name, string scope, string source, string fromType, string toType);
    void addReplaceVarType(string handle, double error, long assignments);

    vector<string>& getSourceFiles();

    string getExecutable();
    void setExecutable(string fileName);

    string getToolID();
    void setToolID(string toolID);

    string getVersion();
    void setVersion(string v);

    vector<JAction>& getActions();

    bool saveConfig(string fileName);
private:
        vector<string> sourceFiles;
        string executable;
        string toolID;
        string version;
        vector<JAction> actions;
};

void to_json(json &j, const JConfig &a);
void from_json(const json& j, JConfig& a);

#endif
