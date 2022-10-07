#ifndef LANGUAGE_RESTRICTOR_COLLECTION
#define LANGUAGE_RESTRICTOR_COLLECTION

#include "LanguageRestrictor.h"

namespace CodeThorn {

  class LanguageRestrictorRers : public CodeThorn::LanguageRestrictor {
  public:
    void initialize();
  };

  class LanguageRestrictorC : public CodeThorn::LanguageRestrictorRers {
  public:
    void initialize();
  };
  
  class LanguageRestrictorCppSubset1 : public CodeThorn::LanguageRestrictorC {
  public:
  };

} // end of namespace CodeThorn

#endif
