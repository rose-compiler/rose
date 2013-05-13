#ifndef APIDEPCHECKER_H
#define APIDEPCHECKER_H

#include "DangerousOperationFinder.h"

class APIDepChecker : public NodeChecker {
  private:

    APISpecs *specs;

  public:

    APIDepChecker(APISpecs *s) : specs(s) {}

    bool check(SgNode * const node) {
        if ( node->attributeExists("APIDep") ) {
          AstAttribute * const attr = node->getAttribute("APIDep");
          ROSE_ASSERT( attr );

          // Note: the def-use analysis will point us at the defining
          // *statement* for a given variable. If that's an API call, it
          // could be a problem, but we'll need to know more to know
          // whether the definition is dependent to ommitted data.
          if ( debug ) {
              std::cout << "Attribute: " << attr->toString()    << std::endl;
              std::cout << "  " << node->unparseToString()      << std::endl;
          }

          const SgSymbol* const head = findFunctionHead(node);
          if ( head ) {
            bool omitted = false;

            APISpec * const spec = lookupFunction(specs, head->get_name());
            if ( spec ) {
              const ArgTreatments * const ts = spec->getFunction(head->get_name());
              ArgTreatments::const_iterator argAttr = ts->begin();
              for ( ; argAttr != ts->end() ; ++argAttr ) {
                  const GenericDepAttribute * const depAttr =
                      dynamic_cast<const GenericDepAttribute *>(*argAttr);
                  ROSE_ASSERT(depAttr != NULL);
                  omitted = omitted || spec->shouldOmit(depAttr);
              }
            }
            return omitted;
          }
          return false; // JED: added this because it's valid control-flow path, but unsure what it should return

        } else {
          return false;
        }
    }
};

#endif
