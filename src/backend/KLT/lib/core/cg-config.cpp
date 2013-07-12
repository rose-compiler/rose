
#include "KLT/Core/cg-config.hpp"
#include "KLT/Core/data-flow.hpp"
#include "KLT/Core/loop-selector.hpp"

#include <cassert>

namespace KLT {

namespace Core {

CG_Config::CG_Config(DataFlow * data_flow, LoopSelector * loop_selector) :
  p_data_flow(data_flow),
  p_loop_selector(loop_selector)
{
  assert(p_data_flow != NULL && p_loop_selector != NULL);
}

CG_Config::~CG_Config() {
  if (p_data_flow != NULL) delete p_data_flow;
  if (p_loop_selector != NULL) delete p_loop_selector;
}

const DataFlow & CG_Config::getDataFlow() const { return *p_data_flow; }

const LoopSelector & CG_Config::getLoopSelector() const { return *p_loop_selector; }

}

}

