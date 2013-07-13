
#include "KLT/Core/kernel.hpp"

#include <cassert>

namespace KLT {

namespace Core {

unsigned long Kernel::id_cnt = 0;

Kernel::Kernel() :
  id(id_cnt++),
  p_dataflow_done(false),
  p_data_in(),
  p_data_out(),
  p_arguments_done(false),
  p_parameters_argument_order(),
  p_coefficients_argument_order(),
  p_datas_argument_order(),
  p_loop_distribution_done(false),
  p_loop_distributions()
{}

void Kernel::setDataflow(const std::set<Data *> & data_in, const std::set<Data *> & data_out) {
  assert(!p_dataflow_done);

  p_data_in = data_in;
  p_data_out = data_out;

  p_dataflow_done = true;
}

void Kernel::setArgument(
  const std::list<SgVariableSymbol *> & parameters_argument_order,
  const std::list<SgVariableSymbol *> & coefficients_argument_order,
  const std::list<Data *> & datas_argument_order
) {
  assert(!p_arguments_done);

  p_parameters_argument_order = parameters_argument_order;
  p_coefficients_argument_order = coefficients_argument_order;
  p_datas_argument_order = datas_argument_order;

  p_arguments_done = true;
}

void Kernel::setLoopDistributions(const std::set<loop_distribution_t *> & loop_distributions) {
  p_loop_distributions = loop_distributions;

  p_loop_distribution_done = true;
}

Kernel::~Kernel() {}

bool Kernel::isDataflowDone() const { return p_dataflow_done; }

const std::set<Data *> & Kernel::getFlowingIn() const {
  assert(p_dataflow_done);
  return p_data_in;
}

const std::set<Data *> & Kernel::getFlowingOut() const {
  assert(p_dataflow_done);
  return p_data_out;
}

bool Kernel::isArgumentDone() const { return p_arguments_done; }

const std::list<SgVariableSymbol *> & Kernel::getCoefficientsArguments() const {
  assert(p_arguments_done);
  return p_coefficients_argument_order;
}

const std::list<SgVariableSymbol *> & Kernel::getParametersArguments() const {
  assert(p_arguments_done);
  return p_parameters_argument_order;
}

const std::list<Data *> & Kernel::getDatasArguments() const {
  assert(p_arguments_done);
  return p_datas_argument_order;
}

bool Kernel::isLoopDistributionDone() const { return p_loop_distribution_done; }

const std::set<Kernel::loop_distribution_t *> & Kernel::getLoopDistributions() const {
  assert(p_loop_distribution_done);
  return p_loop_distributions;
}

}

}

