
#include "common/placement.hpp"

#include "common/spmd-tree.hpp"
#include "common/compute-system.hpp"
#include "common/array-analysis.hpp"
#include "common/comm-analysis.hpp"

#include "toolboxes/algebra-container.hpp"

#include <queue>
#include <cassert>

#include "rose.h"

#define DEBUG 1

Conditions::Conditions() {}
Conditions::Conditions(const Conditions & arg) {}
Conditions::~Conditions() {}

NodePlacement::NodePlacement(ComputeSystem * system_) :
  system(system_)
{}

NodePlacement::~NodePlacement() {
  if (system != NULL)
    delete system;
}

void NodePlacement::clear() {}

void NodePlacement::collectAliasing(
  SPMD_Root * root,
  ArrayAnalysis & array_analysis,
  std::map<ComputeSystem *, std::pair<std::set<ArrayPartition *>, std::set<ArrayPartition *> > > & to_be_aliased
) const {
  std::queue<SPMD_Tree *> queue;
  queue.push(root);
  std::vector<SPMD_KernelCall *> kernel_calls;
  while (!queue.empty()) {
    SPMD_Tree * tree = queue.front();
    queue.pop();
    SPMD_KernelCall * kernel_call = dynamic_cast<SPMD_KernelCall *>(tree);
    if (kernel_call != NULL)
      kernel_calls.push_back(kernel_call);
    // go done even for 'SPMD_KernelCall' as it can have multiple level of it (MPI+Ocl or fine grained GPU placement)
    std::vector<SPMD_Tree *> & children = tree->getChildren();
    std::vector<SPMD_Tree *>::iterator it_child;
    for (it_child = children.begin(); it_child != children.end(); it_child++)
      queue.push(*it_child);
  }
  std::vector<SPMD_KernelCall *>::iterator it_kernel_call;
  for (it_kernel_call = kernel_calls.begin(); it_kernel_call != kernel_calls.end(); it_kernel_call++) {
    ComputeSystem * placement = assigned(*it_kernel_call);
    const std::pair<std::set<ArrayPartition *>, std::set<ArrayPartition *> > & in_out = array_analysis.get(*it_kernel_call);
    std::set<ArrayPartition *>::const_iterator it_array;
    std::map<ComputeSystem *, std::pair<std::set<ArrayPartition *>, std::set<ArrayPartition *> > >::iterator it_part_set =
        to_be_aliased.find(placement);
    assert(it_part_set != to_be_aliased.end());
    for (it_array = in_out.first.begin(); it_array != in_out.first.end(); it_array++)
      if ((*it_array)->getDimensions().size() > 0)
        it_part_set->second.first.insert(*it_array);
    for (it_array = in_out.second.begin(); it_array != in_out.second.end(); it_array++)
      if ((*it_array)->getDimensions().size() > 0)
        it_part_set->second.second.insert(*it_array);
      else {
        // if a kernel write a scalar: we need to privatize it
      }
    std::set<ArrayPartition *>::iterator it_rem;
    for (it_rem = it_part_set->second.second.begin(); it_rem != it_part_set->second.second.end(); it_rem++)
      it_part_set->second.first.erase(*it_rem);
  }
}

#if DEBUG
void printValidData(
  std::map<SPMD_Tree *, std::map<ArrayPartition *, std::map<ComputeSystem *, std::vector<Conditions *> > > > valid_datas
) {
  std::map<SPMD_Tree *, std::map<ArrayPartition *, std::map<ComputeSystem *, std::vector<Conditions *> > > >::iterator it_tree;
  std::map<ArrayPartition *, std::map<ComputeSystem *, std::vector<Conditions *> > >::iterator it_array;
  std::map<ComputeSystem *, std::vector<Conditions *> >::iterator it_cs;
  std::vector<Conditions *>::iterator it_cond;

  std::ofstream out;
  out.open("test.dot");

  out << "digraph {" << std::endl;

  for (it_tree = valid_datas.begin(); it_tree != valid_datas.end(); it_tree++) {
    out << "\tn_" << it_tree->first << " [label=\"";
    it_tree->first->print(out); out << "\\nArrays:\\n";
    for (it_array = it_tree->second.begin(); it_array != it_tree->second.end(); it_array++) {
      out << "\t" << it_array->first->getUniqueName() << " is on:\\n";
      for (it_cs = it_array->second.begin(); it_cs != it_array->second.end(); it_cs++) {
        out << "\t\t" << it_cs->first->getID() << " if:\\n";
        for (it_cond = it_cs->second.begin(); it_cond != it_cs->second.end(); it_cond++) {
          if (*it_cond != NULL) {
            out << "\t\t\t";
            (*it_cond)->print(out);
            out << "\\n";
          }
          else {
            out << "\t\t\tTRUE\\n";
          }
        }
      }
    }
    out << "\"]" << std::endl;
    std::vector<SPMD_Tree *> & children = it_tree->first->getChildren();
    std::vector<SPMD_Tree *>::iterator it_child;
    for (it_child = children.begin(); it_child != children.end(); it_child++)
      out << "\t\tn_" << it_tree->first << " -> n_" << *it_child << std::endl;
  }

  out << "}" << std::endl;

  out.close();
}
#endif

void NodePlacement::generateCommSync(
  SPMD_Root * root,
  ArrayAnalysis & array_analysis
) const {
  ComputeSystem * default_compute_system = assigned(root);
  assert(default_compute_system != NULL);

#if DEBUG
  std::cerr << " Global Compute System ID = " << system->getID() << std::endl;
  std::cerr << "Default Compute System ID = " << default_compute_system->getID() << std::endl;
#endif

  std::set<ArrayPartition *> * partitions = array_analysis.get_partitions(root);
  assert(partitions != NULL && partitions->size() > 0);
  // TODO partition will have been merge... (not currently)
  std::set<ArrayPartition *>::iterator it_partition;

  assert(partitions->size() >= 0);

// start new impl
  std::map<ArrayPartition *, std::map<ComputeSystem *, std::vector<Conditions *> > > original_position;
  for (it_partition = partitions->begin(); it_partition != partitions->end(); it_partition++) {
    original_position.insert(std::pair<ArrayPartition *, std::map<ComputeSystem *, std::vector<Conditions *> > >(
      *it_partition, std::map<ComputeSystem *, std::vector<Conditions *> >()
    )).first->second.insert(std::pair<ComputeSystem *, std::vector<Conditions *> >(
      default_compute_system, std::vector<Conditions *>()
    )).first->second.push_back(genAssociatedFullCondition());
  }

  std::map<SPMD_Tree *, std::map<ArrayPartition *, std::map<ComputeSystem *, std::vector<Conditions *> > > > valid_datas;
  positionValidData(root, array_analysis, valid_datas, original_position, genAssociatedFullCondition());

  delete partitions;

#if DEBUG
  printValidData(valid_datas);

  std::cerr << std::endl << std::endl;
#endif

  placeCommSync(root, array_analysis, valid_datas, original_position, genAssociatedFullCondition());

  std::map<SPMD_Tree *, std::map<ArrayPartition *, std::map<ComputeSystem *, std::vector<Conditions *> > > >::iterator it_valid_datas_root = 
      valid_datas.find(root);
  assert(it_valid_datas_root != valid_datas.end());
  const std::pair<std::set<ArrayPartition *>, std::set<ArrayPartition *> > & in_out = array_analysis.get(root);
  std::set<ArrayPartition *>::iterator it_out;
  for (it_out = in_out.second.begin(); it_out != in_out.second.end(); it_out++) {
    std::map<ArrayPartition *, std::map<ComputeSystem *, std::vector<Conditions *> > >::iterator it_pos = it_valid_datas_root->second.find(*it_out);
    assert(it_pos != it_valid_datas_root->second.end());
    std::vector<std::pair<std::vector<Conditions *>, ComputeSystem *> > sources;
    findSources(default_compute_system, it_pos->second, sources, genAssociatedFullCondition());
    std::vector<std::pair<std::vector<Conditions *>, ComputeSystem *> >::iterator it_sources;
    for (it_sources = sources.begin(); it_sources != sources.end(); it_sources++) {
      CommDescriptor * comm_descriptor = new CommDescriptor(it_sources->second, default_compute_system, *it_out); // TODO sync
      SPMD_Comm * comm = new SPMD_Comm(root, comm_descriptor, it_sources->first);
      root->appendChild(comm); 
    }
  }

}

void NodePlacement::findSources(
  ComputeSystem * destination,
  const std::map<ComputeSystem *, std::vector<Conditions *> > & current_position,
  std::vector<std::pair<std::vector<Conditions *>, ComputeSystem *> > & results_sources,
  Conditions * context
) const {

#if DEBUG
  std::cerr << "\tStart findSources:" << std::endl;
  std::cerr << "\t|\t\tdestination = " << destination->getID() << std::endl;

  std::map<ComputeSystem *, std::vector<Conditions *> >::const_iterator it_cs_map;
  std::vector<Conditions *>::const_iterator it_cond;

  for (it_cs_map = current_position.begin(); it_cs_map != current_position.end(); it_cs_map++) {
    std::cerr << "\t>\t\t" << it_cs_map->first->getID() << " if:" << std::endl;
    for (it_cond = it_cs_map->second.begin(); it_cond != it_cs_map->second.end(); it_cond++) {
      if (*it_cond != NULL) {
        std::cerr << "\t>\t\t\t";
        (*it_cond)->print(std::cerr);
        std::cerr << std::endl;
      }
      else {
        std::cerr << "\t>\t\t\tTRUE" << std::endl;
      }
    }
  }
#endif
  std::vector<std::pair<ComputeSystem *, std::vector<Conditions *> > > vect_current_position(current_position.begin(), current_position.end());
  assert(vect_current_position.size() == 1 || vect_current_position.size() == 2); // FIXME
  // TODO sort 'vect_current_position' by distance to 'destination' (could depend on the array...)

  std::vector<Conditions *> need_transfert;
  need_transfert.push_back(context);
  if (vect_current_position[0].first == destination) {
    removeFromCondSet(need_transfert, vect_current_position[0].second);
  }
  else if (vect_current_position.size() == 2 && vect_current_position[1].first == destination) {
    removeFromCondSet(need_transfert, vect_current_position[1].second);
  }

#if DEBUG
  std::cerr << "\t|\t\tTransfert needed for:" << std::endl; 
  for (it_cond = need_transfert.begin(); it_cond != need_transfert.end(); it_cond++) {
    if (*it_cond != NULL) {
      std::cerr << "\t|\t\t|\t";
      (*it_cond)->print(std::cerr);
      std::cerr << std::endl;
    }
    else {
      std::cerr << "\t|\t\t|\tTRUE" << std::endl;
    }
  }
#endif

  if (need_transfert.size() > 0) {
    ComputeSystem * source = NULL;
    if (vect_current_position.size() == 1) source = vect_current_position[0].first;
    else {
      source = vect_current_position[0].first == destination ? vect_current_position[1].first : vect_current_position[0].first;
    }
    assert(source != NULL && source != destination);
    results_sources.push_back(std::pair<std::vector<Conditions *>, ComputeSystem *>(need_transfert, source));
  }

#if DEBUG
  std::cerr << "\tEnd   findSources" << std::endl;
#endif
}

void NodePlacement::removeFromCondSet(std::vector<Conditions *> & cond_set_1, const std::vector<Conditions *> & cond_set_2) const {
  std::vector<Conditions *>::const_iterator it;
  for (it = cond_set_2.begin(); it != cond_set_2.end(); it++)
    removeFromCondSet(cond_set_1, *it);
}

void NodePlacement::placeCommSync(
  SPMD_Tree * tree,
  ArrayAnalysis & array_analysis,
  const std::map<SPMD_Tree *, std::map<ArrayPartition *, std::map<ComputeSystem *, std::vector<Conditions *> > > > & valid_data,
  std::map<ArrayPartition *, std::map<ComputeSystem *, std::vector<Conditions *> > > original_position,
  Conditions * condition_to_reach_this_node
) const {

  SPMD_NativeStmt * native_stmt = dynamic_cast<SPMD_NativeStmt *>(tree);
  if (native_stmt != NULL) {
    // FIXME why not some check ???
    return;
  }

#if DEBUG
  std::cerr << "Start placeCommSync: ";
  tree->print(std::cerr);
  std::cerr << " (" << tree << "):" << std::endl;
#endif
 
  SPMD_Loop * loop = dynamic_cast<SPMD_Loop *>(tree);
  SPMD_DomainRestriction * domain_restriction = dynamic_cast<SPMD_DomainRestriction *>(tree);
  SPMD_KernelCall * kernel_call = dynamic_cast<SPMD_KernelCall *>(tree);

  if (loop != NULL)
    condition_to_reach_this_node = condition_to_reach_this_node->new_extended_by(loop);
  if (domain_restriction != NULL)
    condition_to_reach_this_node = condition_to_reach_this_node->new_restricted_by(domain_restriction);
  if (kernel_call != NULL) {
    condition_to_reach_this_node = condition_to_reach_this_node->new_extended_by(kernel_call);
    condition_to_reach_this_node = condition_to_reach_this_node->new_restricted_by(kernel_call);
  }

  std::map<SPMD_Tree *, std::map<ArrayPartition *, std::map<ComputeSystem *, std::vector<Conditions *> > > >::const_iterator it_valid_data =
      valid_data.find(tree);
  assert(it_valid_data != valid_data.end());

  std::vector<SPMD_Tree *> & children = tree->getChildren();
  std::set<ArrayPartition *>::const_iterator it_array;
  if (children.size() > 0) {
    SPMD_Tree * first_child = children[0];
    std::vector<SPMD_Tree *>::iterator it_child = children.begin();
    while (it_child != children.end()) {
      SPMD_Tree * child = *it_child;
      if (dynamic_cast<SPMD_DomainRestriction *>(child) == NULL && dynamic_cast<SPMD_Loop *>(child) == NULL) {
        std::set<ArrayPartition *> * partitions = array_analysis.get_partitions(child);
        ComputeSystem * placement = assigned(child);
        for (it_array = partitions->begin(); it_array != partitions->end(); it_array++) {
#if DEBUG
          std::cerr << "|\t\tCheck for partition " << (*it_array)->getUniqueName() << " for child " << child << std::endl;
#endif
          std::map<ArrayPartition *, std::map<ComputeSystem *, std::vector<Conditions *> > >::iterator it_array_original_position =
              original_position.find(*it_array);
          std::vector<std::pair<std::vector<Conditions *>, ComputeSystem *> > sources;
          if (loop != NULL && child == first_child) {
            std::vector<std::pair<std::vector<Conditions *>, ComputeSystem *> > tmp_sources;
            std::vector<std::pair<std::vector<Conditions *>, ComputeSystem *> >::iterator it_tmp_sources;

            findSources(placement, it_array_original_position->second, tmp_sources, condition_to_reach_this_node->new_restricted_by(loop, true));
            sources.insert(sources.end(), tmp_sources.begin(), tmp_sources.end());

            tmp_sources.clear();

            std::map<SPMD_Tree *, std::map<ArrayPartition *, std::map<ComputeSystem *, std::vector<Conditions *> > > >::const_iterator it_vd_tmp;
            it_vd_tmp = valid_data.find(loop->getChildren().back());
            std::map<ArrayPartition *, std::map<ComputeSystem *, std::vector<Conditions *> > >::const_iterator it_arr_tmp;
            it_arr_tmp = it_vd_tmp->second.find(*it_array);
            const std::map<ComputeSystem *, std::vector<Conditions *> > & pos_after_last_in_loop = it_arr_tmp->second;
            findSources(placement, pos_after_last_in_loop, tmp_sources, condition_to_reach_this_node->new_without_first_it(loop));
            sources.insert(sources.end(), tmp_sources.begin(), tmp_sources.end());
          }
          else {
            findSources(placement, it_array_original_position->second, sources, condition_to_reach_this_node);
          }
          std::vector<std::pair<std::vector<Conditions *>, ComputeSystem *> >::iterator it_sources;
          for (it_sources = sources.begin(); it_sources != sources.end(); it_sources++) {
#if DEBUG
            std::cerr << "|\t\t|\tInsert Communication" << std::endl;
            std::cerr << "|\t\t|\t|\tFrom: " << it_sources->second->getID() << std::endl;
            std::cerr << "|\t\t|\t|\tTo  : " << placement->getID() << std::endl;
            std::cerr << "|\t\t|\t|\tIf: " << std::endl;
            std::vector<Conditions *>::iterator it_cond;
            for (it_cond = it_sources->first.begin(); it_cond != it_sources->first.end(); it_cond++) {
              std::cerr << "|\t\t|\t|\t|\t";
              (*it_cond)->print(std::cerr);
              std::cerr << std::endl;
            }
#endif
            CommDescriptor * comm_descriptor = new CommDescriptor(it_sources->second, placement, *it_array); // TODO sync
            SPMD_Comm * comm = new SPMD_Comm(tree, comm_descriptor, it_sources->first);
            it_child = children.insert(it_child, comm);
          }

          std::vector<Conditions *> & cond_before_comm = it_array_original_position->second[placement]; // FIXME unsafe
          cond_before_comm.push_back(condition_to_reach_this_node->copy());
          simplify(cond_before_comm);
        }
      }
      if (dynamic_cast<SPMD_KernelCall *>(child) == NULL) // FIXME hack because there is problem passing position to child (ok because only 1 lvl)
        placeCommSync(child, array_analysis, valid_data, original_position, condition_to_reach_this_node);
      std::map<SPMD_Tree *, std::map<ArrayPartition *, std::map<ComputeSystem *, std::vector<Conditions *> > > >::const_iterator it_valid_data = 
          valid_data.find(child);
      assert(it_valid_data != valid_data.end());
      merge(original_position, it_valid_data->second); // merge because transfert may have been done by parents
      while (*it_child != child) it_child++; // FIXME is that necessary?
      it_child++;
    }
  }

#if DEBUG
  std::cerr << "End   placeCommSync" << std::endl;
#endif

}

void NodePlacement::simplify(
  std::map<ArrayPartition *, std::map<ComputeSystem *, std::vector<Conditions *> > > & map
) const {
  std::map<ArrayPartition *, std::map<ComputeSystem *, std::vector<Conditions *> > >::iterator it_arr_map;
  std::map<ComputeSystem *, std::vector<Conditions *> >::iterator it_cs_map;

  for (it_arr_map = map.begin(); it_arr_map != map.end(); it_arr_map++) {
    for (it_cs_map = it_arr_map->second.begin(); it_cs_map != it_arr_map->second.end(); it_cs_map++) {
      simplify(it_cs_map->second);
    }
  }
}

void NodePlacement::merge(
  std::map<ArrayPartition *, std::map<ComputeSystem *, std::vector<Conditions *> > > & map_inout,
  const std::map<ArrayPartition *, std::map<ComputeSystem *, std::vector<Conditions *> > > & map_in
) const {
  std::map<ArrayPartition *, std::map<ComputeSystem *, std::vector<Conditions *> > >::iterator it_arr_map_inout;
  std::map<ArrayPartition *, std::map<ComputeSystem *, std::vector<Conditions *> > >::const_iterator it_arr_map_in;
  std::map<ComputeSystem *, std::vector<Conditions *> >::iterator it_cs_map_inout;
  std::map<ComputeSystem *, std::vector<Conditions *> >::const_iterator it_cs_map_in;
  std::vector<Conditions *>::iterator it_cond_inout;
  std::vector<Conditions *>::const_iterator it_cond_in;

  assert(map_inout.size() == map_in.size());

  for (it_arr_map_in = map_in.begin(); it_arr_map_in != map_in.end(); it_arr_map_in++) {
    it_arr_map_inout = map_inout.find(it_arr_map_in->first);
    assert(it_arr_map_inout != map_inout.end());

    it_cs_map_in = it_arr_map_in->second.begin();
    it_cs_map_inout = it_arr_map_inout->second.begin();

    std::vector<std::pair<ComputeSystem *, std::vector<Conditions *> > > to_add;

    while (it_cs_map_in != it_arr_map_in->second.end() && it_cs_map_inout != it_arr_map_inout->second.end()) {
      if (it_cs_map_in->first < it_cs_map_inout->first) {
        to_add.push_back(std::pair<ComputeSystem *, std::vector<Conditions *> >(
            it_cs_map_in->first, std::vector<Conditions *>()
        ));
        for (it_cond_in = it_cs_map_in->second.begin(); it_cond_in != it_cs_map_in->second.end(); it_cond_in++)
          to_add.back().second.push_back((*it_cond_in)->copy());
        it_cs_map_in++;
      }
      else if (it_cs_map_in->first > it_cs_map_inout->first) {
        // nothing todo
        it_cs_map_inout++;
      }
      else {
        std::vector<Conditions *> merged_cond;
        // TODO something more sophisticated than simple copy
        for (it_cond_in = it_cs_map_in->second.begin(); it_cond_in != it_cs_map_in->second.end(); it_cond_in++)
          merged_cond.push_back((*it_cond_in)->copy());
        for (it_cond_inout = it_cs_map_inout->second.begin(); it_cond_inout != it_cs_map_inout->second.end(); it_cond_inout++)
          merged_cond.push_back((*it_cond_inout)->copy());
        it_cs_map_inout->second = merged_cond;
        it_cs_map_in++; it_cs_map_inout++;
      }
    }
    assert(it_cs_map_in == it_arr_map_in->second.end() || it_cs_map_inout == it_arr_map_inout->second.end());
    while (it_cs_map_in != it_arr_map_in->second.end()) {
      to_add.push_back(std::pair<ComputeSystem *, std::vector<Conditions *> >(
          it_cs_map_in->first, std::vector<Conditions *>()
      ));
      for (it_cond_in = it_cs_map_in->second.begin(); it_cond_in != it_cs_map_in->second.end(); it_cond_in++)
        to_add.back().second.push_back((*it_cond_in)->copy());
      it_cs_map_in++;
    }
    while (it_cs_map_inout != it_arr_map_inout->second.end()) {
      // nothing todo
      it_cs_map_inout++;
    }

    std::vector<std::pair<ComputeSystem *, std::vector<Conditions *> > >::iterator it_to_add;
    for (it_to_add = to_add.begin(); it_to_add != to_add.end(); it_to_add++) {
      it_arr_map_inout->second.insert(*it_to_add);
    }
  }
  simplify(map_inout);
}

bool NodePlacement::diff(
  const std::map<ArrayPartition *, std::map<ComputeSystem *, std::vector<Conditions *> > > & map1,
  const std::map<ArrayPartition *, std::map<ComputeSystem *, std::vector<Conditions *> > > & map2
) const {
  std::map<ArrayPartition *, std::map<ComputeSystem *, std::vector<Conditions *> > >::const_iterator it_arr_map_1;
  std::map<ArrayPartition *, std::map<ComputeSystem *, std::vector<Conditions *> > >::const_iterator it_arr_map_2;
  std::map<ComputeSystem *, std::vector<Conditions *> >::const_iterator it_cs_map_1;
  std::map<ComputeSystem *, std::vector<Conditions *> >::const_iterator it_cs_map_2;
  std::vector<Conditions *>::const_iterator it_cond_1;
  std::vector<Conditions *>::const_iterator it_cond_2;

  assert(map1.size() == map2.size());

  for (it_arr_map_1 = map1.begin(); it_arr_map_1 != map1.end(); it_arr_map_1++) {
    it_arr_map_2 = map2.find(it_arr_map_1->first);
    assert(it_arr_map_2 != map2.end());

    if (it_arr_map_1->second.size() != it_arr_map_2->second.size()) return true;

    it_cs_map_1 = it_arr_map_1->second.begin();
    it_cs_map_2 = it_arr_map_2->second.begin();
    while (it_cs_map_1 != it_arr_map_1->second.end() && it_cs_map_2 != it_arr_map_2->second.end()) {
      if (it_cs_map_1->first != it_cs_map_2->first) return true;

      if (!areEqual(it_cs_map_1->second, it_cs_map_2->second)) return true;

      it_cs_map_1++; it_cs_map_2++;
    }
    if (it_cs_map_1 != it_arr_map_1->second.end() || it_cs_map_2 != it_arr_map_2->second.end()) return true;
  }

  return false;
}

void NodePlacement::deepCopy(
  std::map<ArrayPartition *, std::map<ComputeSystem *, std::vector<Conditions *> > > & map_out,
  const std::map<ArrayPartition *, std::map<ComputeSystem *, std::vector<Conditions *> > > & map_in
) const {
  assert(map_out.size() == 0);

  std::map<ArrayPartition *, std::map<ComputeSystem *, std::vector<Conditions *> > >::iterator it_arr_map_out;
  std::map<ArrayPartition *, std::map<ComputeSystem *, std::vector<Conditions *> > >::const_iterator it_arr_map_in;
  std::map<ComputeSystem *, std::vector<Conditions *> >::iterator it_cs_map_out;
  std::map<ComputeSystem *, std::vector<Conditions *> >::const_iterator it_cs_map_in;
  std::vector<Conditions *>::iterator it_cond_out;
  std::vector<Conditions *>::const_iterator it_cond_in;

  for (it_arr_map_in = map_in.begin(); it_arr_map_in != map_in.end(); it_arr_map_in++) {
    it_arr_map_out = map_out.insert(std::pair<ArrayPartition *, std::map<ComputeSystem *, std::vector<Conditions *> > >(
        it_arr_map_in->first, std::map<ComputeSystem *, std::vector<Conditions *> >()
    )).first;
    for (it_cs_map_in = it_arr_map_in->second.begin(); it_cs_map_in != it_arr_map_in->second.end(); it_cs_map_in++) {
      it_cs_map_out = it_arr_map_out->second.insert(std::pair<ComputeSystem *, std::vector<Conditions *> >(
          it_cs_map_in->first, std::vector<Conditions *>()
      )).first;
      for (it_cond_in = it_cs_map_in->second.begin(); it_cond_in != it_cs_map_in->second.end(); it_cond_in++)
        it_cs_map_out->second.push_back((*it_cond_in)->copy());
    }
  }
}

void NodePlacement::flush(
  SPMD_Tree * tree,
  std::map<SPMD_Tree *, std::map<ArrayPartition *, std::map<ComputeSystem *, std::vector<Conditions *> > > > & valid_datas
) const {
  std::vector<SPMD_Tree *> & children = tree->getChildren();
    std::vector<SPMD_Tree *>::iterator it_child;
    for (it_child = children.begin(); it_child != children.end(); it_child++)
      flush(*it_child, valid_datas);

  valid_datas.erase(tree);
}

void NodePlacement::restrictToFirstIt(std::vector<Conditions *> conds, SPMD_Loop * loop) const {
  std::vector<Conditions *>::iterator it_cond;
  for (it_cond = conds.begin(); it_cond != conds.end(); it_cond++) {
    Conditions * cond = (*it_cond)->new_restricted_by(loop, true);
    *it_cond = cond;
  }
  simplify(conds);
}

void NodePlacement::restrictToNext(std::vector<Conditions *> conds, SPMD_Loop * loop) const {
  std::vector<Conditions *>::iterator it_cond;
  for (it_cond = conds.begin(); it_cond != conds.end(); it_cond++) {
    Conditions * cond = (*it_cond)->new_without_first_it(loop);
    *it_cond = cond;
  }
  simplify(conds);
}

void NodePlacement::setFirstIt(
  std::map<ArrayPartition *, std::map<ComputeSystem *, std::vector<Conditions *> > > & map,
  SPMD_Loop * loop
) const {
  std::map<ArrayPartition *, std::map<ComputeSystem *, std::vector<Conditions *> > >::iterator it_arr_map;
  std::map<ComputeSystem *, std::vector<Conditions *> >::iterator it_cs_map;
  std::vector<Conditions *>::iterator it_cond;

  for (it_arr_map = map.begin(); it_arr_map != map.end(); it_arr_map++) {
    for (it_cs_map = it_arr_map->second.begin(); it_cs_map != it_arr_map->second.end(); it_cs_map++) {
      for (it_cond = it_cs_map->second.begin(); it_cond != it_cs_map->second.end(); it_cond++) {
        Conditions * cond = (*it_cond)->new_restricted_by(loop, true);
        //delete *it_cond;
        *it_cond = cond;
      }
      simplify(it_cs_map->second);
    }
  }
}

void NodePlacement::setLastIt(
  std::map<ArrayPartition *, std::map<ComputeSystem *, std::vector<Conditions *> > > & map,
  SPMD_Loop * loop
) const {
  std::map<ArrayPartition *, std::map<ComputeSystem *, std::vector<Conditions *> > >::iterator it_arr_map;
  std::map<ComputeSystem *, std::vector<Conditions *> >::iterator it_cs_map;
  std::vector<Conditions *>::iterator it_cond;

  for (it_arr_map = map.begin(); it_arr_map != map.end(); it_arr_map++) {
    for (it_cs_map = it_arr_map->second.begin(); it_cs_map != it_arr_map->second.end(); it_cs_map++) {
      for (it_cond = it_cs_map->second.begin(); it_cond != it_cs_map->second.end(); it_cond++) {
        Conditions * cond = (*it_cond)->new_restricted_by(loop, false);
        //delete *it_cond;
        *it_cond = cond;
      }
      simplify(it_cs_map->second);
    }
  }
}

void NodePlacement::setNextIt(
  std::map<ArrayPartition *, std::map<ComputeSystem *, std::vector<Conditions *> > > & map,
  SPMD_Loop * loop
) const {
  std::map<ArrayPartition *, std::map<ComputeSystem *, std::vector<Conditions *> > >::iterator it_arr_map;
  std::map<ComputeSystem *, std::vector<Conditions *> >::iterator it_cs_map;
  std::vector<Conditions *>::iterator it_cond;

  for (it_arr_map = map.begin(); it_arr_map != map.end(); it_arr_map++) {
    for (it_cs_map = it_arr_map->second.begin(); it_cs_map != it_arr_map->second.end(); it_cs_map++) {
      for (it_cond = it_cs_map->second.begin(); it_cond != it_cs_map->second.end(); it_cond++) {
        Conditions * cond = (*it_cond)->new_without_first_it(loop);
        //delete *it_cond;
        *it_cond = cond;
      }
      simplify(it_cs_map->second);
    }
  }
}

void NodePlacement::addIterator(
  std::map<ArrayPartition *, std::map<ComputeSystem *, std::vector<Conditions *> > > & map,
  SPMD_Loop * loop
) const {
  std::map<ArrayPartition *, std::map<ComputeSystem *, std::vector<Conditions *> > >::iterator it_arr_map;
  std::map<ComputeSystem *, std::vector<Conditions *> >::iterator it_cs_map;
  std::vector<Conditions *>::iterator it_cond;

  for (it_arr_map = map.begin(); it_arr_map != map.end(); it_arr_map++) {
    for (it_cs_map = it_arr_map->second.begin(); it_cs_map != it_arr_map->second.end(); it_cs_map++) {
      for (it_cond = it_cs_map->second.begin(); it_cond != it_cs_map->second.end(); it_cond++) {
        Conditions * cond = (*it_cond)->new_extended_by(loop);
        //delete *it_cond;
        *it_cond = cond;
      }
      simplify(it_cs_map->second);
    }
  }
}

void NodePlacement::addIterator(
  std::map<ArrayPartition *, std::map<ComputeSystem *, std::vector<Conditions *> > > & map,
  SPMD_KernelCall * kernel_call
) const {
  std::map<ArrayPartition *, std::map<ComputeSystem *, std::vector<Conditions *> > >::iterator it_arr_map;
  std::map<ComputeSystem *, std::vector<Conditions *> >::iterator it_cs_map;
  std::vector<Conditions *>::iterator it_cond;

  for (it_arr_map = map.begin(); it_arr_map != map.end(); it_arr_map++) {
    for (it_cs_map = it_arr_map->second.begin(); it_cs_map != it_arr_map->second.end(); it_cs_map++) {
      for (it_cond = it_cs_map->second.begin(); it_cond != it_cs_map->second.end(); it_cond++) {
        Conditions * cond = (*it_cond)->new_extended_by(kernel_call);
        //delete *it_cond;
        *it_cond = cond;
      }
      simplify(it_cs_map->second);
    }
  }
}

void NodePlacement::restrictIterator(
  std::map<ArrayPartition *, std::map<ComputeSystem *, std::vector<Conditions *> > > & map,
  SPMD_DomainRestriction * domain_restriction
) const {
  std::map<ArrayPartition *, std::map<ComputeSystem *, std::vector<Conditions *> > >::iterator it_arr_map;
  std::map<ComputeSystem *, std::vector<Conditions *> >::iterator it_cs_map;
  std::vector<Conditions *>::iterator it_cond;

  for (it_arr_map = map.begin(); it_arr_map != map.end(); it_arr_map++) {
    for (it_cs_map = it_arr_map->second.begin(); it_cs_map != it_arr_map->second.end(); it_cs_map++) {
      for (it_cond = it_cs_map->second.begin(); it_cond != it_cs_map->second.end(); it_cond++) {
        Conditions * cond = (*it_cond)->new_restricted_by(domain_restriction);
        //delete *it_cond;
        *it_cond = cond;
      }
      simplify(it_cs_map->second);
    }
  }
}

void NodePlacement::restrictIterator(
  std::map<ArrayPartition *, std::map<ComputeSystem *, std::vector<Conditions *> > > & map,
  SPMD_KernelCall * kernel_call
) const {
  std::map<ArrayPartition *, std::map<ComputeSystem *, std::vector<Conditions *> > >::iterator it_arr_map;
  std::map<ComputeSystem *, std::vector<Conditions *> >::iterator it_cs_map;
  std::vector<Conditions *>::iterator it_cond;

  for (it_arr_map = map.begin(); it_arr_map != map.end(); it_arr_map++) {
    for (it_cs_map = it_arr_map->second.begin(); it_cs_map != it_arr_map->second.end(); it_cs_map++) {
      for (it_cond = it_cs_map->second.begin(); it_cond != it_cs_map->second.end(); it_cond++) {
        Conditions * cond = (*it_cond)->new_restricted_by(kernel_call);
        //delete *it_cond;
        *it_cond = cond;
      }
      simplify(it_cs_map->second);
    }
  }
}

// Compute for each Node where the valid data are situated when the computation left the node
void NodePlacement::positionValidData(
  SPMD_Tree * tree,
  ArrayAnalysis & array_analysis,
  std::map<SPMD_Tree *, std::map<ArrayPartition *, std::map<ComputeSystem *, std::vector<Conditions *> > > > & valid_datas,
  std::map<ArrayPartition *, std::map<ComputeSystem *, std::vector<Conditions *> > > previous_position,
  Conditions * condition_to_reach_this_node
) const {
  std::map<SPMD_Tree *, std::map<ArrayPartition *, std::map<ComputeSystem *, std::vector<Conditions *> > > >::iterator it_tree_map;

// Current node setup
  it_tree_map = valid_datas.find(tree);
  if (it_tree_map == valid_datas.end()) {
    it_tree_map = valid_datas.insert(std::pair<SPMD_Tree *, std::map<ArrayPartition *, std::map<ComputeSystem *, std::vector<Conditions *> > > >(
        tree, std::map<ArrayPartition *, std::map<ComputeSystem *, std::vector<Conditions *> > >()
    )).first;
  }

  std::map<ArrayPartition *, std::map<ComputeSystem *, std::vector<Conditions *> > > & current_valid_data_map = it_tree_map->second;

// Cast the tree
  SPMD_Root * root = dynamic_cast<SPMD_Root *>(tree);
  SPMD_Loop * loop = dynamic_cast<SPMD_Loop *>(tree);
  SPMD_DomainRestriction * domain_restriction = dynamic_cast<SPMD_DomainRestriction *>(tree);
  SPMD_NativeStmt * native_stmt = dynamic_cast<SPMD_NativeStmt *>(tree);
  SPMD_KernelCall * kernel_call = dynamic_cast<SPMD_KernelCall *>(tree);

// Placement
  ComputeSystem * placement = assigned(tree);

#if DEBUG
  std::cerr << "Node ";
  tree->print(std::cerr);
  std::cerr << " (" << tree << "):" << std::endl;
  std::cerr << "\tis assigned to " << placement->getID() << std::endl;
  std::cerr << "\tis reached if: ";
  condition_to_reach_this_node->print(std::cerr);
  std::cerr << std::endl;
#endif

// Data in/out (double vars for simplicity...)
  const std::set<ArrayPartition *> & data_in  = array_analysis.get(tree).first;
  const std::set<ArrayPartition *> & data_out = array_analysis.get(tree).second;
  std::set<ArrayPartition *>::const_iterator it_data_out;
  std::set<ArrayPartition *>::const_iterator it_data_in;

#if DEBUG
  std::cerr << "\tIt reads: ";
  for (it_data_in = data_in.begin(); it_data_in != data_in.end(); it_data_in++)
    std::cerr << " " << (*it_data_in)->getUniqueName();
  std::cerr << std::endl << "\tIt writes:";
  for (it_data_out = data_out.begin(); it_data_out != data_out.end(); it_data_out++)
    std::cerr << " " << (*it_data_out)->getUniqueName();
  std::cerr << std::endl;
#endif

// iterators
  std::map<ArrayPartition *, std::map<ComputeSystem *, std::vector<Conditions *> > >::iterator it_arr_map;
  std::map<ArrayPartition *, std::map<ComputeSystem *, std::vector<Conditions *> > >::iterator it_arr_map_local;
  std::map<ComputeSystem *, std::vector<Conditions *> >::iterator it_cs_map;
  std::map<ComputeSystem *, std::vector<Conditions *> >::iterator it_cs_map_local;
  std::vector<Conditions *>::iterator it_cond;

#if DEBUG
  for (it_arr_map = previous_position.begin(); it_arr_map != previous_position.end(); it_arr_map++) {
      std::cerr << "\t" << it_arr_map->first->getUniqueName() << " is on:" << std::endl;
      for (it_cs_map = it_arr_map->second.begin(); it_cs_map != it_arr_map->second.end(); it_cs_map++) {
        std::cerr << "\t\t" << it_cs_map->first->getID() << " if:" << std::endl;
        for (it_cond = it_cs_map->second.begin(); it_cond != it_cs_map->second.end(); it_cond++) {
          if (*it_cond != NULL) {
            std::cerr << "\t\t\t";
            (*it_cond)->print(std::cerr);
            std::cerr << std::endl;
          }
          else {
            std::cerr << "\t\t\tTRUE" << std::endl;
          }
        }
      }
    }
#endif

// Root tree
  if (root != NULL) {
    std::vector<SPMD_Tree *> & children = tree->getChildren();
    std::vector<SPMD_Tree *>::iterator it_child;
    for (it_child = children.begin(); it_child != children.end(); it_child++) {
      positionValidData(*it_child, array_analysis, valid_datas, previous_position, condition_to_reach_this_node);
      it_tree_map = valid_datas.find(*it_child);
      assert(it_tree_map != valid_datas.end());
      previous_position = it_tree_map->second;
    }

    deepCopy(current_valid_data_map, previous_position);
  }

// Loop tree
  if (loop != NULL) {
    std::vector<SPMD_Tree *> & children = tree->getChildren();
    std::vector<SPMD_Tree *>::iterator it_child;
    condition_to_reach_this_node = condition_to_reach_this_node->new_extended_by(loop);
    addIterator(previous_position, loop);
    setFirstIt(previous_position, loop);
    for (it_child = children.begin(); it_child != children.end(); it_child++) {
      positionValidData(*it_child, array_analysis, valid_datas, previous_position, condition_to_reach_this_node->new_restricted_by(loop, true));
      it_tree_map = valid_datas.find(*it_child);
      assert(it_tree_map != valid_datas.end());
      previous_position = it_tree_map->second;
    }

    std::map<ArrayPartition *, std::map<ComputeSystem *, std::vector<Conditions *> > > copy;
    deepCopy(copy, previous_position);

    removeIterator(previous_position, loop);
    addIterator(previous_position, loop);
    setNextIt(previous_position, loop);

    for (it_child = children.begin(); it_child != children.end(); it_child++) {
//    flush(*it_child, valid_datas);
      positionValidData(*it_child, array_analysis, valid_datas, previous_position, condition_to_reach_this_node->new_without_first_it(loop));
      it_tree_map = valid_datas.find(*it_child);
      assert(it_tree_map != valid_datas.end());
      previous_position = it_tree_map->second;
    }

    merge(previous_position, copy);

    // If this node have not write an array then its current positions are extended
    for (it_arr_map = previous_position.begin(); it_arr_map != previous_position.end(); it_arr_map++) {
      it_data_out = data_out.find(it_arr_map->first);
      if (it_data_out == data_out.end()) { // have not been write
        for (it_cs_map = it_arr_map->second.begin(); it_cs_map != it_arr_map->second.end(); it_cs_map++) {
          if (it_cs_map->second.size() > 0)
            it_cs_map->second.push_back(condition_to_reach_this_node->copy());
        }
      }
    }

    setLastIt(previous_position, loop);

    removeIterator(previous_position, loop);

    if (current_valid_data_map.size() == 0)
      deepCopy(current_valid_data_map, previous_position);
    else
      merge(current_valid_data_map, previous_position);
  }

// DomainRestriction tree (almost the sam than Root, just restrict 'condition_to_reach_this_node')
  if (domain_restriction != NULL) {
    condition_to_reach_this_node = condition_to_reach_this_node->new_restricted_by(domain_restriction);

    // Copy restricted to not enter this node
    std::map<ArrayPartition *, std::map<ComputeSystem *, std::vector<Conditions *> > > copy;
    deepCopy(copy, previous_position);
    for (it_arr_map = copy.begin(); it_arr_map != copy.end(); it_arr_map++) {
      for (it_cs_map = it_arr_map->second.begin(); it_cs_map != it_arr_map->second.end(); it_cs_map++) {
        removeFromCondSet(it_cs_map->second, condition_to_reach_this_node);
        simplify(it_cs_map->second);
      }
    }

#if DEBUG
  std::cerr << "Copy of previous_position" << std::endl;
  for (it_arr_map = copy.begin(); it_arr_map != copy.end(); it_arr_map++) {
      std::cerr << "\t" << it_arr_map->first->getUniqueName() << " is on:" << std::endl;
      for (it_cs_map = it_arr_map->second.begin(); it_cs_map != it_arr_map->second.end(); it_cs_map++) {
        std::cerr << "\t\t" << it_cs_map->first->getID() << " if:" << std::endl;
        for (it_cond = it_cs_map->second.begin(); it_cond != it_cs_map->second.end(); it_cond++) {
          if (*it_cond != NULL) {
            std::cerr << "\t\t\t";
            (*it_cond)->print(std::cerr);
            std::cerr << std::endl;
          }
          else {
            std::cerr << "\t\t\tTRUE" << std::endl;
          }
        }
      }
    }
#endif


    // go in this node
    restrictIterator(previous_position, domain_restriction);

    std::vector<SPMD_Tree *> & children = tree->getChildren();
    std::vector<SPMD_Tree *>::iterator it_child;
    for (it_child = children.begin(); it_child != children.end(); it_child++) {
      positionValidData(*it_child, array_analysis, valid_datas, previous_position, condition_to_reach_this_node);
      it_tree_map = valid_datas.find(*it_child);
      assert(it_tree_map != valid_datas.end());
      previous_position = it_tree_map->second;
    }

    // If this node have not write an array then its current positions are extended
    for (it_arr_map = previous_position.begin(); it_arr_map != previous_position.end(); it_arr_map++) {
      it_data_out = data_out.find(it_arr_map->first);
      if (it_data_out == data_out.end()) { // have not been write
        for (it_cs_map = it_arr_map->second.begin(); it_cs_map != it_arr_map->second.end(); it_cs_map++) {
          if (it_cs_map->second.size() > 0)
            it_cs_map->second.push_back(condition_to_reach_this_node->copy());
        }
      }
    }

    merge(previous_position, copy);

    if (current_valid_data_map.size() == 0)
      deepCopy(current_valid_data_map, previous_position);
    else
      merge(current_valid_data_map, previous_position);
  }

// KernelCall tree (same as Root)
  if (kernel_call != NULL) {
    condition_to_reach_this_node = condition_to_reach_this_node->new_extended_by(kernel_call);
    condition_to_reach_this_node = condition_to_reach_this_node->new_restricted_by(kernel_call);

    addIterator(previous_position, kernel_call);

    // Copy restricted to not enter this node
    std::map<ArrayPartition *, std::map<ComputeSystem *, std::vector<Conditions *> > > copy;
    deepCopy(copy, previous_position);
    for (it_arr_map = copy.begin(); it_arr_map != copy.end(); it_arr_map++) {
      for (it_cs_map = it_arr_map->second.begin(); it_cs_map != it_arr_map->second.end(); it_cs_map++) {
        removeFromCondSet(it_cs_map->second, condition_to_reach_this_node);
        simplify(it_cs_map->second);
      }
    }

    restrictIterator(previous_position, kernel_call);
    std::vector<SPMD_Tree *> & children = tree->getChildren();
    std::vector<SPMD_Tree *>::iterator it_child;
    for (it_child = children.begin(); it_child != children.end(); it_child++) {
      positionValidData(*it_child, array_analysis, valid_datas, previous_position, condition_to_reach_this_node);
      it_tree_map = valid_datas.find(*it_child);
      assert(it_tree_map != valid_datas.end());
      previous_position = it_tree_map->second;
    }

    // If this node have not write an array then its current positions are extended
    for (it_arr_map = previous_position.begin(); it_arr_map != previous_position.end(); it_arr_map++) {
      it_data_out = data_out.find(it_arr_map->first);
      if (it_data_out == data_out.end()) { // have not been write
        for (it_cs_map = it_arr_map->second.begin(); it_cs_map != it_arr_map->second.end(); it_cs_map++) {
          if (it_cs_map->second.size() > 0)
            it_cs_map->second.push_back(condition_to_reach_this_node->copy());
        }
      }
    }

    merge(previous_position, copy);

    removeIterator(previous_position, kernel_call);

    if (current_valid_data_map.size() == 0)
      deepCopy(current_valid_data_map, previous_position);
    else
      merge(current_valid_data_map, previous_position);
  }

// NativeStmt tree
  if (native_stmt != NULL) {
    std::map<ArrayPartition *, std::map<ComputeSystem *, std::vector<Conditions *> > > copy;
    deepCopy(copy, current_valid_data_map);

    current_valid_data_map.clear();

    // update 'current_valid_data_map'
    for (it_arr_map = previous_position.begin(); it_arr_map != previous_position.end(); it_arr_map++) {
      it_arr_map_local = current_valid_data_map.insert(std::pair<ArrayPartition *, std::map<ComputeSystem *, std::vector<Conditions *> > >(
          it_arr_map->first, std::map<ComputeSystem *, std::vector<Conditions *> >()
      )).first;
      it_data_in  =  data_in.find(it_arr_map->first);
      it_data_out = data_out.find(it_arr_map->first);
      if (it_data_out != data_out.end()) {
        // become only possessor of the array (under 'condition_to_reach_this_node')
        for (it_cs_map = it_arr_map->second.begin(); it_cs_map != it_arr_map->second.end(); it_cs_map++) {
          it_cs_map_local = it_arr_map_local->second.insert(std::pair<ComputeSystem *, std::vector<Conditions *> >(
              it_cs_map->first, std::vector<Conditions *>()
          )).first;
          if (it_cs_map->first == placement) {
            it_cs_map_local->second.push_back(condition_to_reach_this_node->copy());
            for (it_cond = it_cs_map->second.begin(); it_cond != it_cs_map->second.end(); it_cond++) {
              it_cs_map_local->second.push_back((*it_cond)->copy());
            }
          }
          else {
            for (it_cond = it_cs_map->second.begin(); it_cond != it_cs_map->second.end(); it_cond++) {
              it_cs_map_local->second.push_back((*it_cond)->copy());
            }
            removeFromCondSet(it_cs_map_local->second, condition_to_reach_this_node);
          }
        }
        it_cs_map_local = it_arr_map_local->second.find(placement);
        if (it_cs_map_local == it_arr_map_local->second.end()) {
          it_cs_map_local = it_arr_map_local->second.insert(std::pair<ComputeSystem *, std::vector<Conditions *> >(
              placement, std::vector<Conditions *>()
          )).first;
          it_cs_map_local->second.push_back(condition_to_reach_this_node->copy());
        }
      }
      else if (it_data_in != data_in.end()) {
        // become one possessor of the array (under 'condition_to_reach_this_node')
        for (it_cs_map = it_arr_map->second.begin(); it_cs_map != it_arr_map->second.end(); it_cs_map++) {
          it_cs_map_local = it_arr_map_local->second.insert(std::pair<ComputeSystem *, std::vector<Conditions *> >(
              it_cs_map->first, std::vector<Conditions *>()
          )).first;
          if (it_cs_map->first == placement) {
            it_cs_map_local->second.push_back(condition_to_reach_this_node->copy());
          }
          for (it_cond = it_cs_map->second.begin(); it_cond != it_cs_map->second.end(); it_cond++) {
            it_cs_map_local->second.push_back((*it_cond)->copy());
          }
        }
        it_cs_map_local = it_arr_map_local->second.find(placement);
        if (it_cs_map_local == it_arr_map_local->second.end()) {
          it_cs_map_local = it_arr_map_local->second.insert(std::pair<ComputeSystem *, std::vector<Conditions *> >(
              placement, std::vector<Conditions *>()
          )).first;
          it_cs_map_local->second.push_back(condition_to_reach_this_node->copy());
        }
      }
      else {
        // copy 'it_arr_map' this statement don't touch this array
        for (it_cs_map = it_arr_map->second.begin(); it_cs_map != it_arr_map->second.end(); it_cs_map++) {
          it_cs_map_local = it_arr_map_local->second.insert(std::pair<ComputeSystem *, std::vector<Conditions *> >(
              it_cs_map->first, std::vector<Conditions *>()
          )).first;
          for (it_cond = it_cs_map->second.begin(); it_cond != it_cs_map->second.end(); it_cond++) {
            it_cs_map_local->second.push_back((*it_cond)->copy());
          }
        }
      }
    }

    if (copy.size() != 0)
      merge(current_valid_data_map, copy);
  }

  simplify(current_valid_data_map);

#if 0
  for (it_arr_map = current_valid_data_map.begin(); it_arr_map != current_valid_data_map.end(); it_arr_map++) {
      std::cerr << "\t" << it_arr_map->first->getUniqueName() << " would be on:" << std::endl;
      for (it_cs_map = it_arr_map->second.begin(); it_cs_map != it_arr_map->second.end(); it_cs_map++) {
        std::cerr << "\t\t" << it_cs_map->first->getID() << " if:" << std::endl;
        for (it_cond = it_cs_map->second.begin(); it_cond != it_cs_map->second.end(); it_cond++) {
          if (*it_cond != NULL) {
            std::cerr << "\t\t\t";
            (*it_cond)->print(std::cerr);
            std::cerr << std::endl;
          }
          else {
            std::cerr << "\t\t\tTRUE" << std::endl;
          }
        }
      }
    }
#endif

}

