// WARNING: Changes to this file must be contributed back to Sawyer or else they will
//          be clobbered by the next update from Sawyer.  The Sawyer repository is at
//          https://github.com/matzke1/sawyer.




#ifndef Sawyer_Document_PodMarkup_H
#define Sawyer_Document_PodMarkup_H

#include <Sawyer/DocumentBaseMarkup.h>

namespace Sawyer {
namespace Document {

class SAWYER_EXPORT PodMarkup: public BaseMarkup {
public:
    PodMarkup() { init(); }

    virtual void emit(const std::string &doc) /*override*/;

private:
    void init();
    virtual std::string finalizeDocument(const std::string&s) /*override*/;
};

} // namespace
} // namespace

#endif
