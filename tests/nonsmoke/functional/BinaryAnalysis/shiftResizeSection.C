/* Tests string reallocation functions by changing some strings */
#include "conditionalDisable.h"
#ifdef ROSE_BINARY_TEST_DISABLED
#include <iostream>
int main() { std::cout <<"disabled for " <<ROSE_BINARY_TEST_DISABLED <<"\n"; return 1; }
#else

#include "rose.h"
#define __STDC_FORMAT_MACROS
#include <inttypes.h>

class MyTraversal : public SgSimpleProcessing {
  public:
    MyTraversal() 
        : shift_amount(0), extend_amount(0) {}
    void visit(SgNode *astNode);
    std::string secname;
    rose_addr_t shift_amount, extend_amount;
};

void
MyTraversal::visit(SgNode* astNode)
{
    SgAsmGenericSection *section = isSgAsmGenericSection(astNode);
    if (section && section->get_name()->get_string()==secname) {
        SgAsmGenericFile *ef = section->get_file();
        printf("Modifying section [%d] \"%s\"\n", section->get_id(), section->get_name()->get_string(true).c_str());
        printf("   Shifting by  0x%08" PRIx64 " (%" PRIu64 ") bytes.\n", shift_amount, shift_amount);
        printf("   Extending by 0x%08" PRIx64 " (%" PRIu64 ") bytes.\n", extend_amount, extend_amount);
        ef->shift_extend(section, shift_amount, extend_amount);
    }
}

int
main(int argc, char *argv[])
{
    MyTraversal myTraversal;

    /* Extract the shift and extend amounts from the command-line. They appear after the file name. Only one file name is
     * allowed.
     *
     * FIXME: There's surely a better way to process argv before calling frontend() (RPM 2008-09-29) */
    int new_argc = 1;
    char **new_argv = (char**)malloc((argc+1)*sizeof(char*));
    ASSERT_always_not_null(new_argv);
    new_argv[0] = argv[0];
    for (int i=1, nfiles=0; i<argc; i++) {
        new_argv[new_argc++] = argv[i];
        if (argv[i][0]!='-' && access(argv[i], F_OK)>=0) {
            if (nfiles++>0 || i+1>=argc) {
                fprintf(stderr, "usage: %s [SWITCHES] FILE SECTION_NAME [SHIFT_AMOUNT [EXTEND_AMOUNT]]\n", argv[0]);
                exit(1);
            }
            myTraversal.secname = argv[i+1];
            if (i+2<argc && isdigit(argv[i+2][0])) {
                myTraversal.shift_amount = strtol(argv[i+2], 0, 0);
                if (i+3<argc && isdigit(argv[i+3][0])) {
                    myTraversal.extend_amount = strtol(argv[i+3], 0, 0);
                    i += 3; /*removed section name, shift amount, and extend amount*/
                } else {
                    myTraversal.extend_amount = 0;
                    i += 2; /*removed section name and shift amount*/
                }
            } else {
                i += 1; /*removed only section name*/
            }
        }
    }
    new_argv[new_argc] = NULL;

    /* Parse file, change matching section(s), unparse */
    SgProject *project= frontend(new_argc,new_argv);
    myTraversal.traverseInputFiles(project, postorder);
    return backend(project);
}

#endif
