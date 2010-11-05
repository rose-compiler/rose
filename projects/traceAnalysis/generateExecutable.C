/* Tests ability to build a new executable from scratch */
//#include "fileoffsetbits.h"
#include "sage3basic.h"


// This file is copied and modified slightly from the file testElfConstruct.C in the tests/roseTests/binaryTests directory.
SgAsmGenericFile*
generateExecutable( SgAsmBlock *block ) 
{
    /* The SgAsmGenericFile is the root of a tree describing a binary file (executable, shared lib, object, core dump).
     * Usually this node appears in a much larger AST containing instructions contexts and other information, but for the
     * purposes of this example, we'll just be generating the ELF Container and not any instructions.  This root will
     * eventually be passed to unparse() to generate the executable file. */
    SgAsmGenericFile *ef = new SgAsmGenericFile;
    
    /* The ELF File Header is the first thing in the file, always at offset zero. The constructors generally take arguments to
     * describe the new object's relationship with existing objects, i.e., its location in the AST. In this case, the
     * SgAsmElfFileHeader is a child of the SgAsmGenericFile (file headers are always children of their respective file).
     *
     * Section constructors (SgAsmElfFileHeader derives from SgAsmGenericSection) always place the new section at the current
     * end-of-file and give it an initial size of one byte.  This ensures that each section has a unique starting address,
     * which is important when file memory is actually allocated for a section and other sections need to be moved out of the
     * way--the allocator needs to know the relative positions of the sections in order to correctly relocate them.
     *
     * If we were parsing an ELF file we would usually use ROSE's frontend() method. However, one can also parse the file by
     * first constructing the SgAsmElfFileHeader and then invoking its parse() method, which parses the ELF File Header and
     * everything that can be reached from that header.
     * 
     * We use 0x400000 as the virtual address of the main LOAD segment, which occupies the first part of the file, up through
     * the end of the .text section (see below). ELF Headers don't actually store a base address, so instead of assigning one
     * to the SgAsmElfFileHeader we'll leave the headers's base address at zero and add base_va explicitly whenever we need
     * it. */
    SgAsmElfFileHeader *fhdr = new SgAsmElfFileHeader(ef);
    fhdr->get_exec_format()->set_word_size(8);                  /* default is 32-bit; we want 64-bit */
    fhdr->set_isa(SgAsmExecutableFileFormat::ISA_X8664_Family); /* instruction set architecture; default is ISA_IA32_386 */
    rose_addr_t base_va = 0x400000;                             /* base virtual address */

    /* ELF executable files always have an ELF Segment Table (also called the ELF Program Header Table) usually appears
     * immediately after the ELF File Header.  The ELF Segment Table describes contiguous regions of the file that should be
     * memory mapped by the loader. ELF Segments don't have names--names are imparted to the segment by virtue of a segment
     * also being described by the ELF Section Table which we'll create later.
     *
     * Note that an SgAsmGenericSection (which represents both ELF Segments and ELF Sections) does not have the ELF Segment
     * Table or ELF Section Table as a parent in the AST, but rather has the ELF File Header as a parent. This allows a single
     * SgAsmGenericSection to represent an ELF Segment, an ELF Section, both, or neither. It also allows us to define ELF
     * Segments and ELF Sections without needing to first define the ELF Segment Table or ELF Section Table, which is
     * particularly useful when one wants one of those tables to appear at the end of the file. */
    SgAsmElfSegmentTable *segtab = new SgAsmElfSegmentTable(fhdr);

#if 1
    /* Create the .text section to hold machine instructions. We'll eventually add it to the ELF Section Table (which will be
     * created at the end of the file) and store its name in an ELF String Section associated with the ELF Section Table. The
     * fact that we haven't created the string table yet doesn't prevent us from giving the section a name.
     *
     * A note about names: Names are represented by SgAsmGenericString objects. The SgAsmGenericString class is virtual and
     * subclasses SgAsmBasicString and SgAsmStoredString represent strings that have no file storage and strings that are
     * stored in the file, respectively.  The stored-string classes are further specialized to handle string tables for
     * various file formats and understand how to map the string into a file, how to allocate and reallocate space, etc.  In
     * general, you want to change the string characters (with set_string()) stored in an SgAsmGenericString that's already
     * present in the AST.
     * 
     * The section constructors and reallocators don't worry about alignment issues--they always allocate from the next
     * available byte. However, instructions typically need to satisfy some alignment constraints. We can easily adjust the
     * file offset chosen by the constructor, but we also need to tell the reallocator about the alignment constraint.
     * 
     * NOTE: There's a slight nuance of the reallocator that must be accounted for if you want to be very precise about where
     *       things are located in the file. When the reallocator shifts sections to make room for a section that needs to
     *       grow, it shifts all affected sections by a uniform amount. This preserves overlaps (like when a segment overlaps
     *       with a bunch of sections) but it also means that the alignment constraint we'll set for ".text" will cause the
     *       ELF Segment Table constructed above at a lower offset to move in tandem with ".text" when the ELF File Header (at
     *       an even lower address) needs to grow to its final size.  This isn't a problem--you just end up with a couple unused
     *       bytes between the header and the segment table that will show up as a "hole" when subsequently parsed. If you're
     *       picky, the correct solution is to give the header a chance to allocate its file space before we construct the
     *       ".text" segment.
     *
     * In order to write the instructions out during the unparse phase, we create a new class from SgAsmElfSection and
     * override the virtual unparse() method.  The reallocate() method is called just before unparsing and we augment that in
     * order to set an ELF-specific bit in the ELF Section Table flags (alternatively, we could have set that bit after the
     * add_section() call below that adds the .text section to the ELF Section Table, but we do it here for demonstration).
     * The calculate_sizes() function is used by SgAsmElfSection::reallocate() to figure out how much space is needed for the
     * section.
     *
     * The instructions correspond to the assembly:
     *     .section .text
     *     .global _start
     * _start:
     *     movl    $1, %eax    # _exit(86)
     *     movl    $86, %ebx
     *     int     $0x80 */
    class TextSection : public SgAsmElfSection {
    public:
        TextSection(SgAsmElfFileHeader *fhdr, size_t ins_size, const unsigned char *ins_bytes)
            : SgAsmElfSection(fhdr), ins_size(ins_size), ins_bytes(ins_bytes)
            {}
        virtual rose_addr_t calculate_sizes(size_t *entsize, size_t *required, size_t *optional, size_t *entcount) const {
            if (entsize)  *entsize  = 1;                        /* an "entry" is one byte of instruction */
            if (required) *required = 1;                        /* each "entry" is also stored in one byte of the file */
            if (optional) *optional = 0;                        /* there is no extra data per instruction byte */
            if (entcount) *entcount = ins_size;                 /* number of "entries" is the total instruction bytes */
            return ins_size;                                    /* return value is section size required */
        }
        virtual bool reallocate() {
            bool retval = SgAsmElfSection::reallocate();        /* returns true if size or position of any section changed */
            SgAsmElfSectionTableEntry *ste = get_section_entry();
            ste->set_sh_flags(ste->get_sh_flags() | SgAsmElfSectionTableEntry::SHF_ALLOC); /* set the SHF_ALLOC bit */
            return retval;
        }
        virtual void unparse(std::ostream &f) const {
            write(f, 0, ins_size, ins_bytes);                   /* Write the instructions at offset zero in section */
        }
        size_t ins_size;
        const unsigned char *ins_bytes;
    };
    ef->reallocate();                                           /* Give existing sections a chance to allocate file space */
    static const unsigned char instructions[] = {0xb8, 0x01, 0x00, 0x00, 0x00, 0xbb, 0x56, 0x00, 0x00, 0x00, 0xcd, 0x80};
    SgAsmElfSection *text = new TextSection(fhdr, NELMTS(instructions), instructions);
    text->set_purpose(SgAsmGenericSection::SP_PROGRAM);         /* Program-supplied text/data/etc. */
    text->set_offset(ALIGN_UP(text->get_offset(), 4));          /* Align on an 8-byte boundary */
    text->set_file_alignment(4);                                /* Tell reallocator about alignment constraint */
    text->set_mapped_alignment(4);                              /* Alignment constraint for memory mapping */
    text->set_mapped_preferred_rva(base_va+text->get_offset()); /* Mapped address is based on file offset */
    text->set_mapped_size(text->get_size());                    /* Mapped size is same as file size */
    text->set_mapped_rperm(true);                               /* Readable */
    text->set_mapped_wperm(false);                              /* Not writable */
    text->set_mapped_xperm(true);                               /* Executable */
    text->get_name()->set_string(".text");                      /* Give section the standard name */
#else
    /* Another way to do the same thing without a temporary class. */
    ef->reallocate();
    static const unsigned char instructions[] = {0xb8, 0x01, 0x00, 0x00, 0x00, 0xbb, 0x56, 0x00, 0x00, 0x00, 0xcd, 0x80};
    SgAsmElfSection *text = new SgAsmElfSection(fhdr);
    text->set_purpose(SgAsmGenericSection::SP_PROGRAM);         /* Program-supplied text/data/etc. */
    text->set_offset(ALIGN_UP(text->get_offset(), 4));          /* Align on an 8-byte boundary */
    text->set_file_alignment(4);                                /* Tell reallocator about alignment constraint */
    text->set_size(sizeof instructions);                        /* Give section a specific size */
    text->set_mapped_alignment(4);                              /* Alignment constraint for memory mapping */
    text->set_mapped_preferred_rva(base_va+text->get_offset()); /* Mapped address is based on file offset */
    text->set_mapped_size(text->get_size());                    /* Mapped size is same as file size */
    text->set_mapped_rperm(true);                               /* Readable */
    text->set_mapped_wperm(false);                              /* Not writable */
    text->set_mapped_xperm(true);                               /* Executable */
    text->get_name()->set_string(".text");                      /* Give section the standard name */
    unsigned char *content = text->writable_content(sizeof instructions);
    memcpy(content, instructions, sizeof instructions);
#endif


    /* Set the main entry point to be the first virtual address in the text section. A rose_rva_t address is associated
     * with a section (.text in this case) so that if the starting address of the section changes (such as via reallocate())
     * then the rose_rva_t address will also change so as to continue to refer to the same byte of the section. */
    rose_rva_t entry_rva(text->get_mapped_preferred_rva(), text);
    fhdr->add_entry_rva(entry_rva);

    /* We now add an entry to the ELF Segment Table. ELF Segments often overlap multiple sections, and in this case we're
     * going to create a segment that overlaps with the ELF File Header, the ELF Segment Table, and the ".text" section. The
     * functions that reallocate space for sections (since most are born with a size of one byte) understand that things may
     * overlap and make appropriate adjustments. In this case all we need to do is set the starting offset and size. */
    SgAsmElfSection *seg1 = new SgAsmElfSection(fhdr);          /* ELF Segments are represented by SgAsmElfSection */
    seg1->get_name()->set_string("LOAD");                       /* Segment names aren't saved (but useful for debugging) */
    seg1->set_offset(0);                                        /* Starts at beginning of file */
    seg1->set_size(text->get_offset() + text->get_size());      /* Extends to end of .text section */
    seg1->set_mapped_preferred_rva(base_va);                    /* Typically mapped by loader to this memory address */
    seg1->set_mapped_size(seg1->get_size());                    /* Make mapped size match size in the file */
    seg1->set_mapped_rperm(true);                               /* Readable */
    seg1->set_mapped_wperm(false);                              /* Not writable */
    seg1->set_mapped_xperm(true);                               /* Executable */
    segtab->add_section(seg1);                                  /* Add definition to ELF Segment Table */

    /* Add an empty PAX segment as further demonstration of adding an ELF Segment. We need to tell the loader that this
     * section holds PAX Flags. There's no interface (currently) to do this at a generic level, so we make an adjustment
     * directly to the ELF Segment Table entry. */
    SgAsmElfSection *pax = new SgAsmElfSection(fhdr);
    pax->get_name()->set_string("PAX Flags");                   /* Name just for debugging */
    pax->set_offset(0);                                         /* Override address to be at zero rather than EOF */
    pax->set_size(0);                                           /* Override size to be zero rather than one byte */
    segtab->add_section(pax);                                   /* Add definition to ELF Segment Table */
    pax->get_segment_entry()->set_type(SgAsmElfSegmentTableEntry::PT_PAX_FLAGS);
    
    /* In order to give sections names there has to be an ELF String Section where the names are stored. An ELF String Section
     * is a section itself and will appear in the ELF Section Table. The name of this string section will be stored in a
     * string section, possibly this same string section. */
    SgAsmElfStringSection *shstrtab = new SgAsmElfStringSection(fhdr);
    shstrtab->get_name()->set_string(".shstrtab");

    /* Most files have an ELF Section Table that describes the various sections in the file (the code, data, dynamic linking,
     * symbol tables, string tables, etc). The ELF Section Table is a child of the ELF File Header, just as all other sections
     * reachable from the ELF File Header are also children of the header.  The class SgAsmGenericSection is used to represent
     * all types of contiguous file regions, whether they're ELF Sections, ELF Segments, PE Objects, etc.
     * 
     * The first entry of an ELF Section Table should always be an all-zero entry. It is not necessary to explicitly create
     * the zero entry since the unparse() method will do so. When parsing an ELF Section Table, the parser will read the
     * all-zero entry and actually create an empty, no-name, section with ID==0 (this is to handle cases that violate the ELF
     * specification).
     * 
     * Section names are stored in an ELF String Section that's associated with the ELF Section Table. We make that
     * association by adding the ELF String Section to the ELF Section Table. The first string table that's added will be the
     * one that stores section names. */
    SgAsmElfSectionTable *sectab = new SgAsmElfSectionTable(fhdr);
    sectab->add_section(text);                                  /* Add the .text section */
    sectab->add_section(shstrtab);                              /* Add the string table to store section names. */

#if 1 /* This stuff is here for demonstration. It's not necessary for the generated a.out to function properly. */

    /* Create a symbol table. Symbol tables should always be added to the ELF Section Table, and thus have a name. ELF Symbol
     * Tables need to know the section that serves as storage for the symbol names, an ELF String Section. The string table
     * for the section name (set above) does not need to be the same as the string table used for symbol names, although we
     * use the same one here just for brevity. */
    SgAsmElfSymbolSection *symtab = new SgAsmElfSymbolSection(fhdr, shstrtab);
    symtab->get_name()->set_string(".dynsym-test");
    symtab->set_is_dynamic(true);                               /* This is a symbol table for dynamic linking */
    sectab->add_section(symtab);                                /* Make the entry in the ELF Section Table */

    /* Add some symbols to the symbol table. Symbols are born with an empty name pointing into the symbol string table and the
     * names should be adjusted by calling get_name()->set_string("xxx"); there is no need to construct new SgAsmStoredString
     * objects and assign them with set_name(). */
    SgAsmElfSymbol *symbol;

    symbol = new SgAsmElfSymbol(symtab);                        /* New symbol is a child of the symbol table */
    symbol->set_value(0xaabbccdd);                              /* Arbitrary symbol value, often an RVA */
    symbol->set_size(512);                                      /* Arbitrary size of the thing to which the symbol refers */
    symbol->get_name()->set_string("symbolA");                  /* Symbol name */

    symbol = new SgAsmElfSymbol(symtab);
    symbol->set_value(0x11223344);
    symbol->set_size(4);
    symbol->get_name()->set_string("symbolB");

    /* Create a .dynamic section. Dynamic sections are generally added to both the section table and the segment table and are
     * memory mapped with read/write permission. They need an ELF String Table in order to store the library names. In real
     * executables the string table is usually separate from the others, but we'll just use the one we already created. */
    SgAsmElfDynamicSection *dynamic = new SgAsmElfDynamicSection(fhdr, shstrtab);
    dynamic->get_name()->set_string(".dynamic-test");           /* Give section a name */
    dynamic->set_mapped_preferred_rva(0x800000);                /* RVA where loader will map section */
    dynamic->set_mapped_size(dynamic->get_size());              /* Make mapped size same as file size */
    dynamic->set_mapped_rperm(true);                            /* Readable */
    dynamic->set_mapped_wperm(true);                            /* Writable */
    dynamic->set_mapped_xperm(false);                           /* Not executable */
    sectab->add_section(dynamic);                               /* Make an ELF Section Table entry */
    segtab->add_section(dynamic);                               /* Make an ELF Segment Table entry */

    /* Create some entries for the .dynamic section. Entries are born with a null name rather than an empty string since most
     * entries don't have a string value. So an entry that requires a string value (like DT_NEEDED) will need to have a new
     * string constructed. You don't have to worry about placing the new string in a particular string table since
     * reallocate() will eventually take care of that, so you can even use the very simple SgAsmBasicString interface. */
    SgAsmElfDynamicEntry *dynent;

    /* Add some DT_NULL entries */
    dynent = new SgAsmElfDynamicEntry(dynamic);
    dynent = new SgAsmElfDynamicEntry(dynamic);
    dynent = new SgAsmElfDynamicEntry(dynamic);

    /* Add a library name */
    dynent = new SgAsmElfDynamicEntry(dynamic);
    dynent->set_d_tag(SgAsmElfDynamicEntry::DT_NEEDED);
    dynent->set_name(new SgAsmBasicString("testlib"));

    /* Create a relocation table. Relocation tables depend on a symbol table and, indirectly, a string table. */
    SgAsmElfRelocSection *reladyn = new SgAsmElfRelocSection(fhdr, symtab,NULL);
    reladyn->get_name()->set_string(".rela.dyn-test");
    reladyn->set_uses_addend(true);                             /* This is a "rela" table rather than a "rel" table */
    sectab->add_section(reladyn);                               /* Make an ELF Section Table entry */

    /* Add a relocation entry to the relocation table. We create just one here, but in practice you would create as many as
     * you need.  Even REL tabels have a set_r_addend() method, but you should leave the addend at zero for them. The
     * relocation type determines how many bits of the offset are significant (and other details) and are described in the
     * system <elf.h> file (we may eventually change this interface to unify the various file formats). */
    SgAsmElfRelocEntry *relent = new SgAsmElfRelocEntry(reladyn);
    relent->set_r_offset(0x080495ac);                           /* An arbitrary offset */
    relent->set_r_addend(0xaa);                                 /* Arbitrary value to add to relocation address */
    relent->set_sym(symtab->index_of(symbol));                  /* Index of symbol in its symbol table */
    relent->set_type(SgAsmElfRelocEntry::R_386_JMP_SLOT);       /*  */

    /* We can add notes to an executable */
    SgAsmElfNoteSection *notes = new SgAsmElfNoteSection(fhdr);
    notes->set_name(new SgAsmBasicString("ELF Notes"));
    notes->set_mapped_preferred_rva(0x900000);                /* RVA where loader will map section */
    notes->set_mapped_size(notes->get_size());              /* Make mapped size same as file size */
    notes->set_mapped_rperm(true);                            /* Readable */
    notes->set_mapped_wperm(true);                            /* Writable */
    notes->set_mapped_xperm(false);                           /* Not executable */
    SgAsmElfNoteEntry *note = new SgAsmElfNoteEntry(notes);
    note->set_name(new SgAsmBasicString("ROSE"));
    const char *payload = "This is a test note.";
    for (size_t i=0; payload[i]; i++) {
        note->get_payload().push_back(payload[i]);
    }
    segtab->add_section(notes);


    /* Add a Symbol Version section.  This is a gnu extension to elf to support versioning of individual symbols 
       the size of the symver section should match the size of the dynamic symbol section (nominally .dynsym).
       there should also be an entry in the .dynamic section [DT_VERDEFNUM] that matches this same number.
     */
    SgAsmElfSymverSection* symver = new SgAsmElfSymverSection(fhdr);
    symver->get_name()->set_string(".gnu-version-test");       /* Give section a name */
    symver->set_mapped_preferred_rva(0xA00000);                /* RVA where loader will map section */
    symver->set_mapped_size(symver->get_size());               /* Make mapped size same as file size */
    symver->set_mapped_rperm(true);                            /* Readable */
    symver->set_mapped_wperm(false);                           /* Writable */
    symver->set_mapped_xperm(false);                           /* Not executable */
    sectab->add_section(symver);                               /* Make an ELF Section Table entry */

    for(size_t i=0; i < symtab->get_symbols()->get_symbols().size(); ++i){
      SgAsmElfSymverEntry* symver_entry = new SgAsmElfSymverEntry(symver);
      symver_entry->set_value(i);
    }
    
    SgAsmElfSymverDefinedSection* symver_def = new SgAsmElfSymverDefinedSection(fhdr,shstrtab);
    symver_def->get_name()->set_string(".gnu.version_d-test");
    symver_def->set_mapped_preferred_rva(0xB00000);                /* RVA where loader will map section */
    symver_def->set_mapped_size(symver_def->get_size());               /* Make mapped size same as file size */
    symver_def->set_mapped_rperm(true);                            /* Readable */
    symver_def->set_mapped_wperm(false);                           /* Writable */
    symver_def->set_mapped_xperm(false);                           /* Not executable */

    sectab->add_section(symver_def);                               /* Make an ELF Section Table entry */

    /** normal case - one Aux structure*/
    SgAsmElfSymverDefinedEntry* symdefEntry1 = new SgAsmElfSymverDefinedEntry(symver_def);
    symdefEntry1->set_version(1);
    symdefEntry1->set_hash(0xdeadbeef);// need to implement real hash support?
    symdefEntry1->set_flags(0x0);
    
    SgAsmElfSymverDefinedAux* symdefAux1a = new SgAsmElfSymverDefinedAux(symdefEntry1, symver_def);
    symdefAux1a->get_name()->set_string("version-1");

    SgAsmElfSymverDefinedEntry* symdefEntry2 = new SgAsmElfSymverDefinedEntry(symver_def);
    new SgAsmElfSymverDefinedAux(symdefEntry2, symver_def);
    new SgAsmElfSymverDefinedAux(symdefEntry2, symver_def);

    /* Very strange case, but technically legal - zero Aux */
    new SgAsmElfSymverDefinedEntry(symver_def);

    SgAsmElfSymverNeededSection* symver_need = new SgAsmElfSymverNeededSection(fhdr,shstrtab);
    symver_need->get_name()->set_string(".gnu.version_d-test");
    symver_need->set_mapped_preferred_rva(0xC00000);                /* RVA where loader will map section */
    symver_need->set_mapped_size(symver_need->get_size());               /* Make mapped size same as file size */
    symver_need->set_mapped_rperm(true);                            /* Readable */
    symver_need->set_mapped_wperm(false);                           /* Writable */
    symver_need->set_mapped_xperm(false);                           /* Not executable */
    sectab->add_section(symver_need);                               /* Make an ELF Section Table entry */


#endif

    /* Some of the sections we created above have default sizes of one byte because there's no way to determine their true
     * size until we're all done creating sections.  The SgAsmGenericFile::reallocate() traverses the AST and allocates the
     * actual file storage for each section, adjusting other sections to make room and updating various tables and
     * file-based data structures. You can call this function at any time, but it must be called before unparse(). */
    ef->reallocate();

    /* The reallocate() has a shortcoming [as of 2008-12-19] in that it might not correctly update memory mappings in the case
     * where the mapping for a section is inferred from the mapping of a containing segment when the section is shifted in the
     * file to make room for an earlier section that's also in the segment.
     * 
     * We can work around that here by explicitly setting the memory mapping to be relative to the segment. However, we'll
     * need to call reallocate() again because besides moving things around, reallocate() also updates some ELF-specific data
     * structures from values stored in the more generic structures. */
    text->set_mapped_preferred_rva(seg1->get_mapped_preferred_rva()+(text->get_offset()-seg1->get_offset()));
    ef->reallocate(); /*won't resize or move things this time since we didn't modify much since the last call to reallocate()*/

    /* Show some results. This is largely the same output as would be found in the *.dump file produced by many of the other
     * binary analysis tools. */
    ef->dump(stdout);
    SgAsmGenericSectionPtrList all = ef->get_sections(true);
    for (size_t i=0; i<all.size(); i++) {
        fprintf(stdout, "Section %zu:\n", i);
        all[i]->dump(stdout, "    ", -1);
    }

    /* Unparse the AST to generate an executable. */
    std::ofstream f("a.out");
    ef->unparse(f);

 // return 0;
    return ef;
}

