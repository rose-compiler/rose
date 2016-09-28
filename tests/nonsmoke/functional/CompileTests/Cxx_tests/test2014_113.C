#include <vector>

typedef struct Extents_s {} Extents_t ;

#include "boost/function.hpp"

namespace XXX {

class UnstructuredBlockPartitioner {
public:
    typedef boost::function<double (std::vector<Extents_t> const &)> CostFunction;

    UnstructuredBlockPartitioner();

private:
    CostFunction getCostFunction() const;
    CostFunction m_cost_function;
};

} // namespace XXX


namespace XXX 
   {

     double maxPartitionSizeCost(std::vector<Extents_t> const &partitions);

   } // namespace XXX

namespace XXX {

UnstructuredBlockPartitioner::CostFunction
UnstructuredBlockPartitioner::getCostFunction() const
   {
     if (m_cost_function) 
        {
        }

     return maxPartitionSizeCost;
   }

} /* namespace XXX */
