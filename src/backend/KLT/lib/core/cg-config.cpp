
#include "KLT/Core/cg-config.hpp"
#include "KLT/Core/data-flow.hpp"
#include "KLT/Core/loop-selector.hpp"

namespace KLT {

namespace Core {

CG_Config::CG_Config(DataFlow * data_flow, LoopSelector * loop_selector) :
  p_data_flow(data_flow != NULL ? data_flow : new DataFlow()),
  p_loop_selector(loop_selector != NULL ? loop_selector : new LoopSelector())
{}

CG_Config::~CG_Config() {
  if (p_data_flow != NULL) delete p_data_flow;
  if (p_loop_selector != NULL) delete p_loop_selector;
}

const DataFlow & CG_Config::getDataFlow() const { return *p_data_flow; }

const LoopSelector & CG_Config::getLoopSelector() const { return *p_loop_selector; }

}

}

