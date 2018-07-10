#include "JConfig.hpp"

JConfig::JConfig(){
    this->version = newVersion;
    this->sourceFiles = {};
    this->executable = "";
    this->toolID = "None";
    this->actions = {};
}

JConfig::JConfig(string fileName) {
    ifstream in(fileName);
    json config;
    in >> config;
    from_json(config, *this);
}

//sourceFiles
vector<string>& JConfig::getSourceFiles() {
   return sourceFiles;
}

//executable
string JConfig::getExecutable() {
    return executable;
}

void JConfig::setExecutable(string exe) {
    executable = exe;
}

//toolID
string JConfig::getToolID() {
    return toolID;
}

void JConfig::setToolID(string toolID) {
    this->toolID = toolID;
}

//version
string JConfig::getVersion() {
    return this->version;
}

void JConfig::setVersion(string v){
    this->version = v;
}

//actions
void JConfig::addAction(JAction action) {
    actions.push_back(action);
}

void JConfig::addReplaceVarType(string handle, string var_name, string scope, string source, string fromType, string toType){
    JAction action("replace_var_type");
    action.setHandle(handle);
    action.setVarName(var_name);
    action.setScope(scope);
    action.setSourceInfo(source);
    action.setFromType(fromType);
    action.setToType(toType);
    addAction(action);    
}

void JConfig::addReplaceVarType(string handle, double error, long assignments){
    JAction action("replace_var_type");
    action.setError(error);
    action.setAssignments(assignments);    
    action.setHandle(handle);
    addAction(action);    
}

vector<JAction>& JConfig::getActions() {
    return actions;
}

bool JConfig::saveConfig(string fileName) {
    ofstream out(fileName);
    if (out ==  0) {
        return false;
    }
    json config;
    to_json(config, *this);
    //ugly output
    //out << config;
    //pretty output
    out << setw(4) << config << endl;
    return true;
}

void to_json(json &j, const JConfig &a){
    JConfig temp = a;

    vector<string>& files = temp.getSourceFiles();
    if(files.size() > 0){
        j["source_files"] = files;
    }

    if (temp.getExecutable() != ""){
        j["executable"] = temp.getExecutable();
    }

    j["tool_id"] = temp.getToolID();    
    j["version"] = temp.getVersion();

    vector<JAction>& actions = temp.getActions();
    if(actions.size() > 0){
        j["actions"] = actions;
    }
}

void from_json(const json& j, JConfig& a){
    vector<string>& files = a.getSourceFiles();
    try {
        files = j.at("source_files").get<vector<string>>();
    } catch (...) {
        files = {};
    }

    try {
        a.setExecutable(j.at("executable"));
    } catch (...) {
        a.setExecutable("");
    }

    try {
        a.setToolID(j.at("tool_id"));
    } catch (...) {
        a.setToolID("None");
    }

    try {
        a.setVersion(j.at("version"));
    } catch (...) {
        a.setVersion(newVersion);
    }

    vector<JAction>& actions = a.getActions();
    try {
        actions = j.at("actions").get<vector<JAction>>();
    } catch (...) {
        actions = {};
    }
}
