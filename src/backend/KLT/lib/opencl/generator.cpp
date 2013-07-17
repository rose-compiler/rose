
#include "KLT/Core/generator.hpp"
#include "KLT/OpenCL/kernel.hpp"

#include "KLT/Core/mfb-klt.hpp"

namespace KLT {

namespace Core {

template <>
unsigned long Generator<OpenCL::Kernel, ::MultiFileBuilder::KLT_Driver>::createFile() {
  return p_sage_driver.createStandaloneSourceFile(p_file_name, "cl");
}

}

}

