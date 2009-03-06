/* Tests unparsing by changing the byte order of an ELF file */

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

