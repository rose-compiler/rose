#include <rose.h>


#include "BinaryLoaderGeneric.h" // TODO remove
#include "BinaryLoaderElf.h" // TODO remove

/** Loads, performs layout, relocates and disassembles the given file */
bool BinaryLoader::load(SgBinaryComposite* binaryFile,bool performDynamic)
{
  BinaryLoaderElf tmp;
  tmp.set_perform_dynamic_linking(performDynamic);
  tmp.set_perform_relocations(performDynamic);
  tmp.set_perform_disassembly(performDynamic);
  tmp.load(binaryFile,NULL);
  return false;
}
