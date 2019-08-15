/* Tests unparsing by changing the byte order of an ELF file.
 *
 * ELF containers can be big-endian or little-endian. This test changes the ELF container from big to little or vice versa. The
 * program contained in the container (instructions, read-only data, initialized rw data, etc) are not byte swapped--only the
 * ELF container is byte swapped.
 *
 * Running the program produces a *.new file in the current working directory (produced by backend()). The *.new file is
 * identical to the input file except the ELF container is byte swapped.  The makefile test reads and parses the *.new file and
 * compares it to the original file. */
#include "conditionalDisable.h"
#ifdef ROSE_BINARY_TEST_DISABLED
#include <iostream>
int main() { std::cout <<"disabled for " <<ROSE_BINARY_TEST_DISABLED <<"\n"; return 1; }
#else

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
        ByteOrder::Endianness oldsex = fhdr->get_sex();
        ByteOrder::Endianness newsex = oldsex==ByteOrder::ORDER_LSB ? ByteOrder::ORDER_MSB : ByteOrder::ORDER_LSB;

        printf("*** Changing ELF header byte order from %s to %s\n",
               oldsex==ByteOrder::ORDER_LSB?"little endian":"big endian", 
               newsex==ByteOrder::ORDER_LSB?"little endian":"big endian");
        
        fhdr->get_exec_format()->set_sex(newsex);
    }
}

class UnparseFileName: public AstSimpleProcessing {
    std::string name;

public:
    explicit UnparseFileName(const std::string &name)
        : name(name) {}

    void visit(SgNode *node) {
        if (SgFile *file = isSgFile(node))
            file->set_unparse_output_filename(name);
        if (SgAsmGenericFile *asmfile = isSgAsmGenericFile(node))
            asmfile->set_name(name);
    }
};

void
setUnparseFileName(SgProject *project, const std::string &unparseFileName) {
    UnparseFileName visitor(unparseFileName);
    visitor.traverse(project, preorder);
}

int
main(int argc, char *argv[])
{
    std::string unparseFileName;
    for (int i=1; i<argc; ++i) {
        if (strcmp(argv[i], "-o") == 0 && i+1 < argc) {
            unparseFileName = argv[i+1];
            memcpy(argv+i, argv+i+2, (argc-(i+1))*sizeof(argv[0]));// don't forget to move argv[argc]
            argc -= 2;
            break;
        }
    }

    SgProject *project= frontend(argc,argv);
    if (!unparseFileName.empty()) {
        setUnparseFileName(project, unparseFileName);
    }
    
    MyTraversal myTraversal;
    myTraversal.traverseInputFiles(project, postorder);
    return backend(project);
}

#endif
