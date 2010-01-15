// tps (01/14/2010) : Switching from rose.h to sage3.
#include "sage3basic.h"
#include "BinaryLoader.h"

#include "BinaryLoaderGeneric.h" // TODO remove
#include "BinaryLoaderElf.h" // TODO remove

/** Loads, performs layout, relocates and disassembles the given file */
bool BinaryLoader::load(SgBinaryComposite* binaryFile,bool performDynamic)
{
  BinaryLoaderElf tmp;
  tmp.set_perform_dynamic_linking(performDynamic);
  tmp.set_perform_layout(performDynamic);
  tmp.set_perform_relocations(performDynamic);
  tmp.set_perform_disassembly(performDynamic);
  if(false == binaryFile->get_read_executable_file_format_only()){
    tmp.set_perform_layout(true);
    tmp.set_perform_disassembly(true);// support legacy switches
  }
  tmp.load(binaryFile,NULL);
  return false;
}
