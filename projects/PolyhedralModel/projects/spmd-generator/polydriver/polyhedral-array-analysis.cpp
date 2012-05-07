
#include "polydriver/polyhedral-array-analysis.hpp"
#include "polydriver/polyhedral-driver.hpp"
#include "polydriver/polyhedral-utils.hpp"
#include "common/spmd-tree.hpp"

#include <queue>
#include <iostream>

#include "rose.h"

void PolyArrayAnalysis::collectAccess(SPMD_Tree * tree) {
  std::vector<std::pair<std::set<ArrayPartition *>, std::set<ArrayPartition *> > > children_accesses;
  std::vector<SPMD_Tree *>::iterator it_child;
  for (it_child = tree->getChildren().begin(); it_child != tree->getChildren().end(); it_child++) {
    collectAccess(*it_child);
    children_accesses.push_back(accesses_map[*it_child]);
  }
  SPMD_NativeStmt * stmt = dynamic_cast<SPMD_NativeStmt *>(tree);
  if (stmt != NULL) {
    assert(children_accesses.size() == 0);

    // We have to access the statement from the ScopTree as we make copy of the statements    
    std::map<SPMD_Tree *, ScopTree *>::iterator it_trees_map = polydriver->trees_map.find(tree);
    assert(it_trees_map != polydriver->trees_map.end());
    assert(it_trees_map->second->isStatement());
    SgStatement * sg_stmt = ((ScopStatement *)it_trees_map->second)->getExpression();

    assert(sg_stmt != NULL);
    SgExprStatement * expr_stmt = isSgExprStatement(sg_stmt);
    assert(expr_stmt != NULL); // FIXME only supported case now. (would imply a different driver: max cover)

    PolyhedricAnnotation::DataAccess<SgStatement, SgExprStatement, RoseVariable> & data_access =
        PolyhedricAnnotation::getDataAccess<SgStatement, SgExprStatement, RoseVariable>(expr_stmt);

    std::vector<std::pair<RoseVariable, std::vector<std::vector<std::pair<RoseVariable, int> > > > >::iterator it_access;

    std::set<ArrayPartition *> & in  = accesses_map[tree].first;
    std::vector<std::pair<RoseVariable, std::vector<std::vector<std::pair<RoseVariable, int> > > > > * reads = data_access.regenRead();
    for (it_access = reads->begin(); it_access != reads->end(); it_access++) {
      RoseVariable & var = it_access->first;
      ArrayPartition * base_array = NULL;
      std::map<RoseVariable, ArrayPartition *>::iterator it_base_array = whole_array_partition.find(var);
      if (it_base_array != whole_array_partition.end())
        base_array = it_base_array->second;
      else {
        base_array = new ArrayPartition(var);
        whole_array_partition.insert(std::pair<RoseVariable, ArrayPartition *>(var, base_array));
      }
      assert(base_array != NULL);

      // FIXME From here it will have to instantiate a more evolve version of ArrayPartition
      ArrayPartition * array = base_array; // TODO restrict using context (here one point)

      in.insert(array);
    }
    delete reads;

    std::set<ArrayPartition *> & out = accesses_map[tree].second;
    std::vector<std::pair<RoseVariable, std::vector<std::vector<std::pair<RoseVariable, int> > > > > * writes = data_access.regenWrite();
    for (it_access = writes->begin(); it_access != writes->end(); it_access++) {
      RoseVariable & var = it_access->first;
      ArrayPartition * base_array = NULL;
      std::map<RoseVariable, ArrayPartition *>::iterator it_base_array = whole_array_partition.find(var);
      if (it_base_array != whole_array_partition.end())
        base_array = it_base_array->second;
      else {
        base_array = new ArrayPartition(var);
        whole_array_partition.insert(std::pair<RoseVariable, ArrayPartition *>(var, base_array));
      }
      assert(base_array != NULL);

      // FIXME From here it will have to instantiate a more evolve version of ArrayPartition
      ArrayPartition * array = base_array; // TODO restrict using context (here one point, function of iterator)

      out.insert(array);
    }
    delete writes;

    // TODO merge partitions if same array: set of points, could use fixed points algo
  }
  else {
    std::vector<std::pair<std::set<ArrayPartition *>, std::set<ArrayPartition *> > >::iterator it;
    // TODO merge partitions if same array: set of points, could use fixed points algo
    for (it = children_accesses.begin(); it != children_accesses.end(); it++) {
      accesses_map[tree].first.insert(it->first.begin(), it->first.end());
      accesses_map[tree].second.insert(it->second.begin(), it->second.end());
    }
    // TODO add informations from SPMD_Loop or SPMD_DomainRestriction
  }
}

PolyArrayAnalysis::PolyArrayAnalysis(PolyDriver * polydriver_) :
  ArrayAnalysis(),
  polydriver(polydriver_),
  whole_array_partition()
{}

PolyArrayAnalysis::~PolyArrayAnalysis() {}

void PolyArrayAnalysis::process(SPMD_Root * tree) {
  // Start by "touching" each node (for initialization)
  std::queue<SPMD_Tree *> queue;
  queue.push(tree);
  while (!queue.empty()) {
    SPMD_Tree * tmp = queue.front();
    queue.pop();
    accesses_map.insert(std::pair<SPMD_Tree *, std::pair<std::set<ArrayPartition *>, std::set<ArrayPartition *> > >(
      tmp, std::pair<std::set<ArrayPartition *>, std::set<ArrayPartition *> >(std::set<ArrayPartition *>(), std::set<ArrayPartition *>())
    ));
    std::vector<SPMD_Tree *>::iterator it_child;
    for (it_child = tmp->getChildren().begin(); it_child != tmp->getChildren().end(); it_child++)
      queue.push(*it_child);
  }

  // Then recursively collect the accesses
  collectAccess(tree);
}

std::set<CommDescriptor *> PolyArrayAnalysis::genComm(SPMD_Tree * t1, SPMD_Tree * t2, NodePlacement & placement) {
  std::set<CommDescriptor *> res;

  // TODO

  return res;
}

std::set<SyncDescriptor *> PolyArrayAnalysis::genSync(SPMD_Tree * t1, SPMD_Tree * t2, NodePlacement & placement) {
  std::set<SyncDescriptor *> res;

  // TODO

  return res;
}

