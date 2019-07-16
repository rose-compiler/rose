
#include "sage3basic.h"
#include "SgNodeHelper.h"

#include "ToolConfig.hpp"

#include "TFHandles.h"
#include <unistd.h>
#include <iostream>

#define MAXPATHLEN 255

#define DEBUG__ToolConfig 0

namespace SgNodeHelper {
  //Returns the name of the file the specified node is part of
  std::string getNodeFileName(SgNode* node){
    SgNode* currentNode = node;
    SgSourceFile* file = nullptr;
    while(file == nullptr && currentNode != nullptr){
      file = isSgSourceFile(currentNode);
      currentNode = currentNode->get_parent();
    }
    if(currentNode == nullptr) return "";
    else return file->getFileName();
  }
}

namespace Typeforge {

//////////////
//ToolAction//
//////////////
ToolAction::ToolAction(std::string actionType) :
  action(actionType),
  name(""),
  handle(""),
  scope(""),
  sourceInfo(""),
  fromType(""),
  toType(""),
  labels(),
  error(-1),
  assignments(-1)
{}

ToolAction::ToolAction() :
  action("None"),
  name(""),
  handle(""),
  scope(""),
  sourceInfo(""),
  fromType(""),
  toType(""),
  labels(),
  error(-1),
  assignments(-1)
{}

//actionType
std::string ToolAction::getActionType() {
    return action;
}

void ToolAction::setActionType(std::string type) {
    action = type;
}

//name
std::string ToolAction::getName() {
    return name;
}

void ToolAction::setName(std::string name) {
    this->name = name;
}

//handle
std::string ToolAction::getHandle() {
    return handle;
}

void ToolAction::setHandle(std::string handle) {
    this->handle = handle;
}

//scope
std::string ToolAction::getScope() {
    return scope;
}

void ToolAction::setScope(std::string scope) {
    this->scope = scope;
}

//sourceInfo
std::string ToolAction::getSourceInfo() {
    return sourceInfo;
}

void ToolAction::setSourceInfo(std::string source) {
    sourceInfo = source;
}

//fromType
std::string ToolAction::getFromType() {
    return fromType;
}

void ToolAction::setFromType(std::string type) {
    fromType = type;
}

//toType
std::string ToolAction::getToType() {
    return toType;
}

void ToolAction::setToType(std::string type) {
    toType = type;
}

// labels

std::vector<std::string> const & ToolAction::getLabels() const {
  return labels;
}

std::vector<std::string> & ToolAction::getLabels() {
  return labels;
}

//error
double ToolAction::getError() {
    return error;
}

void ToolAction::setError(double error) {
    this->error = error;
}

//assignments
long ToolAction::getAssignments() {
    return assignments;
}

void ToolAction::setAssignments(long assignments) {
    this->assignments = assignments;
}

void to_json(json& j, const ToolAction& a) {
    ToolAction temp = a;

    if (temp.getActionType() == "None") {
        return;
    }
    j["action"] = temp.getActionType();

    if (temp.getName() != "") {
        j["name"] = temp.getName();
    }

    if (temp.getHandle() != "") {
        char buff[MAXPATHLEN];
        std::string pwd = getcwd(buff, MAXPATHLEN);
        std::string handle = temp.getHandle();
        size_t loc = handle.find(pwd);
        if (loc != std::string::npos) {
            handle.replace(loc, pwd.length(), "${PWD}");
        }
        j["handle"] = handle;
    }

    if (temp.getScope() != "") {
        j["scope"] = temp.getScope();
    }

    if (temp.getSourceInfo() != "") {
        j["source_info"] = temp.getSourceInfo();
    }

    if (temp.getFromType() != "") {
        j["from_type"] = temp.getFromType();
    }

    if (temp.getToType() != "") {
        j["to_type"] = temp.getToType();
    }

    if (temp.getError() != -1) {
        j["error"] = temp.getError();
    }

    if (temp.getAssignments() != -1) {
        j["dynamic_assignments"] = temp.getAssignments();
    }

    j["labels"] = temp.getLabels();
}

void from_json(const json& j, ToolAction& a) {
    try {
        a.setActionType(j.at("action"));
    } catch(...) {
        a.setActionType("None");
    }

    try {
        a.setName(j.at("name"));
    } catch(...) {
        a.setName("");
    }

    try {
        char buff[MAXPATHLEN];
        std::string pwd = getcwd(buff, MAXPATHLEN);
        std::string handle = j.at("handle");
        size_t loc = handle.find("${PWD}");
        if (loc != std::string::npos) {
            handle.replace(loc, 6, pwd);
        }
        a.setHandle(handle);
    } catch(...) {
        a.setHandle("");
    }

    try {
        a.setScope(j.at("scope"));
    } catch(...) {
        a.setScope("");
    }

    try {
        a.setSourceInfo(j.at("source_info"));
    } catch(...) {
        a.setSourceInfo("");
    }

    try {
        a.setFromType(j.at("from_type"));
    } catch(...) {
        a.setFromType("");
    }

    try {
        a.setToType(j.at("to_type"));
    } catch(...) {
        a.setToType("");
    }

    try {
        a.setError(j.at("error"));
    } catch(...) {
        a.setError(-1);
    }

    try {
        a.setAssignments(j.at("dynamic_assignments"));
    } catch(...) {
        a.setAssignments(-1);
    }

    // TODO read labels
}


//////////////
//ToolConfig//
//////////////

ToolConfig * ToolConfig::global_config{nullptr};
std::string ToolConfig::filename;

#define DEBUG_ToolConfig__static 0

ToolConfig * ToolConfig::getGlobal() {
#if DEBUG_ToolConfig__static
  std::cout << "ToolConfig::getGlobal()" << std::endl;
  std::cout << "  ToolConfig::global_config = " << ToolConfig::global_config << std::endl;
  std::cout << "  ToolConfig::filename      = " << ToolConfig::filename << std::endl;
#endif
  if (ToolConfig::global_config == nullptr) {
    try {
      ToolConfig::global_config = new ToolConfig(ToolConfig::filename);
    } catch (...) {
      ToolConfig::global_config = new ToolConfig();
    }
    ToolConfig::global_config->setToolID("typeforge");
  }
  return ToolConfig::global_config;
}

void ToolConfig::writeGlobal() {
#if DEBUG_ToolConfig__static
  std::cout << "ToolConfig::writeGlobal()" << std::endl;
  std::cout << "  ToolConfig::global_config = " << ToolConfig::global_config << std::endl;
  std::cout << "  ToolConfig::filename      = " << ToolConfig::filename << std::endl;
#endif
  if (ToolConfig::global_config != nullptr && !ToolConfig::filename.empty()) {
    if (!ToolConfig::global_config->saveConfig(ToolConfig::filename)) {
      std::cerr << "[typeforge] Could not save the configuration!" << std::endl;
    }
  }
}

ToolConfig::ToolConfig() {
    this->version = newVersion;
    this->sourceFiles = {};
    this->executable = "";
    this->toolID = "None";
    this->actions = {};
}

ToolConfig::ToolConfig(std::string fname) {
    std::ifstream in(fname);
    json config;
    in >> config;
    from_json(config, *this);
}

//sourceFiles
std::vector<std::string>& ToolConfig::getSourceFiles() {
    return sourceFiles;
}

//executable
std::string ToolConfig::getExecutable() {
    return executable;
}

void ToolConfig::setExecutable(std::string exe) {
    executable = exe;
}

//toolID
std::string ToolConfig::getToolID() {
    return toolID;
}

void ToolConfig::setToolID(std::string toolID) {
    this->toolID = toolID;
}

//version
std::string ToolConfig::getVersion() {
    return this->version;
}

void ToolConfig::setVersion(std::string v) {
    this->version = v;
}

#define DEBUG__ToolConfig__addAction DEBUG__ToolConfig

void ToolConfig::addAction(SgNode * source, std::string action_tag, std::string var_name, SgNode * scope, SgType * fromType, SgType * toType) {

    std::string handle = TFHandles::getAbstractHandle(source);
    assert(handle != "");

    if (actions.find(handle) != actions.end()) {
      std::cerr << "[typeforge] WARNING: Cannot add another action for an existing handle! (" << handle << ")" << std::endl;
      return;
    }

//  std::cout << "scope = " << scope << " ( " << (scope != nullptr ? scope->class_name() : "") << " )" << std::endl;
    if (SgFunctionDefinition * fdefn = isSgFunctionDefinition(scope)) {
      scope = fdefn->get_declaration();
//    std::cout << "scope = " << scope << " ( " << (scope != nullptr ? scope->class_name() : "") << " )" << std::endl;
    }

    std::string scope_name;
    if (scope == nullptr || isSgGlobal(scope)) {
      scope_name = "global";
    } else if (SgFunctionDeclaration * fdecl = isSgFunctionDeclaration(scope)) {
      scope_name = fdecl->get_name();
      scope_name = "function:<" + scope_name + ">";
    }
    assert(scope_name != "");

    std::string src_info  = SgNodeHelper::getNodeFileName(source);
    std::string from_type = fromType->unparseToString();
    std::string to_type   = toType->unparseToString();

#if DEBUG__ToolConfig__addAction
    std::cout << "ToolConfig::addAction" << std::endl;
    std::cout << "  source     = " << source     << " ( " << (source != nullptr ? source->class_name() : "") << " )" << std::endl;
    std::cout << "  action_tag = " << action_tag << std::endl;
    std::cout << "  handle     = " << handle     << std::endl;
    std::cout << "  var_name   = " << var_name   << std::endl;
    std::cout << "  scope_name = " << scope_name << std::endl;
    std::cout << "  src_info   = " << src_info   << std::endl;
    std::cout << "  from_type  = " << from_type  << std::endl;
    std::cout << "  to_type    = " << to_type    << std::endl;
#endif

    ToolAction action(action_tag);
      action.setHandle(handle);
      action.setName(var_name);
      action.setScope(scope_name);
      action.setSourceInfo(src_info);
      action.setFromType(fromType->unparseToString());
      action.setToType(toType->unparseToString());

    actions.insert(std::pair<std::string, ToolAction>(handle, action));
}

#define DEBUG__ToolConfig__addLabel DEBUG__ToolConfig

void ToolConfig::addLabel(SgNode * node, std::string const & label) {
#if DEBUG__ToolConfig__addLabel
    std::cout << "ToolConfig::addLabel" << std::endl;
    std::cout << "  node    = " << node << " ( " << (node != nullptr ? node->class_name() : "") << " )" << std::endl;
    std::cout << "  label   = " << label  << std::endl;
#endif
    std::string handle = TFHandles::getAbstractHandle(node);
    assert(handle != "");

#if DEBUG__ToolConfig__addLabel
    std::cout << "  handle  = " << handle << std::endl;
#endif

    if (actions.find(handle) != actions.end()) {
      actions[handle].getLabels().push_back(label);
    }
}

std::map<std::string, ToolAction> & ToolConfig::getActions() {
    return actions;
}

bool ToolConfig::saveConfig(std::string fname) {
    std::ofstream out(fname, std::ofstream::out | std::ofstream::trunc);
    if (!out.is_open()) {
        return false;
    }
    json config;
    to_json(config, *this);
    //ugly output
    //out << config;
    //pretty output
    out << std::setw(4) << config << std::endl;
    return true;
}

void to_json(json &j, const ToolConfig &a) {
    ToolConfig temp = a;

    std::vector<std::string>& files = temp.getSourceFiles();
    if(files.size() > 0) {
        j["source_files"] = files;
    }

    if (temp.getExecutable() != "") {
        j["executable"] = temp.getExecutable();
    }

    j["tool_id"] = temp.getToolID();
    j["version"] = temp.getVersion();

    std::map<std::string, ToolAction> & actions = temp.getActions();
    for (auto act: temp.getActions()) {
        j["actions"].push_back(act.second);
    }
}

void from_json(const json& j, ToolConfig& a) {
    std::vector<std::string>& files = a.getSourceFiles();
    try {
        files = j.at("source_files").get<std::vector<std::string>>();
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

    std::map<std::string, ToolAction> & actions = a.getActions();
    try {
        std::vector<ToolAction> action_vect = j.at("actions").get<std::vector<ToolAction>>();
        for (auto act: action_vect) {
          actions.insert(std::pair<std::string, ToolAction>(act.getHandle(), act));
        }
    } catch (...) {}

}

}

