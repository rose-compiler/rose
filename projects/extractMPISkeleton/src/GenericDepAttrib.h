#ifndef __GenericDepAttrib_H_LOADED__
#define __GenericDepAttrib_H_LOADED__

#include <boost/bimap/bimap.hpp>

#include "APIDepAttrib.h"

#ifdef _MSC_VER
  using boost::uint32_t;
#endif

typedef boost::bimaps::bimap<uint32_t,std::string> name_table;

class GenericDepAttribute : public APIDepAttribute {
    private:

    DependenceForm depForm;
    uint32_t depType;
    name_table *nameTable;
    bool backward;

    public:

    GenericDepAttribute(name_table *tab, uint32_t ty) {
        depType = ty;
        nameTable = tab;
        depForm = SELF;
        backward = false;
    }

    GenericDepAttribute(name_table *tab, std::string ty) {
        nameTable = tab;
        depForm = SELF;
        backward = false;
        name_table::right_const_iterator it = nameTable->right.find(ty);
        if(it != nameTable->right.end()) {
            depType = it->second;
        } else {
            depType = 0;
        }
    }

    uint32_t getDepType() { return depType; }

    GenericDepAttribute *copy() {
        GenericDepAttribute *newAttr =
            new GenericDepAttribute(nameTable, depType);
        newAttr->depForm = depForm;
        newAttr->backward = backward;
        return newAttr;
    }

    void join(const APIDepAttribute *attr) {
        GenericDepAttribute *other = (GenericDepAttribute *)attr;
        joinForm(other->depForm);
        joinType(other->depType);
    }

    void joinForm(const DependenceForm form) {
        depForm = (APIDepAttribute::DependenceForm)
                   ((int)depForm | (int)form);
    }

    void joinType(const uint32_t type) {
        depType |= type;
    }

    bool matches(const APIDepAttribute *attr) {
        GenericDepAttribute *other = (GenericDepAttribute *)attr;
        return (depType == other->depType);
    }

    bool contains(const APIDepAttribute *attr) {
        GenericDepAttribute *other = (GenericDepAttribute *)attr;
        return (depType | other->depType) == depType;
    }

    DependenceForm getDepForm() {
        return depForm;
    }

    void setDepForm(DependenceForm newDepForm) {
        depForm = newDepForm;
    }

    std::string attribute_class_name() { return "GenericDepAttribute"; }

    std::string toString() {
        std::string str = "API dependency: ";
        if(depForm & DATA) str += "data ";
        if(depForm & CONTROL) str += "control ";
        if(depForm & SELF) str += "self ";
        if(backward) str += "(backward) ";
        if(depType == 0) {
            str += "(call)";
        } else {
            for(uint32_t mask = 1; mask != 0; mask <<= 1) {
                if(depType & mask) {
                    name_table::left_const_iterator it =
                        nameTable->left.find(mask);
                    if(it != nameTable->left.end()) {
                        str += "(" + it->second + ") ";
                    } else {
                        str += "UNKNOWN ";
                    }
                }
            }
        }
        return str;
    }
};

#endif
