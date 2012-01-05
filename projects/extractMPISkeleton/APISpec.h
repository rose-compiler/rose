#ifndef __APISpec_H_LOADED__
#define __APISpec_H_LOADED__

#include <vector>
#include <map>

#include "GenericDepAttrib.h"

typedef std::vector<GenericDepAttribute*> ArgTreatments;
typedef std::map <std::string, ArgTreatments*> APIArgSpec;

class APISpec {
    private:

    APIArgSpec argSpecs;
    name_table codes;
    std::map<std::string, GenericDepAttribute*> types;
    GenericDepAttribute *callAttr;
    GenericDepAttribute *omitAttr;
    uint32_t mask;

    public:

    APISpec() { APISpec(1); };
    APISpec(uint32_t m) : mask(m) {
        callAttr = new GenericDepAttribute(&codes, 0);
        omitAttr = new GenericDepAttribute(&codes, 0);
    };

    GenericDepAttribute* registerType(std::string name);
    GenericDepAttribute* lookupType(std::string name);
    GenericDepAttribute* callType();
    uint32_t nextTypeCode();
    ArgTreatments* addFunction(std::string name,
                               int argCount,
                               GenericDepAttribute* defType);
    ArgTreatments* getFunction(std::string name);
    bool hasFunction(std::string name);
    bool shouldOmit(GenericDepAttribute *attr);
    void addOmitType(GenericDepAttribute *attr);
};

typedef std::vector<APISpec*> APISpecs;

APISpec *lookupFunction(APISpecs *specs, std::string name);

#endif
