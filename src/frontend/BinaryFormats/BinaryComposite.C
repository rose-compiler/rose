/* SgBinaryComposite is a container for binary files. */
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include "sage3basic.h"

#include <Rose/BinaryAnalysis/BinaryLoader.h>
#include <Rose/BinaryAnalysis/Dwarf/BasicTypes.h>
#include <Rose/BinaryAnalysis/Partitioner2/Engine.h>

using std::string;
using std::vector;
using namespace Rose;
using namespace Rose::Diagnostics; // for mlog, INFO, WARN, ERROR, FATAL, etc.

SgBinaryComposite::SgBinaryComposite(vector<string> &argv, SgProject* project)
  : SgBinaryComposite()
{
    SgFile::doSetupForConstructor(argv, project);
}

void
SgBinaryComposite::post_construction_initialization() {
    // Don't call parent initialization or allow it to be called twice (this function is virtual!)
    if (p_genericFileList == nullptr) {
        p_genericFileList = new SgAsmGenericFileList();
        p_genericFileList->set_parent(this);
    }

    if (p_interpretations == nullptr) {
        p_interpretations = new SgAsmInterpretationList();
        p_interpretations->set_parent(this);
    }

    Rose::is_binary_executable = true;
    set_binary_only(true);

    set_outputLanguage(SgFile::e_Binary_language);
    set_inputLanguage(SgFile::e_Binary_language);
    set_sourceFileUsesBinaryFileExtension(true);

    // Don't do C++ stuff
    set_requires_C_preprocessor(false);
    set_disable_edg_backend(true);
    set_disable_sage_backend(true);
    set_skip_transformation(true);
    set_skip_commentsAndDirectives(true);
}

int SgBinaryComposite::callFrontEnd() {
    return SgFile::callFrontEnd();
}

/* Parses a single binary file and adds a SgAsmGenericFile node under this SgBinaryComposite node. */
void
SgBinaryComposite::buildAsmAST(string executableFileName) {
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
    if (get_verbose() > 0 || SgProject::get_verbose() > 0)
        printf ("Disassemble executableFileName = %s \n",executableFileName.c_str());

    // Parse the binary container, but do not disassemble instructions yet.
    SgAsmGenericFile *file = SgAsmExecutableFileFormat::parseBinaryFormat(executableFileName.c_str());
    ASSERT_not_null(file);

    // Attach the file to this node
    get_genericFileList()->get_files().push_back(file);
    file->set_parent(get_genericFileList());

    // Add a disassembly interpretation for each header. Actual disassembly will occur later.
    // NOTE: This probably isn't the right place to add interpretation nodes, but I'm leaving it here for the time being. We
    //       probably don't want an interpretation for each header if we're doing dynamic linking. [RPM 2009-09-17]
    const SgAsmGenericHeaderPtrList &headers = file->get_headers()->get_headers();
    for (size_t i = 0; i < headers.size(); ++i) {
        SgAsmInterpretation* interp = new SgAsmInterpretation();
        get_interpretations()->get_interpretations().push_back(interp);
        interp->set_parent(get_interpretations());
        interp->get_headers()->get_headers().push_back(headers[i]);
    }

    // DQ (3/14/2009): Dwarf support now works within ROSE when used with Intel Pin
    // (was a huge problem until everything (e.g. libdwarf) was dynamically linked).
    // DQ (11/7/2008): New Dwarf support in ROSE (Dwarf IR nodes are generated in the AST).
    BinaryAnalysis::Dwarf::parse(file);

    // Make sure this node is correctly parented
    ROSE_ASSERT(SageInterface::getEnclosingNode<SgProject>(this));

#else
    // DQ (2/21/2016): Added "error: " to allow this to be caught by the ROSE Matrix Testing.
    ROSE_ASSERT (! "[FATAL] [ROSE] [frontend] [Binary analysis] "
                 "error: ROSE was not configured to support the binary analysis frontend.");
#endif
}

/* Builds the entire AST under the SgBinaryComposite node:
 *    - figures out what binary files are needed
 *    - parses binary container of each file (SgAsmGenericFile nodes)
 *    - optionally disassembles instructions (SgAsmInterpretation nodes) */
int
SgBinaryComposite::buildAST(vector<string> /*argv*/, vector<string> /*inputCommandLine*/) {
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
    /* Parse the specified binary file to create the AST. Do not disassemble instructions yet. If the file is dynamically
     * linked then optionally load (i.e., parse the container, map sections into process address space, and perform relocation
     * fixups) all dependencies also.  See the BinaryLoader class for details. */
    if (get_isLibraryArchive()) {
        ROSE_ASSERT(get_libraryArchiveObjectFileNameList().empty() == false);
        ROSE_ASSERT(get_libraryArchiveObjectFileNameList().empty() == (get_isLibraryArchive() == false));

        for (size_t i = 0; i < get_libraryArchiveObjectFileNameList().size(); i++) {
            printf("Build binary AST for get_libraryArchiveObjectFileNameList()[%" PRIuPTR "] = %s \n",
                   i, get_libraryArchiveObjectFileNameList()[i].c_str());
            string filename = get_libraryArchiveObjectFileNameList()[i];
            printf("Build SgAsmGenericFile from: %s \n", filename.c_str());
            buildAsmAST(filename);
        }
    } else {
        ROSE_ASSERT(get_libraryArchiveObjectFileNameList().empty());
        BinaryAnalysis::BinaryLoader::load(this, get_read_executable_file_format_only());
    }

    // Disassemble each interpretation
    if (!get_read_executable_file_format_only()) {
        const SgAsmInterpretationPtrList &interps = get_interpretations()->get_interpretations();
        for (size_t i=0; i<interps.size(); i++) {
            Rose::BinaryAnalysis::Partitioner2::Engine::disassembleForRoseFrontend(interps[i]);
        }
    }

    // DQ (1/22/2008): The generated unparsed assemble code can not currently be compiled because the
    // addresses are unparsed (see Jeremiah for details).
    // Skip running gnu assemble on the output since we include text that would make this a problem.
    if (get_verbose() > 1)
        printf("set_skipfinalCompileStep(true) because we are on a binary '%s'\n", this->get_sourceFileNameWithoutPath().c_str());

    this->set_skipfinalCompileStep(true);

#else
    // DQ (2/21/2016): Added "error: " to allow this to be caught by the ROSE Matrix Testing.
    ROSE_ASSERT (! "[FATAL] [ROSE] [frontend] [Binary analysis] "
                 "error: ROSE was not configured to support the binary analysis frontend.");
#endif

    int frontendErrorLevel = 0;
    return frontendErrorLevel;
}

#endif // ROSE_ENABLE_BINARY_ANALYSIS
