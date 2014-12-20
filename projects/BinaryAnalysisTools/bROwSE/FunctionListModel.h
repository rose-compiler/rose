#ifndef bROwSE_FunctionListModel_H
#define bROwSE_FunctionListModel_H

#include <bROwSE/bROwSE.h>
#include <bROwSE/Statistics.h>
#include <Partitioner2/Function.h>
#include <Wt/WAbstractTableModel>
#include <Wt/WModelIndex>

namespace bROwSE {

// Model storing a list of functions.
class FunctionListModel: public Wt::WAbstractTableModel {
    Context &ctx_;
    std::vector<P2::Function::Ptr> functions_;
    FpStatistics heatStats_;
public:
    // To change the order of columns in the table, just change them in this enum.
    enum Column {
        C_ENTRY,                                        // function's primary entry address
        C_NAME,                                         // name if known, else empty string
        C_SIZE,                                         // size of function extent (code and data)
        C_NCALLERS,                                     // number of incoming calls
        C_NRETURNS,                                     // how many return edges, i.e., might the function return?
        C_IMPORT,                                       // is function an import
        C_EXPORT,                                       // is function an export
        C_MAYRETURN,                                    // may function return to its caller?
        C_STACKDELTA,                                   // concrete stack delta
        C_NCOLS                                         // must be last
    };

    // Construct a model having the list of all functions in the partitioner
    FunctionListModel(Context &ctx): ctx_(ctx) {
        BOOST_FOREACH (const P2::Function::Ptr &function, ctx.partitioner.functions())
            functions_.push_back(function);
    }

    // Returns the function at the specified index, or null
    P2::Function::Ptr functionAt(size_t idx);

    /** Returns the index for the specified function. */
    Wt::WModelIndex functionIdx(const P2::Function::Ptr&) const;

    virtual int rowCount(const Wt::WModelIndex &parent=Wt::WModelIndex()) const ROSE_OVERRIDE;

    virtual int columnCount(const Wt::WModelIndex &parent=Wt::WModelIndex()) const ROSE_OVERRIDE;

    virtual boost::any headerData(int column, Wt::Orientation orientation=Wt::Horizontal,
                                  int role=Wt::DisplayRole) const ROSE_OVERRIDE;

    virtual boost::any data(const Wt::WModelIndex &index, int role=Wt::DisplayRole) const ROSE_OVERRIDE;

    /** Statistics about the lastest function heat calculatations.  Heat values are stored in function ATTR_HEAT attributes and
     *  are updated when the function table is sorted by a particular column. */
    const FpStatistics& heatStats() const { return heatStats_; }

    // Sort rows according to column number and order
    void sort(int column, Wt::SortOrder order);
};

} // namespace
#endif
