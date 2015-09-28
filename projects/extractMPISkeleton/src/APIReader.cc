#include <string>
#include <iostream>
#include <assert.h>
#include <rose.h>

#include "APISpec.h"
#include "ssexpr.h"

using namespace std;

//
// NOTE: malformed s-expressions will result in an assertion instead of
//       a cleaner exit.  clean this up later.
//
APISpec *readAPISpec(string fname, uint32_t typeCode) {
  // parse the file into an s-expression
  SExpr *sx = SExpr::parse_file(fname);

  // we manipulate these.  sx is intended to not be changed since we
  // want to know the root for deallocation later.
  SExpr *cur, *tmp;

  // check that this is an API spec
  cur = sx->getList();
  assert(cur->getValue() == "api-spec");

  cur = cur->getNext();

  APISpec *theSpec = new APISpec(typeCode);

  // spin down dependency types registering them with the APISpec object
  tmp = cur->getNext();
  cur = tmp->getList();

  assert(cur->getValue() == "dep-types");
  for (cur = cur->getNext(); cur != NULL; cur = cur->getNext()) {
    theSpec->registerType(cur->getValue());
  }

  tmp = tmp->getNext();
  cur = tmp->getList();

  assert(cur->getValue() == "default_deptype");
  cur = cur->getNext();

  GenericDepAttribute *defaultDeptype = theSpec->lookupType(cur->getValue());

  tmp = tmp->getNext();
  tmp = tmp->getList();

  for ( ; tmp != NULL; tmp = tmp->getNext()) {
    string funName;
    int arity;
    int idx;
    GenericDepAttribute *depType;

    cur = tmp->getList();
    funName = cur->getValue();

    cur = cur->getNext();
    arity = atoi(cur->getValue().c_str());

    ArgTreatments *funHandle = 
      theSpec->addFunction(funName, arity, defaultDeptype);

    for (cur = cur->getNext(); cur != NULL; cur = cur->getNext()) {
      SExpr *s = cur->getList();
      depType = theSpec->lookupType(s->getValue());

      for (s = s->getNext(); s != NULL; s = s->getNext()) {
        idx = atoi(s->getValue().c_str());
        (*funHandle)[idx] = depType;
      }
    }

  }

  delete sx;

  return theSpec;
}

// This looks suspiciously like the previous function. :)
APISpecs readAPISpecCollection(string fname) {
  APISpecs theSpecs;
  uint32_t typeCode = 1;
  string path = rose::getPathFromFileName(fname);

  // parse the file into an s-expression
  SExpr *sx = SExpr::parse_file(fname);

  // we manipulate these.  sx is intended to not be changed since we
  // want to know the root for deallocation later.
  SExpr *cur, *tmp;

  // check that this is an API spec
  cur = sx->getList();
  assert(cur != NULL);
  assert(cur->getValue() == "api-spec-collection");

  for(tmp = cur->getNext(); tmp != NULL; tmp = tmp->getNext()) {
      cur = tmp->getList();
      if(cur) {
          string e = cur->getValue();
          if(e == "include-api") {
              cur = cur->getNext();
              string fname = path + "/" + cur->getValue();
              APISpec *spec = readAPISpec(fname, typeCode);
              theSpecs.push_back(spec);
              typeCode = spec->nextTypeCode();
              cur = cur->getNext()->getList();
              e = cur->getValue();
              if(e == "omit-deps") {
                  for(cur = cur->getNext(); cur != NULL; cur = cur->getNext()) {
                      string oname = cur->getValue();
                      spec->addOmitType(spec->lookupType(oname));
                  }
              } else {
                  cerr << "Unknown entry: " << e << endl;
              }
          } else {
              cerr << "Unknown entry: " << e << endl;
          }
      } else {
          cerr << "No children of api-spec-collection" << endl;
      }
  }

  return theSpecs;
}
