#include "costModel.h"
#include <boost/foreach.hpp>

#define foreach BOOST_FOREACH

void SimpleCostModel::setBranchCost(SgNode* node, const SimpleCostModel& cost, bool is_true_body)
{
    if (is_true_body)
        branch_cost_[node].first = cost;
    else
        branch_cost_[node].second = cost;
}

SimpleCostModel& SimpleCostModel::operator +=(const SimpleCostModel& cost)
{
    store_count_ += cost.store_count_;
    branch_cost_.insert(cost.branch_cost_.begin(), cost.branch_cost_.end());
    return *this;
}

bool SimpleCostModel::isZeroCost() const
{
    if (store_count_ != 0)
        return false;
    for (std::map<SgNode*, std::pair<SimpleCostModel, SimpleCostModel> >::const_iterator it =
            branch_cost_.begin(); it != branch_cost_.end(); ++it)
    {
        if (!it->second.first.isZeroCost())
            return false;
        if (!it->second.second.isZeroCost())
            return false;
    }
    return true;
}

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

bool operator <(const SimpleCostModel& cost1, const SimpleCostModel& cost2)
{
    if (cost1.store_count_ > cost2.store_count_)
        return false;

    typedef std::pair<SgNode*, std::pair<SimpleCostModel, SimpleCostModel> > BranchCost;
    foreach (const BranchCost& branch_cost, cost1.branch_cost_)
    {
        if (branch_cost.second.first.isZeroCost() &&
                branch_cost.second.second.isZeroCost())
            continue;
        if (cost2.branch_cost_.count(branch_cost.first) == 0)
            return false;
        if (cost2.branch_cost_.find(branch_cost.first)->second.first > branch_cost.second.first ||
                cost2.branch_cost_.find(branch_cost.first)->second.second > branch_cost.second.second)
            return false;
    }

    if (cost1 == cost2)
        return false;
    return true;
}