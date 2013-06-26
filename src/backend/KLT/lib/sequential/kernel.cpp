
#include "KLT/Sequential/kernel.hpp"

#include <cassert>

namespace KLT {

namespace Sequential {

void Kernel::setKernelSymbol(SgFunctionSymbol * kernel_symbol) {
  assert(p_kernel_symbol ==NULL);
  p_kernel_symbol = kernel_symbol;
}

void Kernel::setArgumentPacker(SgClassSymbol * arguments_packer) {
  assert(p_arguments_packer == NULL);
  p_arguments_packer = arguments_packer;
}

Kernel::Kernel() :
  Core::Kernel(),
  p_kernel_symbol(NULL),
  p_arguments_packer(NULL)
{}

Kernel::~Kernel() {}

SgFunctionSymbol * Kernel::getKernelSymbol() const { return p_kernel_symbol; }

SgClassSymbol * Kernel::getArgumentsPacker() const { return p_arguments_packer; }


}

}

