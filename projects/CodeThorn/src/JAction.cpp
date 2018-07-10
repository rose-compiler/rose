#include "JAction.hpp"

JAction::JAction(string actionType) {
    action = actionType;
    varName = "";
    handle = "";
    scope = "";
    sourceInfo = "";
    fromType = "";
    toType = "";
    error = -1;
    assignments = -1;
}

JAction::JAction() {
    action = "None";
    varName = "";
    handle = "";
    scope = "";
    sourceInfo = "";
    fromType = "";
    toType = "";
    error = -1;
    assignments = -1;
}

//actionType
string JAction::getActionType() {
    return action;
}

void JAction::setActionType(string type) {
    action = type;
}

//varName
string JAction::getVarName() {
    return varName;
}

void JAction::setVarName(string name) {
    varName = name;
}

//handle
string JAction::getHandle() {
    return handle;
}

void JAction::setHandle(string handle) {
    this->handle = handle;
}

//scope
string JAction::getScope() {
    return scope;
}

void JAction::setScope(string scope) {
    this->scope = scope;
}

//sourceInfo
string JAction::getSourceInfo() {
    return sourceInfo;
}

void JAction::setSourceInfo(string source) {
    sourceInfo = source;
}

//fromType
string JAction::getFromType() {
    return fromType;
}

void JAction::setFromType(string type) {
    fromType = type;
}

//toType
string JAction::getToType() {
    return toType;
}

void JAction::setToType(string type) {
    toType = type;
}

//error
double JAction::getError() {
    return error;
}

void JAction::setError(double error) {
    this->error = error;
}

//assignments
long JAction::getAssignments() {
    return assignments;
}

void JAction::setAssignments(long assignments) {
    this->assignments = assignments;
}


void to_json(json& j, const JAction& a) {
    JAction temp = a;

    if (temp.getActionType() == "None") {return;}
    j["action"] = temp.getActionType();

    if (temp.getVarName() != "") {
        j["var_name"] = temp.getVarName();
    }

    if (temp.getHandle() != "") {
        j["handle"] = temp.getHandle();
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
}

void from_json(const json& j, JAction& a) {
    try {
        a.setActionType(j.at("action"));
    } catch(...) {
        a.setActionType("None");
    }

    try {
        a.setVarName(j.at("var_name"));
    } catch(...) {
        a.setVarName("");
    }

    try {
        a.setHandle(j.at("handle"));
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
}

