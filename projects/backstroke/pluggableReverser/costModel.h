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

    //! Increase the store count by 1.
    void increaseStoreCount(int size = 0) { ++store_count_; }

    //! This method set the cost of an if statment, which contains costs of true and false body.
    void setBranchCost(SgNode* node, const SimpleCostModel& cost, bool is_true_body = true);

    //! This is method will be obsoleted.
    int getCost() const { return store_count_; }

    //! Add the given cost to the current one.
    SimpleCostModel& operator += (const SimpleCostModel& cost);

    //! Tell if the current cost is zero cost.
    bool isZeroCost() const;
};

//! Equality operator. Note that this comparison is very restricted.
bool operator ==(const SimpleCostModel& cost1, const SimpleCostModel& cost2);
inline bool operator !=(const SimpleCostModel& cost1, const SimpleCostModel& cost2)
{ return !(cost1 == cost2); }

/*! Less than operator. This comparison is very conservative that if cost1 is less than
cost2, every branch cost in cost2 should not be greater than that cost in the corresponding
 branch of cost1.*/
bool operator <(const SimpleCostModel& cost1, const SimpleCostModel& cost2);
inline bool operator >(const SimpleCostModel& cost1, const SimpleCostModel& cost2)
{ return cost2 < cost1; }






#endif	/* BACKSTROKE_COSTMODEL_H */

