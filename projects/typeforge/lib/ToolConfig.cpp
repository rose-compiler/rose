#include "sage3basic.h"

#include "SgNodeHelper.h"

#include "Typeforge/ToolConfig.hpp"
#include "Typeforge/Analysis.hpp"

#include <unistd.h>
#include <iostream>

#define MAXPATHLEN 255

#ifndef DEBUG__ToolAction
#  define DEBUG__ToolAction 0
#endif
#ifndef DEBUG__ToolAction__from_json
#  define  DEBUG__ToolAction__from_json DEBUG__ToolAction
#endif

#ifndef DEBUG__ToolConfig
#  define DEBUG__ToolConfig 0
#endif
#ifndef DEBUG__ToolConfig__statics
#  define DEBUG__ToolConfig__statics    DEBUG__ToolConfig
#endif
#ifndef DEBUG__ToolConfig__addAction
#  define DEBUG__ToolConfig__addAction DEBUG__ToolConfig
#endif
#ifndef DEBUG__ToolConfig__addLabel
#  define DEBUG__ToolConfig__addLabel  DEBUG__ToolConfig
#endif
#ifndef DEBUG__ToolConfig__appendAnalysis
#  define DEBUG__ToolConfig__appendAnalysis DEBUG__ToolConfig
#endif
#ifndef DEBUG__ToolConfig__from_json
#  define DEBUG__ToolConfig__from_json DEBUG__ToolConfig
#endif
#ifndef WARNING__ToolConfig
#  define WARNING__ToolConfig 0
#endif

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

size_t ToolAction::annon_count = 0;

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
std::string const & ToolAction::getActionType() const {
    return action;
}

void ToolAction::setActionType(std::string type) {
    action = type;
}

//name
std::string const & ToolAction::getName() const {
    return name;
}

void ToolAction::setName(std::string name) {
    this->name = name;
}

//handle
std::string const & ToolAction::getHandle() const {
    return handle;
}

void ToolAction::setHandle(std::string handle) {
    this->handle = handle;
}

//scope
std::string const & ToolAction::getScope() const {
    return scope;
}

void ToolAction::setScope(std::string scope) {
    this->scope = scope;
}

//sourceInfo
std::string const & ToolAction::getSourceInfo() const {
    return sourceInfo;
}

void ToolAction::setSourceInfo(std::string source) {
    sourceInfo = source;
}

//fromType
std::string const & ToolAction::getFromType() const {
    return fromType;
}

void ToolAction::setFromType(std::string type) {
    fromType = type;
}

//toType
std::string const & ToolAction::getToType() const {
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
double ToolAction::getError() const {
    return error;
}

void ToolAction::setError(double error) {
    this->error = error;
}

//assignments
long ToolAction::getAssignments() const {
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
#if DEBUG__ToolAction__from_json
    std::cout << "::from_json(const json &, ToolAction &)" << std::endl;
#endif
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
    if (a.getHandle().size() == 0) {
      std::ostringstream oss;
      oss << "annonymous[" << ToolAction::annon_count++ << "]";
      a.setHandle(oss.str());
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

ToolConfig * ToolConfig::getGlobal() {
#if DEBUG__ToolConfig__statics
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
#if DEBUG__ToolConfig__statics
  std::cout << "ToolConfig::writeGlobal()" << std::endl;
  std::cout << "  ToolConfig::global_config = " << ToolConfig::global_config << std::endl;
  std::cout << "  ToolConfig::filename      = " << ToolConfig::filename << std::endl;
#endif
  if (!ToolConfig::getGlobal()->saveConfig(ToolConfig::filename)) {
    std::cerr << "[typeforge] Could not save the configuration!" << std::endl;
  }
}

void ToolConfig::appendAnalysis(SgType * type) {
#if DEBUG__ToolConfig__appendAnalysis
  std::cout << "ToolConfig::appendAnalysis()" << std::endl;
  std::cout << "  type = " << type << std::endl;
#endif

  if (ToolConfig::global_config != nullptr) {
    std::vector<std::set<SgNode *> > clusters;
    ::Typeforge::typechain.buildClusters(clusters, type);
    for (size_t s = 0; s < clusters.size(); ++s) {
      std::ostringstream oss; oss << "typechain:cluster=" << s;
      std::string label = oss.str();
      for (auto n : clusters[s]) {
        ToolConfig::global_config->addLabel(n, label);
      }
    }

    for (auto e: ::Typeforge::typechain.edges) {
      auto h = ::Typeforge::typechain.getHandle(e.first);
      std::ostringstream oss; oss << "typechain:address=" << h;
      std::string label = oss.str();
      ToolConfig::global_config->addLabel(e.first, label);
      for (auto target_stack: e.second) {
        auto target = target_stack.first;
        ToolConfig::global_config->addLabel(target, label);
      }
    }
  }
}

ToolConfig::ToolConfig() {
#if DEBUG__ToolConfig
  std::cout << "ToolConfig::ToolConfig()" << std::endl;
#endif
    this->version = newVersion;
    this->sourceFiles = {};
    this->executable = "";
    this->toolID = "None";
    this->actions = {};
}

ToolConfig::ToolConfig(std::string fname) {
#if DEBUG__ToolConfig
  std::cout << "ToolConfig::ToolConfig()" << std::endl;
  std::cout << "  fname = " << fname << std::endl;
#endif
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

void ToolConfig::addAction(SgNode * node, SgType * toType, std::string action_tag) {
    assert(node != nullptr);
    assert(toType != nullptr);

#if DEBUG__ToolConfig__addAction
    std::cout << "ToolConfig::addAction" << std::endl;
    std::cout << "  node       = " << node       << " ( " << node->class_name()   << " )" << std::endl;
    std::cout << "  toType     = " << toType     << " ( " << toType->class_name() << " ) : " << toType->unparseToString() << std::endl;
    std::cout << "  action_tag = " << action_tag << std::endl;
#endif

    std::string handle = ::Typeforge::typechain.getHandle(node);
    assert(handle != "");

#if DEBUG__ToolConfig__addAction
    std::cout << "  handle     = " << handle     << std::endl;
#endif

    if (actions.find(handle) != actions.end()) {
#if WARNING__ToolConfig
      std::cerr << "[typeforge] WARNING: Cannot add another action for an existing handle! (" << handle << ")" << std::endl;
#endif
      return;
    }

    ToolAction action(action_tag);
    action.setHandle(handle);
    action.setName(handle);
    action.setToType(toType->unparseToString());

    SgType * fromType = ::Typeforge::typechain.getType(node);
    assert(fromType != nullptr);
#if DEBUG__ToolConfig__addAction
    std::cout << "  fromType   = " << fromType   << " ( " << fromType->class_name() << " ) : " << fromType->unparseToString() << std::endl;
#endif
    action.setFromType(fromType->unparseToString());

    SgNode * scope = ::Typeforge::typechain.getScope(node);
#if DEBUG__ToolConfig__addAction
    std::cout << "  scope      = " << scope      << " ( " << (scope    != nullptr ? scope->class_name()    : "") << " )" << std::endl;
#endif

    std::string scope_name;
    if (scope == nullptr || isSgGlobal(scope)) {
      scope_name = "global";
    } else if (SgClassDeclaration * xdecl = isSgClassDeclaration(scope)) {
      scope_name = xdecl->get_qualified_name();
      scope_name = "class:<" + scope_name + ">";
    } else if (SgFunctionDeclaration * fdecl = isSgFunctionDeclaration(scope)) {
      scope_name = fdecl->get_qualified_name();
      scope_name = "function:<" + scope_name + ">";
    }
    assert(scope_name != "");
#if DEBUG__ToolConfig__addAction
    std::cout << "  scope_name = " << scope_name << std::endl;
#endif
    action.setScope(scope_name);

    std::string src_info = ::Typeforge::typechain.getPosition(node);
#if DEBUG__ToolConfig__addAction
    std::cout << "  src_info   = " << src_info   << std::endl;
#endif
    action.setSourceInfo(src_info);

    actions.insert(std::pair<std::string, ToolAction>(handle, action));
}

void ToolConfig::addLabel(SgNode * node, std::string const & label) {
#if DEBUG__ToolConfig__addLabel
    std::cout << "ToolConfig::addLabel" << std::endl;
    std::cout << "  node    = " << node << " ( " << (node != nullptr ? node->class_name() : "") << " )" << std::endl;
    std::cout << "  label   = " << label  << std::endl;
#endif
    std::string handle = ::Typeforge::typechain.getHandle(node);
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

    for (auto act: temp.getActions()) {
        j["actions"].push_back(act.second);
    }
}

void from_json(const json& j, ToolConfig& a) {
#if DEBUG__ToolConfig__from_json
    std::cout << "::from_json(const json &, ToolConfig &)" << std::endl;
#endif
    std::vector<std::string> & files = a.getSourceFiles();
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

