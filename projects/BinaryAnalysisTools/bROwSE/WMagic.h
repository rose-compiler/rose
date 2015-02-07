#ifndef bROwSE_WMagic_H
#define bROwSE_WMagic_H

#include <bROwSE/bROwSE.h>

#include <Wt/WContainerWidget>

namespace bROwSE {

class MagicModel;

/** Displays information about magic numbers found in a specimen. */
class WMagic: public Wt::WContainerWidget {
public:
    MagicModel *model_;
    Wt::WTableView *wTableView_;

public:
    explicit WMagic(Wt::WContainerWidget *parent=NULL)
        : Wt::WContainerWidget(parent), model_(NULL), wTableView_(NULL) {
        init();
    }

    const MemoryMap& memoryMap() const;
    void memoryMap(const MemoryMap&);

private:
    void init();
    void search(Wt::WLineEdit*);
    void searchNavigate(int direction);
};

} // namespace
#endif
