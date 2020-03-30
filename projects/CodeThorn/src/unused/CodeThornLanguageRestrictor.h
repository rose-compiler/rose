#ifndef CODETHORN_LANGUAGE_RESTRICTOR_H
#define CODETHORN_LANGUAGE_RESTRICTOR_H

#include "LanguageRestrictor.h"
#include "LanguageRestrictorCollection.h"

namespace CodeThorn {
  class CodeThornLanguageRestrictor : public LanguageRestrictorCppSubset1 {
  public:
    void initialize();
  };
}

#endif
