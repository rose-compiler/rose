#include "rose.h"

/* Show the list of import items */
struct list_import_items: public AstSimpleProcessing {
    SgAsmPEImportDirectory *directory;
    std::ostream &o;

    list_import_items(std::ostream &o, SgNode *ast): directory(NULL), o(o) {
        traverse(ast, preorder);
    }

    void visit(SgNode *node) {
        SgAsmPEImportDirectory *idir = isSgAsmPEImportDirectory(node);
        if (idir) {
            directory = idir;
            o <<"=== " <<idir->get_dll_name()->get_string(true) <<" ===" <<std::endl;
        }

        SgAsmPEImportItem *import = isSgAsmPEImportItem(node);
        if (import) {
            assert(directory!=NULL);
            o <<"  IAT entry #" <<directory->find_import_item(import)
              <<" at " <<StringUtility::addrToString(import->get_iat_entry_va());
            if (0!=import->get_bound_rva().get_rva())
                o <<" bound to " <<import->get_bound_rva().to_string();
            if (import->get_by_ordinal())
                o <<", ordinal=" <<import->get_ordinal();
            if (0!=import->get_hint())
                o <<", hint=" <<import->get_hint();
            if (!import->get_name()->get_string().empty())
                o <<", name=\"" <<import->get_name()->get_string(true) <<"\"";
            o <<std::endl;
        }
    }
};

/* Show the addresses of the implied Hint/Name Table */
struct show_hnt_addresses: public AstSimpleProcessing {
    std::ostream &o;

    show_hnt_addresses(std::ostream &o, SgNode *ast): o(o) {
        traverse(ast, preorder);
    }

    void visit(SgNode *node) {
        SgAsmPEImportDirectory *idir = isSgAsmPEImportDirectory(node);
        if (idir) {
            AddressIntervalSet hnt_extent;
            size_t hnt_nentries = idir->hintname_table_extent(hnt_extent);
            o <<idir->get_dll_name()->get_string(true) <<" hint/name table has " <<StringUtility::plural(hnt_nentries, "entries");
            if (hnt_nentries>0)
                o <<" at virtual addresses: " <<hnt_extent;
            o <<std::endl;
        }
    }
};


int
main(int argc, char *argv[])
{
    SgProject *project = frontend(argc, argv);

    list_import_items(std::cout, project);
    show_hnt_addresses(std::cout, project);

    return 0;
}

                
