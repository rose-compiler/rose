
#include "KLT/Core/generator.hpp"
#include "KLT/Sequential/kernel.hpp"

#include "KLT/Core/mfb-klt.hpp"

namespace KLT {

namespace Core {

template <>
unsigned long Generator<Sequential::Kernel, ::MultiFileBuilder::KLT_Driver>::createFile() {
  return p_sage_driver.createPairOfFiles(p_file_name);
}

}

}
