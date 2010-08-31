#include "sage3basic.h"
#include "binaryLoader.h"

#include "BinaryLoaderGeneric.h" // TODO remove
#include "BinaryLoaderElf.h" // TODO remove

bool
BinaryLoader::load(SgBinaryComposite* binaryFile, bool performDynamic/*=false*/)
{
    /*FIXME: This generic interface assumes the binaryFile is ELF*/
    BinaryLoaderElf tmp;
    tmp.set_perform_dynamic_linking(performDynamic);
    tmp.set_perform_layout(performDynamic);
    tmp.set_perform_relocations(performDynamic);
    tmp.set_perform_disassembly(performDynamic);
    if (!binaryFile->get_read_executable_file_format_only()) {
        tmp.set_perform_layout(true);
        tmp.set_perform_disassembly(true);// support legacy switches
    }
    tmp.load(binaryFile,NULL);
    return false;
}
