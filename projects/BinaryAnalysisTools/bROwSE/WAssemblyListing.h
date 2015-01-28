#ifndef bROwSE_WAssemblyListing_H
#define bROwSE_WAssemblyListing_H

#include <bROwSE/bROwSE.h>
#include <Wt/WContainerWidget>

namespace bROwSE {

class WAssemblyListing: public Wt::WContainerWidget {
    Context &ctx_;
    P2::Function::Ptr function_;                        // currently-displayed function
    Wt::WText *wText_;                                  // the listing itself, pretty simple for now

public:
    WAssemblyListing(Context &ctx, Wt::WContainerWidget *parent=NULL)
        : Wt::WContainerWidget(parent), ctx_(ctx), wText_(NULL) {
        init();
    }

    // Returns currently displayed function
    P2::Function::Ptr function() const {
        return function_;
    }

    // Display information for the specified function
    void changeFunction(const P2::Function::Ptr&);
    
private:
    void init();
};

} // namespace
#endif
