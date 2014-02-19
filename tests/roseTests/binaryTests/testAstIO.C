/* Reads a binary file, writes out the AST, then reads the AST */
#include "rose.h"
#include "stringify.h"

using namespace rose;

#if 0 // call is commented out below
static void
dump_ast(std::ostream &o, SgNode *ast)
{
    struct T1: public AstPrePostProcessing {
        std::ostream &o;
        size_t depth;

        T1(std::ostream &o): o(o), depth(0) {}
        
        void preOrderVisit(SgNode *node) {
            for (size_t i=0; i<depth; ++i) o <<"   |";
            o <<"-- " <<stringifyVariantT(node->variantT(), "V_") <<" at " <<node <<" with parent " <<node->get_parent() <<"\n";
            ++depth;
        }

        void postOrderVisit(SgNode*) {
            assert(depth>0);
            --depth;
        }
    };

    T1(o).traverse(ast);
}
#endif

/* Computes a dump file name for an AST by looking for the first SgAsmGenericFile node */
struct BaseName: public SgSimpleProcessing {
    std::string name;
    BaseName(SgNode *ast) {
        try {
            traverse(ast, preorder);
        } catch (const std::string &s) {
            name = s;
            size_t slash = name.find_last_of('/');
            if (slash!=name.npos)
                name.replace(0, slash+1, "");
        }
    }
    const std::string &string() const { return name; }
    void visit(SgNode *node) {
        if (isSgAsmGenericFile(node))
            throw isSgAsmGenericFile(node)->get_name();
    }
};

/* Dumps an AST to a file. The file name is created from the first SgAsmGenericFile node in the AST. Other than the name of
 * the file, the output is identical to the *.dump file that is normally produced by the backend. */
struct Dumper: public SgSimpleProcessing {
    FILE *dumpFile;
    Dumper(SgNode *ast, const std::string &dump_name) {
        dumpFile = fopen(dump_name.c_str(), "w");
        ROSE_ASSERT(dumpFile!=NULL);
        traverse(ast, preorder);
        fclose(dumpFile);
        dumpFile = NULL;
    }
    void visit(SgNode *node) {
        SgAsmGenericFile *file = isSgAsmGenericFile(node);
        if (file) {
            try {
                fprintf(dumpFile, "%s\n", file->format_name());

                /* A table describing the sections of the file */
                file->dump(dumpFile);

                /* Detailed info about each section */
                const SgAsmGenericSectionPtrList &sections = file->get_sections();
                for (size_t i = 0; i < sections.size(); i++) {
                    fprintf(dumpFile, "Section [%zd]:\n", i);
                    ROSE_ASSERT(sections[i] != NULL);
                    sections[i]->dump(dumpFile, "  ", -1);
                }

                /* Dump interpretations that point only to this file. */
                SgBinaryComposite *binary = SageInterface::getEnclosingNode<SgBinaryComposite>(file);
                ROSE_ASSERT(binary!=NULL);
                const SgAsmInterpretationPtrList &interps = binary->get_interpretations()->get_interpretations();
                for (size_t i=0; i<interps.size(); i++) {
                    SgAsmGenericFilePtrList interp_files = interps[i]->get_files();
                    if (interp_files.size()==1 && interp_files[0]==file) {
                        std::string assembly = unparseAsmInterpretation(interps[i]);
                        fputs(assembly.c_str(), dumpFile);
                    }
                }
            } catch (...) {
                fclose(dumpFile);
                throw;
            }
        }
    }
};

struct TestInterpMap: AstSimpleProcessing {
    void visit(SgNode *node) {
        if (SgAsmInterpretation *interp = isSgAsmInterpretation(node)) {
            std::cout <<"interp map = " <<interp->get_map() <<"\n"
                      <<"interp registers = " <<interp->get_registers() <<"\n";
            if (interp->get_map())
                interp->get_map()->print(std::cout);
        }
    }
};

int
main(int argc, char *argv[])
{
    bool rdonly_test = argc==3 && !strcmp(argv[1], "--rdonly");
    std::string ast_name, dump1_name, dump2_name;

    if (rdonly_test) {
        ast_name = argv[2];
    } else {
        /* Parse the binary and create a text dump file */
        SgProject *p1 = frontend(argc, argv);
        ROSE_ASSERT(p1!=NULL);
        std::string base_name = BaseName(p1).string();
        dump1_name = base_name + "-1.dump";
        Dumper(p1, dump1_name);

        /* Write the AST to a file and then read it back */
        ast_name = base_name + ".ast";
        AST_FILE_IO::startUp(p1);
        AST_FILE_IO::writeASTToFile(ast_name);
        AST_FILE_IO::clearAllMemoryPools();
    }
        
    SgProject *p2 = AST_FILE_IO::readASTFromFile(ast_name);
    ROSE_ASSERT(p2!=NULL);

    /* Some sanity checks */
    TestInterpMap().traverse(p2, preorder);

    /* Create a text dump file for what we read back. The two text files should be identical. */
    dump2_name = BaseName(p2).string() + "-2.dump";
    Dumper(p2, dump2_name);

    if (!rdonly_test) {
        /* Compare the text files */
        execl("/usr/bin/diff", "diff", dump1_name.c_str(), dump2_name.c_str(), NULL);
        return 2; /*exec failed*/
    }
    return 0;
}
