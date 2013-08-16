#ifndef __APISpec_H_LOADED__
#define __APISpec_H_LOADED__

#include <vector>
#include <map>

#include "GenericDepAttrib.h"

// the arguments of a function have attributes associated with them
// individually.  ArgTreatments represents the set of arguments for
// a single function
typedef std::vector< const GenericDepAttribute* > ArgTreatments;

// APIArgSpec is the map of names to argument treatments.  note that
// we key on strings, not symbols.  
typedef std::map< std::string, ArgTreatments* > APIArgSpec;

//
// an API spec object represents a single API specification.  for example,
// MPI is a single specification, and it contains a set of functions.
// multiple APIs will be represented as a set of API Spec objects.
//
class APISpec {
  private:

    // Map of names to arg treatment vectors
    APIArgSpec argSpecs;

    name_table codes;
    std::map<std::string, GenericDepAttribute*> types;
    GenericDepAttribute *callAttr;
    GenericDepAttribute *omitAttr;
    uint32_t mask;

  public:

    APISpec()
    : callAttr(NULL), omitAttr(NULL) // Silence warning
    { APISpec(1); };

    APISpec(uint32_t m) : mask(m) {
        callAttr = new GenericDepAttribute(&codes, 0);
        omitAttr = new GenericDepAttribute(&codes, 0);
    };

    // register a new dependency type
    GenericDepAttribute* registerType(const std::string & name);

    // lookup a dependency type
    GenericDepAttribute* lookupType(const std::string & name) const;

    // This is 'const' but manipulating the returned
    // pointer will mutate the APISpec object.
    GenericDepAttribute* callType() const;
    uint32_t nextTypeCode() const;
    // Creates a mapping for the function 'name' that takes argCount parameters
    // and initializes them all to defType.
    ArgTreatments* addFunction(const std::string & name,
                               int argCount,
                               const GenericDepAttribute* defType);
    ArgTreatments* getFunction(const std::string & name) const;

    bool hasFunction(const std::string & name) const;
    bool shouldOmit(const GenericDepAttribute *attr) const;
    void addOmitType(const GenericDepAttribute *attr);
};

typedef std::vector<APISpec*> APISpecs;

APISpec *lookupFunction(const APISpecs *specs, const std::string & name);

#endif
