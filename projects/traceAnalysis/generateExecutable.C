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
static std::vector<SgAsmElfSection*>
generateTextSections(SgAsmElfFileHeader *fhdr, const Disassembler::InstructionMap &insns, rose_addr_t max_separation=8192)
{
    ROSE_ASSERT(fhdr);
    std::vector<SgAsmElfSection*> sections;
    static const rose_addr_t PAGE_SIZE=4096;

#if 0 /* This is the fast alternative because it doesnt' do any std::map find() operations. */
    
    Disassembler::InstructionMap::const_iterator ii=insns.begin();
    while (ii!=insns.end()) {
        /* Find the first instruction (ij) that's more than max_separation bytes from the end of the previous instruction (after
         * page alignment). This will be the point where we split the instructions into two different text sections.  The "ii"
         * instruction iterator points to the first instruction of this section while the "ij" iterator points to the first
         * instruction of the next section (or end). */
        Disassembler::InstructionMap::const_iterator ij = ii;
        rose_addr_t start_va = ALIGN_DN(ii->first, PAGE_SIZE);
        rose_addr_t end_va = ALIGN_UP(ii->first + ii->second->get_raw_bytes().size(), PAGE_SIZE);
        ROSE_ASSERT(start_va >= fhdr->get_base_va());
        while (ij!=insns.end() && end_va + max_separation >= ALIGN_DN(ij->first, PAGE_SIZE)) {
            end_va = ALIGN_UP(ij->first + ij->second->get_raw_bytes().size(), PAGE_SIZE);
            ++ij;
        }

        /* Build the section */
        SgAsmElfSection *section = new SgAsmElfSection(fhdr);
        section->get_name()->set_string(".text" + StringUtility::numberToString(sections.size()));
        section->set_purpose(SgAsmGenericSection::SP_PROGRAM);
        section->set_file_alignment(PAGE_SIZE);
        section->set_mapped_alignment(PAGE_SIZE);
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
        section_extents.insert(ii->first, ii->second->get_raw_bytes().size(), PAGE_SIZE, PAGE_SIZE);
    section_extents.precipitate(max_separation);

    /* Create the text sections */
    SectionContent content;
    for (ExtentMap::iterator sei=section_extents.begin(); sei!=section_extents.end(); ++sei) {
        SgAsmElfSection *section = new SgAsmElfSection(fhdr);
        section->get_name()->set_string(".text" + StringUtility::numberToString(sections.size()));
        section->set_purpose(SgAsmGenericSection::SP_PROGRAM);
        section->set_file_alignment(PAGE_SIZE);
        section->set_mapped_alignment(PAGE_SIZE);
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

/** Generates an ELF file containing the supplied instructions.  Most of this was copied from one of the test*Construct.C files
 *  in the tests/roseTests/binaryTests directory. */
SgAsmGenericFile *
generateExecutable(const std::string &name, const Disassembler::InstructionMap &insns, rose_addr_t entry_va)
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
    std::vector<SgAsmElfSection*> text = generateTextSections(fhdr, insns, 8192);

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

