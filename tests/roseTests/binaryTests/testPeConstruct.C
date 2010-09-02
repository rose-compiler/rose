/* Tests ability to build a new executable from scratch */
#include "sage3basic.h"

int
main() 
{
    /* See testElfConstruct.C for additional commentary. */

    /* The SgAsmGenericFile is the root of a tree describing a binary file (executable, shared lib, object, core dump). */
    SgAsmGenericFile *ef = new SgAsmGenericFile;

    /* The DOS File Header is the first thing in the file, always at offset zero. The constructors generally take arguments to
     * describe the new object's relationship with existing objects. In this case, the SgAsmDOSFileHeader is a child of the
     * SgAsmGenericFile in the AST. If we were parsing an existing binary file then you would construct the SgAsmDOSFileHeader
     * and then invoke parse(), which recursively constructs and parses everything reachable from the DOS File Header.
     *
     * A freshly constructed DOS File Header isn't too useful since nearly all its data members are initialized to zeros. But
     * all we're using it for is to describe the location of the PE File Header. */
    SgAsmDOSFileHeader *dos_fhdr = new SgAsmDOSFileHeader(ef);

    /* The Extended DOS Header immediately follows the DOS File Header and contains the address of the PE File Header. In this
     * example, we'll place the PE File Header immediately after the Extended DOS File Header. */
    SgAsmDOSExtendedHeader *dos2 = new SgAsmDOSExtendedHeader(dos_fhdr);

    /* Call reallocate() to give the sections we've defined so far a chance to adjust the proper sizes since we'll need to
     * know the final offsets in the next step. */
    ef->reallocate();

    /* The PE File Header follows the DOS Extended Header. We need to store the offset in the extended header. */
    SgAsmPEFileHeader *fhdr = new SgAsmPEFileHeader(ef);
    dos2->set_e_lfanew(fhdr->get_offset());

    /* Most files have a PE Section Table that describes the various sections in the file (the code, data, dynamic linking,
     * symbol tables, string tables, etc). The PE Section Table is a child of the PE File Header. We give it an address
     * that's at the end of the file. Generally, newly-constructed objects will have a size of at least one byte so that when
     * placing new items at the EOF they'll have unique starting offsets (this is important for the functions that resize
     * and/or rearrange sections--they need to know the relative order of sections in the file). */
    SgAsmPESectionTable *sectab = new SgAsmPESectionTable(fhdr);

    /* A generic section can be easily added to the PE file. See compilerWrapper.C for an example of how to specialize a
     * generic section to do something more specific. The section should be added to the PE Section Table or else it will be
     * an orphan and the parser would see it as a hole. */
    SgAsmPESection *s1 = new SgAsmPESection(fhdr);
    s1->get_name()->set_string("test section s1"); /*name will be truncated since PE allows only eight characters*/
    s1->set_size(512);
    s1->set_mapped_preferred_rva(0x4000);
    s1->set_mapped_size(s1->get_size());
    s1->set_mapped_rperm(true);
    s1->set_mapped_wperm(true);
    s1->set_mapped_xperm(false);
    sectab->add_section(s1);

    /* Some of the sections we created above have default sizes of one byte because there's no way to determine their true
     * size until we're all done creating sections.  The SgAsmGenericFile::reallocate() traverses the AST and allocates the
     * actual file storage for each section, adjusting other sections to make room and updating various tables and
     * file-based data structures. */
    ef->reallocate();

    /* Show the results */
    ef->dump(stdout);
    SgAsmGenericSectionPtrList all = ef->get_sections(true);
    for (size_t i=0; i<all.size(); i++) {
        fprintf(stdout, "Section %zu:\n", i);
        all[i]->dump(stdout, "    ", -1);
    }

    /* Unparse the AST to generate an executable. */
    std::ofstream f("a.out");
    ef->unparse(f);

    return 0;
}

    
