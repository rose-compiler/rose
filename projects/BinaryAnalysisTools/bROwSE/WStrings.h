#ifndef bROwSE_WStrings_H
#define bROwSE_WStrings_H

#include <bROwSE/bROwSE.h>
#include <Wt/WContainerWidget>

namespace bROwSE {

class StringsModel;

/** Displays information about strings found in a specimen. */
class WStrings: public Wt::WContainerWidget {
    WAddressSpace *wAddressSpace_;
    StringsModel *model_;
    Wt::WTableView *wTableView_;
    Wt::WText *wSearchResults_;
    Wt::Signal<size_t> stringClicked_;

    enum AddressSpaceBar { SegmentsBar, StringsBar };

public:
    explicit WStrings(Wt::WContainerWidget *parent=NULL)
        : Wt::WContainerWidget(parent), wAddressSpace_(NULL), model_(NULL), wTableView_(NULL), wSearchResults_(NULL) {
        init();
    }

    const MemoryMap& memoryMap() const;
    void memoryMap(const MemoryMap&);

    /** Update string table from new partitioner.
     *
     *  Update the model using the partitioner's memory map and initializes cross reference information from the partitioned
     *  instructions. */
    void partitioner(const P2::Partitioner&);

    /** Emitted when a row of the table is clicked. Argument is the row number. */
    Wt::Signal<size_t>& stringClicked() { return stringClicked_; }

    /** Return cross reference info for a particular string. */
    const P2::ReferenceSet& crossReferences(size_t stringIdx);

private:
    void init();
    void search(Wt::WLineEdit*);
    void selectStringByRow(const Wt::WModelIndex &idx);
};

} // namespace
#endif
