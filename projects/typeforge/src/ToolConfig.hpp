#ifndef ToolConfig_H
#define ToolConfig_H

#include <vector>
#include <fstream>
#include <iomanip>
#include <string>
#include <cstdlib>

#include "json.hpp"

#define newVersion "1.0"

class SgNode;
class SgType;
class SgScopeStatement;

namespace Typeforge {

using json = nlohmann::json;

class ToolAction
{
public:
    ToolAction();

    ToolAction(std::string actionType);

    std::string getActionType();
    void setActionType(std::string type);

    void setName(std::string name);
    std::string getName();

    void setHandle(std::string handle);
    std::string getHandle();

    void setScope(std::string scope);
    std::string getScope();

    void setSourceInfo(std::string source);
    std::string getSourceInfo();

    std::string getFromType();
    void setFromType(std::string type);

    std::string getToType();
    void setToType(std::string type);

    std::vector<std::string> & getLabels();
    std::vector<std::string> const & getLabels() const;

    double getError();
    void setError(double error);

    long getAssignments();
    void setAssignments(long asignments);

private:
    std::string action;
    std::string name;
    std::string handle;
    std::string scope;
    std::string sourceInfo;
    std::string fromType;
    std::string toType;
    std::vector<std::string> labels;
    double error;
    long assignments;
};

void to_json(json &j, const ToolAction &a);
void from_json(const json& j, ToolAction& a);

class ToolConfig {
public:
    static ToolConfig * getGlobal();
    static void writeGlobal();
    static void appendAnalysis(SgType * type = nullptr);

    static std::string filename;

private:
    static ToolConfig * global_config;

public:
    ToolConfig();
    ToolConfig(std::string fileName);

    void addAction(SgNode * node, SgType * toType, std::string action_tag);
    void addLabel(SgNode * node, std::string const & label);

    std::vector<std::string>& getSourceFiles();

    std::string getExecutable();
    void setExecutable(std::string fileName);

    std::string getToolID();
    void setToolID(std::string toolID);

    std::string getVersion();
    void setVersion(std::string v);

    std::map<std::string, ToolAction> & getActions();

    bool saveConfig(std::string fileName);

private:
    std::vector<std::string> sourceFiles;
    std::string executable;
    std::string toolID;
    std::string version;
    std::map<std::string, ToolAction> actions;
};

void to_json(json &j, const ToolConfig &a);
void from_json(const json& j, ToolConfig& a);

}

#endif
