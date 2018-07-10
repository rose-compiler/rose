#ifndef JAction_H
#define JAction_H

#include <string>

#include <nlohmann/json.hpp>

using namespace std;
using json = nlohmann::json;

class JAction
{
public:
    JAction();

    JAction(string actionType);

    string getActionType();
    void setActionType(string type);

    void setVarName(string name);
    string getVarName();

    void setHandle(string handle);
    string getHandle();

    void setScope(string scope);
    string getScope();

    void setSourceInfo(string source);
    string getSourceInfo();

    string getFromType();
    void setFromType(string type);

    string getToType();
    void setToType(string type);

    double getError();
    void setError(double error);

    long getAssignments();
    void setAssignments(long asignments);

private:
    string action;
    string varName;
    string handle;
    string scope;
    string sourceInfo;
    string fromType;
    string toType;
    double error;
    long assignments;    
};

void to_json(json &j, const JAction &a);
void from_json(const json& j, JAction& a);
#endif
