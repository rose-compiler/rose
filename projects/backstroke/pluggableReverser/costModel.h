#ifndef BACKSTROKE_COSTMODEL_H
#define	BACKSTROKE_COSTMODEL_H

#include <map>

class SgNode;

/*! This simple cost model just records the count of stores to stacks. */
class SimpleCostModel
{
    int store_count_;
    
    /*! This map stores the number of stores in true and false bodies for
    each branch. */
    std::map<SgNode*, std::pair<SimpleCostModel, SimpleCostModel> > branch_cost_;

    friend bool operator <(const SimpleCostModel& cost1, const SimpleCostModel& cost2);
    friend bool operator ==(const SimpleCostModel& cost1, const SimpleCostModel& cost2);

public:

    SimpleCostModel()
    : store_count_(0)
    {}

    void increaseStoreCount(int size = 0) { ++store_count_; }

    void setBranchCost(SgNode* node, const SimpleCostModel& cost, bool is_true_body = true)
    {
        if (is_true_body)
            branch_cost_[node].first = cost;
        else
            branch_cost_[node].second = cost;
    }

    int getCost() const { return store_count_; }

    SimpleCostModel& operator += (const SimpleCostModel& cost)
    {
        store_count_ += cost.store_count_;
        return *this;
    }

    bool isZeroCost() const
    {
        return true;
    }
};

bool operator ==(const SimpleCostModel& cost1, const SimpleCostModel& cost2);
inline bool operator !=(const SimpleCostModel& cost1, const SimpleCostModel& cost2)
{ return !(cost1 == cost2); }

inline bool operator <(const SimpleCostModel& cost1, const SimpleCostModel& cost2)
{
    return cost1.getCost() < cost2.getCost();
}

inline bool operator >(const SimpleCostModel& cost1, const SimpleCostModel& cost2)
{
    return cost2 < cost1;
}






#endif	/* BACKSTROKE_COSTMODEL_H */

