// Adds functions to the database.
#include "sage3basic.h"
#include "DwarfLineMapper.h"
#include "BinaryFunctionCall.h"
#include "BinaryLoader.h"
#include "Partitioner.h"
#include "rose.h"

#include <boost/lexical_cast.hpp>


std::string argv0;

SgAsmInterpretation *
open_specimen(const std::string &specimen_name, const std::string &argv0, bool do_link)
{
    // Parse the binary container (ELF, PE, etc) but do not disassemble yet.
    std::cerr <<argv0 <<": parsing binary container: " <<specimen_name <<"\n";
    std::string arg = "-rose:read_executable_file_format_only";
    char *argv[4];
    argv[0] = strdup(argv0.c_str());
    argv[1] = strdup(arg.c_str());
    argv[2] = strdup(specimen_name.c_str());
    argv[3] = NULL;
    int argc = 3;
    SgProject *project = frontend(argc, argv);
    free(argv[0]);
    free(argv[1]);
    free(argv[2]);

    // Find the primary interpretation (e.g., the PE, not DOS, interpretation in PE files).
    std::cerr <<argv0 <<": finding primary interpretation\n";
    std::vector<SgAsmInterpretation*> interps = SageInterface::querySubTree<SgAsmInterpretation>(project);
    if (interps.empty()) {
        std::cerr <<argv0 <<": ERROR: no binary specimen given\n";
        return NULL;
    }
    SgAsmInterpretation *interp = interps.back();
    SgAsmGenericHeader *spec_header = interp->get_headers()->get_headers().back();

    // If the specimen is a shared library then the .text section (and most of the others) are mapped starting at virtual
    // address zero.  This will interfere with tests where the specimen reads from a memory address that was randomly
    // generated--because the randomly generated address will be a low number. When the test tries to read from that low
    // address it will read an instruction rather than a value from an input queue. The way we avoid this is to pre-map the low
    // addresses to force BinaryLoader::remap() to move the specimen to a higher address. [Robb P. Matzke 2013-11-26]
    Extent exclusion_area(0, 0x03000000); // size is arbitrary, but something recognizable
    SgAsmGenericSection *text = spec_header->get_section_by_name(".text");
    bool added_exclusion_area = false;
    if (text!=NULL && text->is_mapped() && text->get_mapped_preferred_extent().overlaps(exclusion_area)) {
        std::cerr <<argv0 <<": specimen is a shared object; remapping it to a higher virtual address\n";
        MemoryMap *map = interp->get_map();
        if (!map)
            interp->set_map(map = new MemoryMap);
        map->insert(exclusion_area,
                    MemoryMap::Segment(MemoryMap::AnonymousBuffer::create(exclusion_area.size()),
                                       0, MemoryMap::MM_PROT_NONE, "temporary exclusion area"));
        added_exclusion_area = true;
    }

    // Get the shared libraries, map them, and apply relocation fixups. We have to do the mapping step even if we're not
    // linking with shared libraries, because that's what gets the various file sections lined up in memory for the
    // disassembler.
    SgAsmGenericHeader *builtin_header = NULL;
    if (do_link)
        std::cerr <<argv0 <<": loading shared libraries\n";
    if (BinaryLoader *loader = BinaryLoader::lookup(interp)) {
        try {
            loader = loader->clone(); // so our settings are private
            if (do_link) {
                // Link with the standard libraries
                loader->add_directory("/lib32");
                loader->add_directory("/usr/lib32");
                loader->add_directory("/lib");
                loader->add_directory("/usr/lib");
                if (char *ld_library_path = getenv("LD_LIBRARY_PATH")) {
                    std::vector<std::string> paths;
                    StringUtility::splitStringIntoStrings(ld_library_path, ':', paths/*out*/);
                    loader->add_directories(paths);
                }
                loader->link(interp);
            } else {
                // If we didn't link with the standard C library, then link with our own library.  Our own library is much
                // smaller and is intended to provide the same semantics as the C library for those few functions that GCC
                // occassionally inlines because the function is built into GCC.  This allows us to compare non-optimized
                // code (without these functions having been inlined) with optimized code (where these functions are inlined)
                // because the unoptimized code will traverse into the definitions we provide.
                //
                // Note: Using BinaryLoader to link our builtins.so does not actually resolve the imports in the specimen.
                // I'm not sure why, but we work around this by calling link_builtins() below. [Robb P. Matzke 2013-07-11]
                std::string subdir = "/projects/BinaryCloneDetection/semantic";
                loader->add_directory(ROSE_AUTOMAKE_TOP_BUILDDIR + subdir);
                loader->add_directory(ROSE_AUTOMAKE_TOP_SRCDIR + subdir);
                loader->add_directory(ROSE_AUTOMAKE_LIBDIR);
                std::string builtin_name = loader->find_so_file("builtins.so");
                SgBinaryComposite *binary = SageInterface::getEnclosingNode<SgBinaryComposite>(interp);
                assert(binary!=NULL);
                SgAsmGenericFile *builtin_file = loader->createAsmAST(binary, builtin_name);
                assert(builtin_file!=NULL);
                builtin_header = builtin_file->get_headers()->get_headers().back();
            }
            loader->remap(interp);
            BinaryLoader::FixupErrors fixup_errors;
            loader->fixup(interp, &fixup_errors);
            if (!fixup_errors.empty()) {
                std::cerr <<argv0 <<": warning: " <<fixup_errors.size()
                          <<" relocation fixup error" <<(1==fixup_errors.size()?"":"s") <<" encountered\n";
            }
            if (SageInterface::querySubTree<SgAsmInterpretation>(project).size() != interps.size())
                std::cerr <<argv0 <<": warning: new interpretations created by the linker; mixed 32- and 64-bit libraries?\n";
        } catch (const BinaryLoader::Exception &e) {
            std::cerr <<argv0 <<": BinaryLoader error: " <<e.mesg <<"\n";
            return NULL;
        }
    } else {
        std::cerr <<argv0 <<": ERROR: no suitable loader/linker found\n";
        return NULL;
    }
    assert(interp->get_map()!=NULL);
    if (added_exclusion_area)
        interp->get_map()->erase(exclusion_area);
    MemoryMap map = *interp->get_map();

    // Figure out what to disassemble.  If we did dynamic linking then we can mark the .got and .got.plt sections as read-only
    // because we've already filled them in with the addresses of the dynamically linked entities.  This will allow the
    // disassembler to know the successors for the indirect JMP instruction in the .plt section (the dynamic function thunks).
    if (do_link) {
        const SgAsmGenericHeaderPtrList &headers = interp->get_headers()->get_headers();
        for (SgAsmGenericHeaderPtrList::const_iterator hi=headers.begin(); hi!=headers.end(); ++hi) {
            SgAsmGenericSectionPtrList sections = (*hi)->get_sections_by_name(".got.plt");      // ELF
            SgAsmGenericSectionPtrList s2 = (*hi)->get_sections_by_name(".got");                // ELF
            SgAsmGenericSectionPtrList s3 = (*hi)->get_sections_by_name(".import");             // PE
            sections.insert(sections.end(), s2.begin(), s2.end());
            sections.insert(sections.end(), s3.begin(), s3.end());
            for (SgAsmGenericSectionPtrList::iterator si=sections.begin(); si!=sections.end(); ++si) {
                if ((*si)->is_mapped()) {
                    Extent mapped_va((*si)->get_mapped_actual_va(), (*si)->get_mapped_size());
                    map.mprotect(mapped_va, MemoryMap::MM_PROT_READ, true/*relax*/);
                }
            }
        }
    }

    // Disassemble the executable
    std::cerr <<argv0 <<": disassembling and partitioning\n";
    if (Disassembler *disassembler = Disassembler::lookup(interp)) {
        disassembler = disassembler->clone(); // so our settings are private
#if 1 // FIXME [Robb P. Matzke 2013-05-14]
        // We need to handle -rose:disassembler_search, -rose:partitioner_search, and -rose:partitioner_config
        // command-line switches.
#endif
        Partitioner *partitioner = new Partitioner();
        SgAsmBlock *gblk = partitioner->partition(interp, disassembler, &map);
        interp->set_global_block(gblk);
        gblk->set_parent(interp);
    } else {
        std::cerr <<argv0 <<": unable to disassemble this specimen\n";
        return NULL;
    }
    return interp;
}



static void
usage(int exit_status)
{
    std::cerr <<"usage: " <<argv0 <<" [SWITCHES] [--] SPECIMEN\n"
              <<"  This command splits a binary into a file per function.\n"
              <<"\n"
              <<"    SPECIMENS\n"
              <<"            Zero or more binary specimen names.\n";
    exit(exit_status);
}

struct Switches {
    Switches(): link(false), save_ast(false) {}
    bool link, save_ast;
};

static struct InstructionSelector: SgAsmFunction::NodeSelector {
    virtual bool operator()(SgNode *node) { return isSgAsmInstruction(node)!=NULL; }
} iselector;

static struct DataSelector: SgAsmFunction::NodeSelector {
    virtual bool operator()(SgNode *node) { return isSgAsmStaticData(node)!=NULL; }
} dselector;


int
main(int argc, char *argv[])
{
    std::ios::sync_with_stdio();
    argv0 = argv[0];
    {
        size_t slash = argv0.rfind('/');
        argv0 = slash==std::string::npos ? argv0 : argv0.substr(slash+1);
        if (0==argv0.substr(0, 3).compare("lt-"))
            argv0 = argv0.substr(3);
    }

    int argno = 1;

    for (/*void*/; argno<argc && '-'==argv[argno][0]; ++argno) {
        std::cout << argv[argno] << std::endl;
        if (!strcmp(argv[argno], "--")) {
            ++argno;
            break;
        } else if (!strcmp(argv[argno], "--help") || !strcmp(argv[argno], "-h")) {
            usage(0);
        } else {
            std::cerr <<argv0 <<": unrecognized switch: " <<argv[argno] <<"\n"
                      <<"see \"" <<argv0 <<" --help\" for usage info.\n";
            exit(1);
        }
    }
    if (argno+1!=argc)
        usage(1);


    std::string specimen_name = StringUtility::getAbsolutePathFromRelativePath(argv[argno++], true);

    std::string specimen_path = StringUtility::getPathFromFileName(specimen_name);

    std::cout << "Specimen name is: " << specimen_name << std::endl;

    SgAsmInterpretation *interp = open_specimen(specimen_name, argv0, false);
    SgBinaryComposite *binfile = SageInterface::getEnclosingNode<SgBinaryComposite>(interp);
    assert(interp!=NULL && binfile!=NULL);
 
    // Figure out what functions we need to generate files from.
    std::vector<SgAsmFunction*> all_functions = SageInterface::querySubTree<SgAsmFunction>(interp);
    std::cerr <<argv0 <<": " <<all_functions.size() <<" function" <<(1==all_functions.size()?"":"s") <<" found\n";

 
    for (std::vector<SgAsmFunction*>::iterator fi=all_functions.begin(); fi!=all_functions.end(); ++fi) {
        // Save function
        SgAsmFunction *func = *fi;

        std::vector<SgAsmInstruction*> insns = SageInterface::querySubTree<SgAsmInstruction>(func);


        std::string function_name = func->get_name();

       if( function_name.size() == 0 || insns.size() < 100 ||  function_name.find("@plt") != std::string::npos  )
        {
          continue;
        }
        std::cout << "function name is: " << function_name << std::endl;

 
        {

          //std::string file_name = specimen_name+"_"+func->get_name()+"_"+boost::lexical_cast<std::string>(func->get_entry_va());
          std::string file_name = specimen_path + "/" + function_name;

          std::cout << "generating " << file_name << " from " << specimen_name << std::endl; 

          std::ofstream func_file;
          func_file.open(file_name.c_str());

          // Save instructions
          for (std::vector<SgAsmInstruction*>::iterator it = insns.begin(); it != insns.end(); ++it) {

            SgUnsignedCharList array = (*it)->get_raw_bytes();

            std::string str = "";
            for(size_t i=0; i < array.size(); ++i) {
              unsigned char c = array[i];

              str+= c;
            }
            func_file << str;
          }


          func_file.close();
        }

    }

    return 0;
}
