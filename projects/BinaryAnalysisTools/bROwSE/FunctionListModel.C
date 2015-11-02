#include <rose.h>
#include <bROwSE/FunctionListModel.h>
#include <bROwSE/FunctionUtil.h>
#include <boost/bind.hpp>

namespace bROwSE {

void
FunctionListModel::reload() {
    layoutAboutToBeChanged().emit();
    functions_.clear();
    if (!ctx_.partitioner.isDefaultConstructed()) {
        BOOST_FOREACH (const P2::Function::Ptr &function, ctx_.partitioner.functions())
            functions_.push_back(function);
    }
    layoutChanged().emit();
}

P2::Function::Ptr
FunctionListModel::functionAt(size_t idx) {
    return idx < functions_.size() ? functions_[idx] : P2::Function::Ptr();
}

Wt::WModelIndex
FunctionListModel::functionIdx(const P2::Function::Ptr &function) const {
    for (size_t i=0; i<functions_.size(); ++i) {
        if (functions_[i] == function)
            return index(i, 0);
    }
    return Wt::WModelIndex();
}

int
FunctionListModel::rowCount(const Wt::WModelIndex &parent) const {
    return parent.isValid() ? 0 : functions_.size();
}

int
FunctionListModel::columnCount(const Wt::WModelIndex &parent) const {
    return parent.isValid() ? 0 : analyzers_.size();
}

boost::any
FunctionListModel::headerData(int column, Wt::Orientation orientation, int role) const {
    if (Wt::Horizontal == orientation) {
        if (Wt::DisplayRole == role) {
            ASSERT_require(column>=0 && (size_t)column < analyzers_.size());
            return analyzers_[column]->header();
        } else if (Wt::ToolTipRole == role) {
            return analyzers_[column]->toolTip();
        }
    }
    return boost::any();
}

boost::any
FunctionListModel::data(const Wt::WModelIndex &index, int role) const {
    ASSERT_require(index.isValid());
    ASSERT_require(index.row()>=0 && (size_t)index.row() < functions_.size());
    P2::Function::Ptr function = functions_[index.row()];
    if (Wt::DisplayRole == role) {
        ASSERT_require(index.column()>=0 && (size_t)index.column() < analyzers_.size());
        return analyzers_[index.column()]->data(ctx_.partitioner, function);
    }
    return boost::any();
}

void
FunctionListModel::sort(int column, Wt::SortOrder order) {
    ASSERT_require(column>=0 && (size_t)column < analyzers_.size());

    // Make sure all the values are computed and cached if necessary before we sort.
    BOOST_FOREACH (const P2::Function::Ptr &function, functions_)
        (void) analyzers_[column]->data(ctx_.partitioner, function);

    // Sort
    layoutAboutToBeChanged().emit();
    if (Wt::AscendingOrder == order) {
        std::sort(functions_.begin(), functions_.end(),
                  boost::bind(&FunctionAnalyzer::isAscending, getRawPointer(analyzers_[column]), _1, _2));
    } else {
        std::sort(functions_.begin(), functions_.end(),
                  boost::bind(&FunctionAnalyzer::isDescending, getRawPointer(analyzers_[column]), _1, _2));
    }
    
    layoutChanged().emit();

    // Calculate and cache values for ATTR_Heat used by WAddressSpace when calculating colors for a heat map.
    std::vector<double> dv;
    dv.reserve(functions_.size());
    heatStats_ = FpStatistics();
    BOOST_FOREACH (const P2::Function::Ptr &function, functions_) {
        double d = analyzers_[column]->heatValue(ctx_.partitioner, function);
        function->attribute(ATTR_Heat, d);
        if (!isnan(d)) {
            heatStats_.insert(d);
            dv.push_back(d);
        }
    }
    heatStats_.computeRanks(dv);
}

} // namespace
