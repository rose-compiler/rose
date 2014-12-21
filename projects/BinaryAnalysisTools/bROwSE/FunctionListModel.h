#ifndef bROwSE_FunctionListModel_H
#define bROwSE_FunctionListModel_H

#include <bROwSE/bROwSE.h>
#include <bROwSE/FunctionUtil.h>
#include <bROwSE/Statistics.h>
#include <Partitioner2/Function.h>
#include <Wt/WAbstractTableModel>
#include <Wt/WModelIndex>

namespace bROwSE {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Model for storing a list of functions and their associated analysis results. The analyses are calculated on demand. */
class FunctionListModel: public Wt::WAbstractTableModel {
    Context &ctx_;
    std::vector<P2::Function::Ptr> functions_;
    FpStatistics heatStats_;
    std::vector<FunctionAnalyzer::Ptr> analyzers_;

public:
    /** Construct a model having the list of all functions in the partitioner. */
    FunctionListModel(Context &ctx): ctx_(ctx) {
        BOOST_FOREACH (const P2::Function::Ptr &function, ctx.partitioner.functions())
            functions_.push_back(function);
    }

    /** Returns the function at the specified index, or null. */
    P2::Function::Ptr functionAt(size_t idx);

    /** Returns the index for the specified function. */
    Wt::WModelIndex functionIdx(const P2::Function::Ptr&) const;

    /** Array of table columns.  The values in this array should not be changed once the table is being used. I.e., only change
     * them when first constructing the model.
     *
     * @{ */
    const std::vector<FunctionAnalyzer::Ptr>& analyzers() const { return analyzers_; }
    std::vector<FunctionAnalyzer::Ptr>& analyzers() { return analyzers_; }
    /** @} */

    /** Statistics about the lastest function heat calculatations.  Heat values are stored in function ATTR_HEAT attributes and
     *  are updated when the function table is sorted by a particular column. */
    const FpStatistics& heatStats() const { return heatStats_; }

    virtual int rowCount(const Wt::WModelIndex &parent=Wt::WModelIndex()) const ROSE_OVERRIDE;

    virtual int columnCount(const Wt::WModelIndex &parent=Wt::WModelIndex()) const ROSE_OVERRIDE;

    virtual boost::any headerData(int column, Wt::Orientation orientation=Wt::Horizontal,
                                  int role=Wt::DisplayRole) const ROSE_OVERRIDE;

    virtual boost::any data(const Wt::WModelIndex &index, int role=Wt::DisplayRole) const ROSE_OVERRIDE;

    // Sort rows according to column number and order
    void sort(int column, Wt::SortOrder order);
};

} // namespace
#endif
