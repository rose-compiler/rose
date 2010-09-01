#include "costModel.h"
#include <boost/foreach.hpp>

#define foreach BOOST_FOREACH

bool operator ==(const SimpleCostModel& cost1, const SimpleCostModel& cost2)
{
    if (cost1.store_count_ != cost2.store_count_)
        return false;

    typedef std::pair<SgNode*, std::pair<SimpleCostModel, SimpleCostModel> > BranchCost;
    foreach (const BranchCost& branch_cost, cost1.branch_cost_)
    {
        if (branch_cost.second.first.isZeroCost() &&
                branch_cost.second.second.isZeroCost())
            continue;
        if (cost2.branch_cost_.count(branch_cost.first) == 0)
            return false;
        if (cost2.branch_cost_.find(branch_cost.first)->second.first != branch_cost.second.first ||
                cost2.branch_cost_.find(branch_cost.first)->second.second != branch_cost.second.second)
            return false;
    }
    foreach (const BranchCost& branch_cost, cost2.branch_cost_)
    {
        if (branch_cost.second.first.isZeroCost() &&
                branch_cost.second.second.isZeroCost())
            continue;
        if (cost1.branch_cost_.count(branch_cost.first) == 0)
            return false;
        /*
        if (cost1.branch_cost_[branch_cost.first].first != branch_cost.second.first ||
                cost1.branch_cost_[branch_cost.first].second != branch_cost.second.second)
            return false;
        */
    }

    return true;
}
