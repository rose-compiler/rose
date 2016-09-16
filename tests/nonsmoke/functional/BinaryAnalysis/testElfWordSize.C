/* Tests unparsing by changing the ELF word size from 8 to 4 bytes */

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
        size_t oldsize = fhdr->get_word_size();
        if (8==oldsize) {
            size_t newsize = 4;
            printf("*** Changing ELF word size from %zu bits to %zu bits\n", 8*oldsize, 8*newsize);
            fhdr->get_exec_format()->set_word_size(newsize);
        } else if (4==oldsize) {
            size_t newsize = 8;
            printf("*** Changing ELF word size from %zu bits to %zu bits\n", 8*oldsize, 8*newsize);
            fhdr->get_exec_format()->set_word_size(newsize);
        }
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

