#include "APISpec.h"
#include <iostream>

GenericDepAttribute *APISpec::registerType(const std::string & type) {
    std::map<std::string, GenericDepAttribute *>::const_iterator it =
        types.find(type);
    if(it != types.end()) {
        std::cerr << "Type already registered: " << type << std::endl;
        return NULL;
    }
    if(mask == 0) {
        std::cerr << "Maximum number of types exceeded." << std::endl;
        return NULL;
    }

    typedef name_table::value_type nt_value;
    GenericDepAttribute *attr = new GenericDepAttribute(&codes, mask);
    codes.insert(nt_value(mask, type));
    types[type] = attr;
    mask <<= 1;
    return attr;
}

GenericDepAttribute *APISpec::lookupType(const std::string & type) const {
    std::map<std::string, GenericDepAttribute *>::const_iterator it =
        types.find(type);
    if(it != types.end()) {
        return it->second;
    } else {
        std::cerr << "Type not found: " << type << std::endl;
        return NULL;
    }
}

GenericDepAttribute *APISpec::callType() const {
    return callAttr;
}

uint32_t APISpec::nextTypeCode() const {
    return mask;
}

ArgTreatments* APISpec::addFunction(const std::string & name,
                                    int argCount,
                                    const GenericDepAttribute* defType) {
    argSpecs[name] = new ArgTreatments(argCount, defType);
    return argSpecs[name];
}

ArgTreatments* APISpec::getFunction(const std::string & name) const {
    APIArgSpec::const_iterator it = argSpecs.find(name); 
    return it != argSpecs.end() ? it->second : NULL;
}

bool APISpec::hasFunction(const std::string & name) const {
    return argSpecs.find(name) != argSpecs.end();
}

bool APISpec::shouldOmit(const GenericDepAttribute *attr) const {
    bool omit = omitAttr->contains(attr) && attr->getDepType() != 0;
    return omit;
}

void APISpec::addOmitType(const GenericDepAttribute *attr) {
    omitAttr->join(attr);
}

APISpec *lookupFunction(const APISpecs *specs, const std::string & name) {
    APISpecs::const_iterator it;
    for(it = specs->begin(); it != specs->end(); ++it) {
        APISpec *spec = *it;
        if(spec->hasFunction(name)) {
            return spec;
        }
    }
    return NULL;
}
