/* Tests unparsing by changing the byte order of an ELF file.
 *
 * ELF containers can be big-endian or little-endian. This test changes the ELF container from big to little or vice versa. The
 * program contained in the container (instructions, read-only data, initialized rw data, etc) are not byte swapped--only the
 * ELF container is byte swapped.
 *
 * Running the program produces a *.new file in the current working directory (produced by backend()). The *.new file is
 * identical to the input file except the ELF container is byte swapped.  The makefile test reads and parses the *.new file and
 * compares it to the original file. */

#include "rose.h"

class MyTraversal : public SgSimpleProcessing {
  public:
    void visit(SgNode *astNode);
};

void
MyTraversal::visit(SgNode* astNode)
{
    SgAsmElfFileHeader *fhdr = isSgAsmElfFileHeader(astNode);
    if (fhdr) {
        SgAsmExecutableFileFormat::ByteOrder oldsex = fhdr->get_sex();
        SgAsmExecutableFileFormat::ByteOrder newsex = 
            oldsex==SgAsmExecutableFileFormat::ORDER_LSB ?
            SgAsmExecutableFileFormat::ORDER_MSB :
            SgAsmExecutableFileFormat::ORDER_LSB;

        printf("*** Changing ELF header byte order from %s to %s\n",
               oldsex==SgAsmExecutableFileFormat::ORDER_LSB?"little endian":"big endian", 
               newsex==SgAsmExecutableFileFormat::ORDER_LSB?"little endian":"big endian");
        
        fhdr->get_exec_format()->set_sex(newsex);
    }
}

int
main(int argc, char *argv[])
{
    SgProject *project= frontend(argc,argv);
    MyTraversal myTraversal;
    myTraversal.traverseInputFiles(project, postorder);
    return backend(project);
}
