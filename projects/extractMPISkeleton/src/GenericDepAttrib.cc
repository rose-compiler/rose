#include "rose.h"
#include "GenericDepAttrib.h"

GenericDepAttribute::GenericDepAttribute(name_table *tab, uint32_t ty) {
    depType = ty;
    nameTable = tab;
    depForm = SELF;
    backward = false;
}

GenericDepAttribute::GenericDepAttribute(name_table *tab, std::string ty) {
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

uint32_t GenericDepAttribute::getDepType() const { return depType; }

GenericDepAttribute * GenericDepAttribute::copy() const {
    GenericDepAttribute * const newAttr =
        new GenericDepAttribute(nameTable, depType);
    newAttr->depForm = depForm;
    newAttr->backward = backward;
    return newAttr;
}

void GenericDepAttribute::join(const APIDepAttribute *attr) {
    const GenericDepAttribute * const other = dynamic_cast<const GenericDepAttribute * const>(attr);
    ROSE_ASSERT( other != NULL );
    joinForm(other->depForm);
    joinType(other->depType);
}

void GenericDepAttribute::joinForm(const DependenceForm form) {
    depForm = (APIDepAttribute::DependenceForm)
               ((int)depForm | (int)form);
}

void GenericDepAttribute::joinType(const uint32_t type) {
    depType |= type;
}
bool GenericDepAttribute::matches(const APIDepAttribute *attr) const {
    const GenericDepAttribute * const other = dynamic_cast<const GenericDepAttribute * const>(attr);
    ROSE_ASSERT( other != NULL );
    return (depType == other->depType);
}

bool GenericDepAttribute::contains(const APIDepAttribute *attr) const {
    const GenericDepAttribute * const other = dynamic_cast<const GenericDepAttribute * const>(attr);
    ROSE_ASSERT( other != NULL );
    return (depType | other->depType) == depType;
}

GenericDepAttribute::DependenceForm GenericDepAttribute::getDepForm() const {
    return depForm;
}

void GenericDepAttribute::setDepForm(DependenceForm newDepForm) {
    depForm = newDepForm;
}

std::string GenericDepAttribute::attribute_class_name() const { return "GenericDepAttribute"; }

std::string GenericDepAttribute::toString() const {
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
                    str += "(" + (it->second) + ") ";
                } else {
                    str += "UNKNOWN ";
                }
            }
        }
    }
    return str;
}
