
#include "KLT/Sequential/kernel.hpp"

#include <cassert>

namespace KLT {

namespace Sequential {

Kernel::Kernel(Core::LoopTrees::node_t * root) :
  Core::Kernel(root),
  p_iteration_maps(),
  p_kernel_map()
{}

Kernel::~Kernel() {}

std::set<Core::IterationMap<Kernel> *> & Kernel::getIterationMaps(loop_mapping_t * loop_mapping) {
  std::map<loop_mapping_t *, std::set<Core::IterationMap<Kernel> *> >::iterator it_iteration_map = p_iteration_maps.find(loop_mapping);
  if (it_iteration_map == p_iteration_maps.end())
    return p_iteration_maps.insert(std::pair<loop_mapping_t *, std::set<Core::IterationMap<Kernel> *> >(loop_mapping, std::set<Core::IterationMap<Kernel> *>())).first->second;
  else return it_iteration_map->second;
}

const std::set<Core::IterationMap<Kernel> *> & Kernel::getIterationMaps(loop_mapping_t * loop_mapping) const {
  std::map<loop_mapping_t *, std::set<Core::IterationMap<Kernel> *> >::const_iterator it_iteration_map = p_iteration_maps.find(loop_mapping);
  assert(it_iteration_map != p_iteration_maps.end());
  return it_iteration_map->second;
}

void Kernel::setKernel(loop_mapping_t * loop_mapping, Core::IterationMap<Kernel> * iteration_map, a_kernel * kernel) {
  std::map<loop_mapping_t *, std::map<Core::IterationMap<Kernel> *, a_kernel *> >::iterator it_iter_to_kernel_map = p_kernel_map.find(loop_mapping);
  if (it_iter_to_kernel_map == p_kernel_map.end())
    p_kernel_map.insert(
      std::pair<loop_mapping_t *, std::map<Core::IterationMap<Kernel> *, a_kernel *> >(
        loop_mapping, std::map<Core::IterationMap<Kernel> *, a_kernel *>()
       )
     ).first->second.insert(
       std::pair<Core::IterationMap<Kernel> *, a_kernel *>(iteration_map, kernel)
     );
  else {
    std::map<Core::IterationMap<Kernel> *, a_kernel *> & iter_to_kernel_map = it_iter_to_kernel_map->second;
    std::map<Core::IterationMap<Kernel> *, a_kernel *>::iterator it_kernel = iter_to_kernel_map.find(iteration_map);
    assert(it_kernel == iter_to_kernel_map.end());
    iter_to_kernel_map.insert(std::pair<Core::IterationMap<Kernel> *, a_kernel *>(iteration_map, kernel));
  }
}

Kernel::a_kernel * Kernel::getKernel(loop_mapping_t * loop_mapping, Core::IterationMap<Kernel> * iteration_map) const {
  std::map<loop_mapping_t *, std::map<Core::IterationMap<Kernel> *, a_kernel *> >::const_iterator it_iter_to_kernel_map = p_kernel_map.find(loop_mapping);
  assert(it_iter_to_kernel_map != p_kernel_map.end());

  const std::map<Core::IterationMap<Kernel> *, a_kernel *> & iter_to_kernel_map = it_iter_to_kernel_map->second;
  std::map<Core::IterationMap<Kernel> *, a_kernel *>::const_iterator it_kernel = iter_to_kernel_map.find(iteration_map);
  assert(it_kernel != iter_to_kernel_map.end());

  return it_kernel->second;
}

}

}

