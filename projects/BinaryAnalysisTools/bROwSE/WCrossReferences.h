#ifndef bROwSE_WCrossReferences_H
#define bROwSE_WCrossReferences_H

#include <bROwSE/bROwSE.h>
#include <Wt/WContainerWidget>

namespace bROwSE {

class CrossReferencesModel;

class WCrossReferences: public Wt::WContainerWidget {
    P2::ReferenceSet refs_;
    Wt::WText *wTableName_;
    CrossReferencesModel *model_;
    Wt::WTableView *view_;
    Wt::Signal<P2::Reference> referenceClicked_;
public:
    explicit WCrossReferences(Wt::WContainerWidget *parent=NULL)
        : Wt::WContainerWidget(parent), wTableName_(NULL), model_(NULL), view_(NULL) {
        init();
    }

    const P2::ReferenceSet &refs() const { return refs_; }
    void refs(const P2::ReferenceSet &rs);

    void name(const std::string &);

    Wt::Signal<P2::Reference>& referenceClicked() { return referenceClicked_; }

private:
    void init();
    void handleRowClicked(const Wt::WModelIndex&);
};

} // namespace
#endif
