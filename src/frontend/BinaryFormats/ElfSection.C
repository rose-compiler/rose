/* ELF Sections (SgAsmElfSection and related classes) */

// tps (01/14/2010) : Switching from rose.h to sage3.
#include "sage3basic.h"
#define __STDC_FORMAT_MACROS
#include <inttypes.h>
    
/** Constructor for sections that are in neither the ELF Section Table nor the ELF Segment Table yet (but eventually will be) */
void
SgAsmElfSection::ctor()
{
    set_synthesized(false);
    set_purpose(SP_UNSPECIFIED);
}

/** Initializes the section from data parsed from the ELF Section Table. This includes the section name, offset, size, memory
 *  mapping, and alignments. The @p id is the index into the section table. This function complements
 *  SgAsmElfSectionTable::add_section(): this function initializes this section from the section table while add_section()
 *  initializes the section table from the section. */
SgAsmElfSection *
SgAsmElfSection::init_from_section_table(SgAsmElfSectionTableEntry *shdr, SgAsmElfStringSection *strsec, int id)
{
    ROSE_ASSERT(shdr);
    ROSE_ASSERT(strsec);
    ROSE_ASSERT(id>=0);

    /* Purpose */
    switch (shdr->get_sh_type()) {
      case SgAsmElfSectionTableEntry::SHT_PROGBITS:
        set_purpose(SP_PROGRAM);
        break;
      case SgAsmElfSectionTableEntry::SHT_STRTAB:
        set_purpose(SP_HEADER);
        break;
      case SgAsmElfSectionTableEntry::SHT_DYNSYM:
      case SgAsmElfSectionTableEntry::SHT_SYMTAB:
        set_purpose(SP_SYMTAB);
        break;
      default:
        set_purpose(SP_OTHER);
        break;
    }

    /* File mapping */
    set_offset(shdr->get_sh_offset());
    if (SgAsmElfSectionTableEntry::SHT_NULL==shdr->get_sh_type() ||
        SgAsmElfSectionTableEntry::SHT_NOBITS==shdr->get_sh_type()) {
        set_size(0);
    } else {
        set_size(shdr->get_sh_size());
    }
    set_file_alignment(shdr->get_sh_addralign());
    grab_content();

    /* Memory mapping */
    if (shdr->get_sh_addr() > 0) {
        set_mapped_preferred_rva(shdr->get_sh_addr());
        set_mapped_actual_rva(0); /*will be assigned by Loader*/
        set_mapped_size(shdr->get_sh_size());
        set_mapped_rperm(true);
        set_mapped_wperm((shdr->get_sh_flags() & 0x01) == 0x01);
        set_mapped_xperm((shdr->get_sh_flags() & 0x04) == 0x04);
        set_mapped_alignment(shdr->get_sh_addralign());
    } else {
        set_mapped_preferred_rva(0);
        set_mapped_actual_rva(0); /*will be assigned by Loader*/
        set_mapped_size(0);
        set_mapped_rperm(false);
        set_mapped_wperm(false);
        set_mapped_xperm(false);
        set_mapped_alignment(shdr->get_sh_addralign());
    }

    /* Name. This has to be near the end because possibly strsec==this, in which case we have to call set_size() to extend the
     * section to be large enough before we can try to look up the name. */
    set_id(id);
    set_name(new SgAsmStoredString(strsec->get_strtab(), shdr->get_sh_name()));

    /* Add section table entry to section */
    set_section_entry(shdr);
    shdr->set_parent(this);

    return this;
}

/** Initializes the section from data parse from the ELF Segment Table similar to init_from_section_table() */
SgAsmElfSection *
SgAsmElfSection::init_from_segment_table(SgAsmElfSegmentTableEntry *shdr, bool mmap_only)
{
    if (!mmap_only) {
        /* Purpose */
        set_purpose(SP_HEADER);

        /* File mapping */
        set_offset(shdr->get_offset());
        set_size(shdr->get_filesz());
        set_file_alignment(shdr->get_align());
        grab_content();
    
        /* Name */
        char name[128];
        switch (shdr->get_type()) {
          case SgAsmElfSegmentTableEntry::PT_LOAD:         strcpy(name, "ELF Load");          break;
          case SgAsmElfSegmentTableEntry::PT_DYNAMIC:      strcpy(name, "ELF Dynamic");       break;
          case SgAsmElfSegmentTableEntry::PT_INTERP:       strcpy(name, "ELF Interpreter");   break;
          case SgAsmElfSegmentTableEntry::PT_NOTE:         strcpy(name, "ELF Note");          break;
          case SgAsmElfSegmentTableEntry::PT_SHLIB:        strcpy(name, "ELF Shlib");         break;
          case SgAsmElfSegmentTableEntry::PT_PHDR:         strcpy(name, "ELF Segment Table"); break;
          case SgAsmElfSegmentTableEntry::PT_GNU_EH_FRAME: strcpy(name, "GNU EH Frame");      break;
          case SgAsmElfSegmentTableEntry::PT_GNU_STACK:    strcpy(name, "GNU Stack");         break;
          case SgAsmElfSegmentTableEntry::PT_GNU_RELRO:    strcpy(name, "GNU Reloc RO");      break;
          case SgAsmElfSegmentTableEntry::PT_PAX_FLAGS:    strcpy(name, "PAX Flags");         break;
          case SgAsmElfSegmentTableEntry::PT_SUNWBSS:      strcpy(name, "Sun WBSS");          break;
          case SgAsmElfSegmentTableEntry::PT_SUNWSTACK:    strcpy(name, "Sun WStack");        break;
          default:                                         sprintf(name, "ELF type 0x%08x", shdr->get_type()); break;
        }
        sprintf(name+strlen(name), " (segment %zu)", shdr->get_index());
        set_name(new SgAsmBasicString(name));
    }
    
    /* Memory mapping */
    set_mapped_preferred_rva(shdr->get_vaddr());
    set_mapped_actual_rva(0); /*will be assigned by Loader*/
    set_mapped_size(shdr->get_memsz());
    set_mapped_alignment(shdr->get_align());
    set_mapped_rperm(shdr->get_flags() & SgAsmElfSegmentTableEntry::PF_RPERM ? true : false);
    set_mapped_wperm(shdr->get_flags() & SgAsmElfSegmentTableEntry::PF_WPERM ? true : false);
    set_mapped_xperm(shdr->get_flags() & SgAsmElfSegmentTableEntry::PF_XPERM ? true : false);

    /* Add segment table entry to section */
    set_segment_entry(shdr);
    shdr->set_parent(this);

    return this;
}

/** Just a convenience function so we don't need to constantly cast the return value from get_header() */
SgAsmElfFileHeader*
SgAsmElfSection::get_elf_header() const
{
    return dynamic_cast<SgAsmElfFileHeader*>(get_header());
}

/** Returns info about the size of the entries based on information already available. Any or all arguments may be null
 *  pointers if the caller is not interested in the value. Return values are:
 *
 *   entsize  - size of each entry, sum of required and optional parts. This comes from the sh_entsize member of this
 *              section's ELF Section Table Entry, adjusted upward to be large enough to hold the required part of each
 *              entry (see "required").
 *
 *   required - size of the required (leading) part of each entry. The size of the required part is based on the ELF word size.
 *
 *   optional - size of the optional (trailing) part of each entry. If the section has been parsed then the optional size will
 *              be calculated from the entry with the largest "extra" (aka, optional) data. Otherwise this is calculated as the
 *              difference between the "entsize" and the "required" sizes.
 *   
 *   entcount - total number of entries in this section. If the section has been parsed then this is the actual number of
 *              parsed entries, otherwise its the section size divided by the "entsize".
 *
 *  Return value is the total size needed for the section. In all cases, it is entsize*entcount.
 */
rose_addr_t
SgAsmElfSection::calculate_sizes(size_t r32size, size_t r64size,       /*size of required parts*/
                                 const std::vector<size_t> &optsizes,  /*size of optional parts and number of parts parsed*/
                                 size_t *entsize, size_t *required, size_t *optional, size_t *entcount) const
{
    size_t struct_size = 0;
    size_t extra_size = 0;
    size_t entry_size = 0;
    size_t nentries = 0;
    SgAsmElfFileHeader *fhdr = get_elf_header();

    /* Assume ELF Section Table Entry is correct for now for the size of each entry in the table. */
    ROSE_ASSERT(get_section_entry()!=NULL);
    entry_size = get_section_entry()->get_sh_entsize();

    /* Size of required part of each entry */
    if (0==r32size && 0==r64size) {
        /* Probably called by four-argument SgAsmElfSection::calculate_sizes and we don't know the sizes of the required parts
         * because there isn't a parser for this type of section, or the section doesn't contain a table. In the latter case
         * the ELF Section Table has a zero sh_entsize and we'll treat the section as if it were a table with one huge entry.
         * Otherwise we'll assume that the struct size is the same as the sh_entsize and there's no optional data. */
        struct_size = entry_size>0 ? entry_size : get_size();
    } else if (4==fhdr->get_word_size()) {
        struct_size = r32size;
    } else if (8==fhdr->get_word_size()) {
        struct_size = r64size;
    } else {
        throw FormatError("bad ELF word size");
    }

    /* Entire entry should be at least large enough for the required part. This also takes care of the case when the ELF
     * Section Table Entry has a zero-valued sh_entsize */
    entry_size = std::max(entry_size, struct_size);

    /* Size of optional parts. If we've parsed the table then use the largest optional part, otherwise assume the entry from
     * the ELF Section Table is correct. */
    nentries = optsizes.size();
    if (nentries>0) {
        for (size_t i=0; i<nentries; i++) {
            extra_size = std::max(extra_size, optsizes[i]);
        }
        entry_size = std::min(entry_size, struct_size+extra_size);
    } else {
        extra_size = entry_size - struct_size;
        nentries = entry_size>0 ? get_size() / entry_size : 0;
    }

    /* Return values */
    if (entsize)
        *entsize = entry_size;
    if (required)
        *required = struct_size;
    if (optional)
        *optional = extra_size;
    if (entcount)
        *entcount = nentries;
    return entry_size * nentries;
}

/** Most subclasses will override this virtual function in order to return more useful values. This implementation returns the
 *  following values:
 *   entsize  -- size stored in the ELF Section Table's sh_entsize member, or size of entire section if not a table.
 *   required -- same as entsize
 *   optional -- zero
 *   entcount -- number of entries, each of size entsize, that can fit in the section.
 *  The return size is entsize*entcount, which, if this section is a table (nonzero sh_entsize), could be smaller than the
 *  total size of the section. */
rose_addr_t
SgAsmElfSection::calculate_sizes(size_t *entsize, size_t *required, size_t *optional, size_t *entcount) const
{
    return calculate_sizes(0, 0, std::vector<size_t>(), entsize, required, optional, entcount);
}

/** Called prior to unparse to make things consistent. */
bool
SgAsmElfSection::reallocate()
{
    bool reallocated = false;
    SgAsmElfSectionTableEntry *sechdr = get_section_entry();
    SgAsmElfSegmentTableEntry *seghdr = get_segment_entry();

    /* Change section size if this section was defined in the ELF Section Table */
    if (sechdr!=NULL) {
        addr_t need = calculate_sizes(NULL, NULL, NULL, NULL);
        if (need < get_size()) {
            if (is_mapped()) {
                ROSE_ASSERT(get_mapped_size()==get_size());
                set_mapped_size(need);
            }
            set_size(need);
            reallocated = true;
        } else if (need > get_size()) {
            get_file()->shift_extend(this, 0, need-get_size(), SgAsmGenericFile::ADDRSP_ALL, SgAsmGenericFile::ELASTIC_HOLE);
            reallocated = true;
        }
    }

    /* Update entry in the ELF Section Table and/or ELF Segment Table */
    if (sechdr)
        sechdr->update_from_section(this);
    if (seghdr)
        seghdr->update_from_section(this);
    
    return reallocated;
}

/** Print some debugging info */
void
SgAsmElfSection::dump(FILE *f, const char *prefix, ssize_t idx) const
{
    char p[4096];
    if (idx>=0) {
        sprintf(p, "%sElfSection[%zd].", prefix, idx);
    } else {
        sprintf(p, "%sElfSection.", prefix);
    }
    int w = std::max(1, DUMP_FIELD_WIDTH-(int)strlen(p));
    
    SgAsmGenericSection::dump(f, p, -1);
    
    if (get_section_entry())
        get_section_entry()->dump(f, p, -1);
    if (get_segment_entry())
        get_segment_entry()->dump(f, p, -1);

    if (p_linked_section) {
        fprintf(f, "%s%-*s = [%d] \"%s\"\n", p, w, "linked_to",
                p_linked_section->get_id(), p_linked_section->get_name()->c_str());
    } else {
        fprintf(f, "%s%-*s = NULL\n",    p, w, "linked_to");
    }

    if (variantT() == V_SgAsmElfSection) //unless a base class
        hexdump(f, 0, std::string(p)+"data at ", p_data);
}
