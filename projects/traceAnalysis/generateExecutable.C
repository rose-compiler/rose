// This file is based on testElfConstruct.C and testPeConstruct.C in the tests/roseTests/binaryTests directory.
//#include "fileoffsetbits.h"
#include "sage3basic.h"
#include "AsmUnparser_compat.h" /* for unparseInstruction() for debugging [RPM 2010-11-12] */

#define __STDC_FORMAT_MACROS
#include <inttypes.h>

/** Creates text sections from supplied instructions.
 *
 *  The supplied instructions are organized into zero or more text sections such that none of the sections has more than @p
 *  max_separation bytes of unused space between any two instructions.  This is necessary because if the set of instructions
 *  came from an execution trace, its likely to included instructions from widely separated regions of the process address space
 *  (corresponding to instructions from the main executable and each of its shared objects).  If we attempt to place all the
 *  instructions in a single section, then that section would have to be large enough to span from the minimum instruction
 *  address to the maximum instruction address.
 *
 *  Sections are created in order of starting virtual address. They are also defined to be stored in this same order in the file
 *  (which is created later).  Parts of a section that are not filled with instructions will be initialized to zero.
 *
 *  No attempt is to resolve instructions that overlap (the one starting at the higher address is written last).  Rather, we
 *  depend on the caller to have supplied a list of instructions whose encoding bytes are consistent in the overlapping regions,
 *  if any.
 *
 *  The return value is a list of sections that were created. The list is sorted by section preferred virtual address.
 *
 *  Implementation note: Using three loops to do this isn't the most efficient. We could use one loop and avoid a whole bunch 
 *  of map lookups, but three loops is certainly the easy way. */
template<class HeaderType, class SectionType>
static std::vector<SectionType*>
generateTextSections(HeaderType *fhdr, const Disassembler::InstructionMap &insns, const rose_addr_t max_separation=8192,
                     const rose_addr_t mem_align=4096, const rose_addr_t file_align=4096)
{
    ROSE_ASSERT(fhdr);
    std::vector<SectionType*> sections;

#if 0 /* This is the fast alternative because it doesnt' do any std::map find() operations. */
    
    Disassembler::InstructionMap::const_iterator ii=insns.begin();
    while (ii!=insns.end()) {
        /* Find the first instruction (ij) that's more than max_separation bytes from the end of the previous instruction (after
         * page alignment). This will be the point where we split the instructions into two different text sections.  The "ii"
         * instruction iterator points to the first instruction of this section while the "ij" iterator points to the first
         * instruction of the next section (or end). */
        Disassembler::InstructionMap::const_iterator ij = ii;
        rose_addr_t start_va = ALIGN_DN(ii->first, mem_align);
        rose_addr_t end_va = ALIGN_UP(ii->first + ii->second->get_raw_bytes().size(), mem_align);
        ROSE_ASSERT(start_va >= fhdr->get_base_va());
        while (ij!=insns.end() && end_va + max_separation >= ALIGN_DN(ij->first, mem_align)) {
            end_va = ALIGN_UP(ij->first + ij->second->get_raw_bytes().size(), mem_align);
            ++ij;
        }

        /* Build the section */
        SectionType *section = new SectionType(fhdr);
        section->get_name()->set_string(".text" + StringUtility::numberToString(sections.size()));
        section->set_purpose(SgAsmGenericSection::SP_PROGRAM);
        section->set_file_alignment(page_align);
        section->set_mapped_alignment(mem_align);
        section->set_size(end_va - start_va);
        section->set_mapped_preferred_rva(start_va - fhdr->get_base_va());
        section->set_mapped_size(end_va - start_va);
        section->set_mapped_rperm(true);
        section->set_mapped_wperm(false);
        section->set_mapped_xperm(true);
        section->align();
        sections.push_back(section);

        /* Write instructions into the section */
        unsigned char *content = section->writable_content(section->get_size());
        for (/*void*/; ii!=ij; ii++) {
            rose_addr_t offset = ii->first - start_va;
            memcpy(content+offset, &(ii->second->get_raw_bytes()[0]), ii->second->get_raw_bytes().size());
        }
    }

#else /* This is the cleaner alternative because ExtentMap figures out the text sections for us. */

    typedef std::map<rose_addr_t, unsigned char*> SectionContent;

    /* Determine what regions of address space we need for text sections. */
    ExtentMap section_extents;
    for (Disassembler::InstructionMap::const_iterator ii=insns.begin(); ii!=insns.end(); ++ii)
        section_extents.insert(ii->first, ii->second->get_raw_bytes().size(), mem_align, mem_align);
    section_extents.precipitate(max_separation);

    /* Create the text sections */
    SectionContent content;
    for (ExtentMap::iterator sei=section_extents.begin(); sei!=section_extents.end(); ++sei) {
        SectionType *section = new SectionType(fhdr);
        section->get_name()->set_string(".text" + StringUtility::numberToString(sections.size()));
        section->set_purpose(SgAsmGenericSection::SP_PROGRAM);
        section->set_file_alignment(file_align);
        section->set_mapped_alignment(mem_align);
        section->set_size(sei->second);
        section->set_mapped_preferred_rva(sei->first - fhdr->get_base_va());
        section->set_mapped_size(sei->second);
        section->set_mapped_rperm(true);
        section->set_mapped_wperm(false);
        section->set_mapped_xperm(true);
        section->align();                                       /* Necessary because we changed alignment constraints */
        sections.push_back(section);
        content.insert(std::make_pair(sei->first, section->writable_content(sei->second)));
    }
    
    /* Copy instructions into their appropriate sections. */
    for (Disassembler::InstructionMap::const_iterator ii=insns.begin(); ii!=insns.end(); ++ii) {
        SectionContent::iterator ci=content.upper_bound(ii->first);
        --ci;
        rose_addr_t offset = ii->first - ci->first;
        memcpy(ci->second + offset, &(ii->second->get_raw_bytes()[0]), ii->second->get_raw_bytes().size());
    }
    
#endif
    
    return sections;
}

/** Generates a PE file containing the supplied instructions.  Most of this was copied from testPeConstruct.C in the
 * tests/roseTests/binaryTests directory. */
SgAsmGenericFile *
generatePeExecutable(const std::string &name, const Disassembler::InstructionMap &insns, rose_addr_t entry_va)
{
    /* The SgAsmGenericFile is the root of a tree describing a binary file (executable, shared lib, object, core dump). */
    SgAsmGenericFile *ef = new SgAsmGenericFile;

    /***************************************************************************************************************************
     * The DOS part of the file.
     ***************************************************************************************************************************/

    /* The DOS File Header is the first thing in the file, always at offset zero. The constructors generally take arguments to
     * describe the new object's relationship with existing objects. In this case, the SgAsmDOSFileHeader is a child of the
     * SgAsmGenericFile in the AST. If we were parsing an existing binary file then we would construct the SgAsmDOSFileHeader
     * and then invoke parse(), which recursively constructs and parses everything reachable from the DOS File Header.
     *
     * A freshly constructed DOS File Header isn't too useful since nearly all its data members are initialized to zeros. But
     * all we're using it for is to describe the location of the PE File Header. */
    SgAsmDOSFileHeader *dos_fhdr = new SgAsmDOSFileHeader(ef);

    /* The Extended DOS Header immediately follows the DOS File Header and contains the address of the PE File Header. In this
     * example, we'll place the PE File Header immediately after the Extended DOS File Header. */
    SgAsmDOSExtendedHeader *dos2 = new SgAsmDOSExtendedHeader(dos_fhdr);

    /* The optional DOS real-mode text and data section. This isn't actually necessary if the program will only be run in
     * Windows. Normally the DOS program will just print an error message that the program must be run in Windows.  DOS files
     * don't have a section table like many other headers. Instead, the location and size of the text/data segment is stored
     * directly in the DOS header. If the DOS text/data section is created, it must appear directly after the DOS Extended
     * Header in the file. */
    SgAsmGenericSection *dos_rm = new SgAsmGenericSection(ef, dos_fhdr);
    dos_rm->set_size(0x90);
    dos_rm->set_mapped_preferred_rva(0);
    dos_rm->set_mapped_size(dos_rm->get_size());
    dos_rm->set_mapped_rperm(true);
    dos_rm->set_mapped_wperm(true);
    dos_rm->set_mapped_xperm(true);
    dos_fhdr->set_rm_section(dos_rm);
    dos_fhdr->add_entry_rva(rose_rva_t(dos_rm->get_mapped_preferred_rva(), dos_rm));

    /* Call reallocate() to give the sections we've defined so far a chance to adjust to the proper sizes since we'll need to
     * know the final offsets in the next step. */
    ef->reallocate();

    /***************************************************************************************************************************
     * PE File Header
     ***************************************************************************************************************************/

    /* The PE File Header follows the DOS Extended Header. We need to store the offset in the extended header.  PE files usually
     * have a base virtual address of something like 0x01000000. However, the executable we're generating from a trace does not
     * need a particular base address since it will include instructions from not only the main executable, but also any
     * libraries that were (partially) executed. */
    SgAsmPEFileHeader *fhdr = new SgAsmPEFileHeader(ef);
    fhdr->set_base_va(0);                                       /* not typical */
    dos2->set_e_lfanew(fhdr->get_offset());
    ROSE_ASSERT(entry_va>=fhdr->get_base_va());
    fhdr->add_entry_rva(entry_va - fhdr->get_base_va());        /* we know address but not section */

    /***************************************************************************************************************************
     * PE Section Table
     ***************************************************************************************************************************/

    /* Most files have a PE Section Table that describes the various sections in the file (the code, data, dynamic linking,
     * symbol tables, string tables, etc). The PE Section Table is a child of the PE File Header. We give it an address that's
     * at the end of the file. Generally, newly-constructed objects will have a size of at least one byte so that when placing
     * new items at the EOF they'll have unique starting offsets (this is important for the functions that resize and/or
     * rearrange sections--they need to know the relative order of sections in the file). */
    SgAsmPESectionTable *sectab = new SgAsmPESectionTable(fhdr);

    /***************************************************************************************************************************
     * PE Text Section
     ***************************************************************************************************************************/

    /* Most PE executables have just one text section. However, the trace may have instructions that are widely separated in
     * memory (some from the main executable and some from various shared libraries).  We'll create a text section for each area
     * of memory. */
    std::vector<SgAsmPESection*> text = generateTextSections<SgAsmPEFileHeader, SgAsmPESection>(fhdr, insns, 8192, 4096, 512);
    for (std::vector<SgAsmPESection*>::iterator ti=text.begin(); ti!=text.end(); ++ti)
        sectab->add_section(*ti);

    /***************************************************************************************************************************
     * Generate the output.
     ***************************************************************************************************************************/

    /* Some of the sections we created above have default sizes of one byte because there's no way to determine their true
     * size until we're all done creating sections.  The SgAsmGenericFile::reallocate() traverses the AST and allocates the
     * actual file storage for each section, adjusting other sections to make room and updating various tables and
     * file-based data structures. */
    ef->reallocate();

    /* The .text section also contains (parts of) various other sections defined by the RVA/Size pairs. The way we do this is
     * to first declare the .text section to be big enough only for the program instructions, then create the other sections
     * immediately after it, allow all sections to allocate file space, and finally we extend the .text section to overlap with
     * the following sections. */
    // text->set_mapped_size(xxx);
    // text->set_size(xxx);

    /* Unparse the AST to generate an executable. */
    std::ofstream f(name.c_str());
    ef->unparse(f);

    return ef;
}

/** Generates an ELF file containing the supplied instructions.  Most of this was copied from testElfConstruct.C in the
 *  tests/roseTests/binaryTests directory. */
SgAsmGenericFile *
generateElfExecutable(const std::string &name, const Disassembler::InstructionMap &insns, rose_addr_t entry_va)
{
    /* The SgAsmGenericFile is the root of a tree describing a binary file (executable, shared lib, object, core dump).
     * Usually this node appears in a much larger AST containing instructions contexts and other information, but for the
     * purposes of this example, we'll just be generating the ELF Container and not any instructions.  This root will
     * eventually be passed to unparse() to generate the executable file. */
    SgAsmGenericFile *ef = new SgAsmGenericFile;
    
    /* The ELF File Header is the first thing in the file, always at offset zero. The constructors generally take arguments to
     * describe the new object's relationship with existing objects, i.e., its location in the AST. In this case, the
     * SgAsmElfFileHeader is a child of the SgAsmGenericFile (file headers are always children of their respective file). */
    SgAsmElfFileHeader *fhdr = new SgAsmElfFileHeader(ef);
    ROSE_ASSERT(entry_va>=fhdr->get_base_va());                 /* no worries here, since base is probably zero anyway */
    fhdr->add_entry_rva(entry_va - fhdr->get_base_va());        /* we know address but not section */

    /* Place the ELF Segment Table immediately after the ELF File Header just created. We'll fill it in later, after we've
     * determined the addresses of all the ELF Sections. */
    SgAsmElfSegmentTable *segtab = new SgAsmElfSegmentTable(fhdr);

    /* Create text section(s) based on instructions. */
    std::vector<SgAsmElfSection*> text = generateTextSections<SgAsmElfFileHeader, SgAsmElfSection>(fhdr, insns, 8192, 4096, 4096);

    /* Although ELF executables don't need a section table, we'll create one anyway and populate it with all the text
     * sections. A section table needs a string table in which to store section names. The string table usually comes just
     * before the section table in the file.  The string table is a section itself, and appears as an item in the section table.
     * Section tables usually come at the end of the file. */
    SgAsmElfStringSection *shstrtab = new SgAsmElfStringSection(fhdr);
    shstrtab->get_name()->set_string(".shstrtab");
    SgAsmElfSectionTable *sectab = new SgAsmElfSectionTable(fhdr);
    sectab->add_section(shstrtab);

    /* Most ELF executables have just one PT_LOAD section for text, and it usually starts at the beginning of the file (i.e.,
     * includes the ELF File Header and ELF Segment Table). We'll create one for each text section, the first of which will
     * include everything earlier in the file. Since ROSE uses the same class for ELF Segments and ELF Sections, we only need to
     * create the first segment. */
    SgAsmElfSection *segment0 = NULL;
    if (!text.empty()) {
        segment0 = new SgAsmElfSection(fhdr);
        segment0->get_name()->set_string("segment0");           /* ELF does not store name; used only for debugging */
        segment0->set_offset(0);                                /* Start at beginning of file and...*/
        segment0->set_size(text[0]->get_offset()+text[0]->get_size());   /* ...continue through end of first text section */
        segment0->set_mapped_preferred_rva(text[0]->get_mapped_preferred_rva() -
                                          text[0]->get_offset()); /* Address adjusted for pre-text file content */
        segment0->set_mapped_size(segment0->get_size());
        segment0->set_mapped_rperm(true);
        segment0->set_mapped_wperm(false);
        segment0->set_mapped_xperm(true);
        segtab->add_section(segment0);
        sectab->add_section(text[0]);
    }

    /* Add the remaining sections to both the segment and section tables */
    for (size_t i=1; i<text.size(); i++) {
        segtab->add_section(text[i]);
        sectab->add_section(text[i]);
    }
    
    /* Some of the sections we created above have default sizes of one byte because there's no way to determine their true size
     * until we're all done creating things.  The SgAsmGenericFile::reallocate() traverses the AST and allocates the actual file
     * storage and virtual memory for each section, adjusting other sections to make room and updating various tables and
     * file-based data structures. We can call this function at any time, but it must be called before unparse(). */
    ef->reallocate();

    /* Unparse the AST to generate an executable. */
    std::ofstream f(name.c_str());
    ef->unparse(f);
    return ef;
}

