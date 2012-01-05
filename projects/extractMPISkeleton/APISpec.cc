#include "APISpec.h"

GenericDepAttribute *APISpec::registerType(std::string type) {
    std::map<std::string, GenericDepAttribute *>::iterator it =
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

GenericDepAttribute *APISpec::lookupType(std::string type) {
    std::map<std::string, GenericDepAttribute *>::iterator it =
        types.find(type);
    if(it != types.end()) {
        return it->second;
    } else {
        std::cerr << "Type not found: " << type << std::endl;
        return NULL;
    }
}

GenericDepAttribute *APISpec::callType() {
    return callAttr;
}

uint32_t APISpec::nextTypeCode() {
    return mask;
}

ArgTreatments* APISpec::addFunction(std::string name,
                                    int argCount,
                                    GenericDepAttribute* defType) {
    argSpecs[name] = new ArgTreatments(argCount, defType);
    return argSpecs[name];
}

ArgTreatments* APISpec::getFunction(std::string name) {
    return argSpecs[name];
}

bool APISpec::hasFunction(std::string name) {
    return argSpecs.find(name) != argSpecs.end();
}

bool APISpec::shouldOmit(GenericDepAttribute *attr) {
    bool omit = omitAttr->contains(attr) && attr->getDepType() != 0;
    return omit;
}

void APISpec::addOmitType(GenericDepAttribute *attr) {
    omitAttr->join(attr);
}

APISpec *lookupFunction(APISpecs *specs, std::string name) {
    APISpecs::iterator it;
    for(it = specs->begin(); it != specs->end(); it++) {
        APISpec *spec = *it;
        if(spec->hasFunction(name)) {
            return spec;
        }
    }
    return NULL;
}
