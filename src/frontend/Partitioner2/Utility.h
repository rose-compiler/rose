#ifndef ROSE_Partitioner2_Utility_H
#define ROSE_Partitioner2_Utility_H

#include <Partitioner2/AddressUsageMap.h>
#include <Partitioner2/BasicBlock.h>
#include <Partitioner2/ControlFlowGraph.h>
#include <Partitioner2/DataBlock.h>
#include <Partitioner2/Function.h>

#include "Diagnostics.h"

#include <algorithm>
#include <ostream>

namespace rose {
namespace BinaryAnalysis {
namespace Partitioner2 {

extern Sawyer::Message::Facility mlog;
void initDiagnostics();

bool sortBasicBlocksByAddress(const BasicBlock::Ptr&, const BasicBlock::Ptr&);
bool sortDataBlocksByAddress(const DataBlock::Ptr&, const DataBlock::Ptr&);
bool sortFunctionsByAddress(const Function::Ptr&, const Function::Ptr&);
bool sortByExpression(const BasicBlock::Successor&, const BasicBlock::Successor&);
bool sortVerticesByAddress(const ControlFlowGraph::ConstVertexNodeIterator&, const ControlFlowGraph::ConstVertexNodeIterator&);
bool sortEdgesBySrc(const ControlFlowGraph::ConstEdgeNodeIterator&, const ControlFlowGraph::ConstEdgeNodeIterator&);
bool sortEdgesByDst(const ControlFlowGraph::ConstEdgeNodeIterator&, const ControlFlowGraph::ConstEdgeNodeIterator&);
bool sortBlocksForAst(SgAsmBlock*, SgAsmBlock*);

template<class Container, class Comparator>
static bool
isSorted(const Container &container, Comparator sorted, bool distinct=true) {
    typename Container::const_iterator current = container.begin();
    if (current!=container.end()) {
        typename Container::const_iterator next = current;
        while (++next != container.end()) {
            if ((distinct && !sorted(*current, *next)) || sorted(*next, *current))
                return false;
            ++current;
        }
    }
    return true;
}

template<class Container>
static void
sortedInsert(Container &sortedFunctions /*in,out*/, const Function::Ptr &function) {
    ASSERT_not_null(function);
    ASSERT_require(isSorted(sortedFunctions, sortFunctionsByAddress, true));
    typename Container::iterator lb = std::lower_bound(sortedFunctions.begin(), sortedFunctions.end(), function,
                                                       sortFunctionsByAddress);
    if (lb==sortedFunctions.end() || (*lb)->address()!=function->address())
        sortedFunctions.insert(lb, function);
}

std::ostream& operator<<(std::ostream&, const AddressUser&);
std::ostream& operator<<(std::ostream&, const AddressUsers&);
std::ostream& operator<<(std::ostream&, const AddressUsageMap&);
std::ostream& operator<<(std::ostream&, const ControlFlowGraph::VertexNode&);
std::ostream& operator<<(std::ostream&, const ControlFlowGraph::EdgeNode&);



} // namespace
} // namespace
} // namespace

#endif
