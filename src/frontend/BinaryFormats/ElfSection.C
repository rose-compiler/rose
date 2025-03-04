/* ELF Sections (SgAsmElfSection and related classes) */
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include "sage3basic.h"

#include <Rose/BinaryAnalysis/Hexdump.h>

SgAsmElfSection*
SgAsmElfSection::get_linked_section() const {
    return get_linkedSection();
}

void
SgAsmElfSection::set_linked_section(SgAsmElfSection* x) {
    set_linkedSection(x);
}

void
SgAsmElfSection::set_linkedSection(SgAsmElfSection* linked_section) {
    set_isModified(true);
    p_linkedSection = linked_section;
}

SgAsmElfSection::SgAsmElfSection(SgAsmGenericHeader *fhdr)
    : SgAsmGenericSection(fhdr->get_file(), fhdr) {
    initializeProperties();
    set_synthesized(false);
    set_purpose(SP_UNSPECIFIED);
}

SgAsmElfSection *
SgAsmElfSection::init_from_section_table(SgAsmElfSectionTableEntry *shdr, SgAsmElfStringSection *strsec, int id)
{
    return initFromSectionTable(shdr, strsec, id);
}

SgAsmElfSection *
SgAsmElfSection::initFromSectionTable(SgAsmElfSectionTableEntry *shdr, SgAsmElfStringSection *strsec, int id)
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
    set_fileAlignment(shdr->get_sh_addralign());
    grabContent();

    /* Memory mapping */
    if (shdr->get_sh_addr() > 0) {
        set_mappedPreferredRva(shdr->get_sh_addr());
        set_mappedActualVa(0); /*will be assigned by Loader*/
        set_mappedSize(shdr->get_sh_size());
        set_mappedReadPermission(true);
        set_mappedWritePermission((shdr->get_sh_flags() & 0x01) == 0x01);
        set_mappedExecutePermission((shdr->get_sh_flags() & 0x04) == 0x04);
        set_mappedAlignment(shdr->get_sh_addralign());
    } else {
        set_mappedPreferredRva(0);
        set_mappedActualVa(0); /*will be assigned by Loader*/
        set_mappedSize(0);
        set_mappedReadPermission(false);
        set_mappedWritePermission(false);
        set_mappedExecutePermission(false);
        set_mappedAlignment(shdr->get_sh_addralign());
    }

    /* Name. This has to be near the end because possibly strsec==this, in which case we have to call set_size() to extend the
     * section to be large enough before we can try to look up the name. */
    set_id(id);
    set_name(new SgAsmStoredString(strsec->get_strtab(), shdr->get_sh_name()));

    /* Add section table entry to section */
    set_sectionEntry(shdr);
    shdr->set_parent(this);

    return this;
}

SgAsmElfSection *
SgAsmElfSection::init_from_segment_table(SgAsmElfSegmentTableEntry *shdr, bool mmap_only)
{
    return initFromSegmentTable(shdr, mmap_only);
}

SgAsmElfSection *
SgAsmElfSection::initFromSegmentTable(SgAsmElfSegmentTableEntry *shdr, bool mmap_only)
{
    if (!mmap_only) {
        /* Purpose */
        set_purpose(SP_HEADER);

        /* File mapping */
        set_offset(shdr->get_offset());
        set_size(shdr->get_filesz());
        set_fileAlignment(shdr->get_align());
        grabContent();
    
        /* Name */
        char name[128];
        switch (shdr->get_type()) {
            case SgAsmElfSegmentTableEntry::PT_LOAD:         strncpy(name, "LOAD", sizeof(name)-1);        break;
            case SgAsmElfSegmentTableEntry::PT_DYNAMIC:      strncpy(name, "DYNAMIC", sizeof(name)-1);     break;
            case SgAsmElfSegmentTableEntry::PT_INTERP:       strncpy(name, "INTERP", sizeof(name)-1);      break;
            case SgAsmElfSegmentTableEntry::PT_NOTE:         strncpy(name, "NOTE", sizeof(name)-1);        break;
            case SgAsmElfSegmentTableEntry::PT_SHLIB:        strncpy(name, "SHLIB", sizeof(name)-1);       break;
            case SgAsmElfSegmentTableEntry::PT_PHDR:         strncpy(name, "PHDR", sizeof(name)-1);        break;
            case SgAsmElfSegmentTableEntry::PT_TLS:          strncpy(name, "TLS", sizeof(name)-1);         break;
            case SgAsmElfSegmentTableEntry::PT_GNU_EH_FRAME: strncpy(name, "EH_FRAME", sizeof(name)-1);    break;
            case SgAsmElfSegmentTableEntry::PT_GNU_STACK:    strncpy(name, "GNU_STACK", sizeof(name)-1);   break;
            case SgAsmElfSegmentTableEntry::PT_GNU_RELRO:    strncpy(name, "GNU_RELRO", sizeof(name)-1);   break;
            case SgAsmElfSegmentTableEntry::PT_PAX_FLAGS:    strncpy(name, "PAX_FLAGS", sizeof(name)-1);   break;
            case SgAsmElfSegmentTableEntry::PT_SUNWBSS:      strncpy(name, "SUNWBSS", sizeof(name)-1);     break;
            case SgAsmElfSegmentTableEntry::PT_SUNWSTACK:    strncpy(name, "SUNWSTACK", sizeof(name)-1);   break;
            default:                                         snprintf(name, sizeof(name), "PT_0x%08x", shdr->get_type()); break;
        }
        snprintf(name+strlen(name), sizeof(name)-strlen(name), "#%" PRIuPTR "", shdr->get_index());
        set_name(new SgAsmBasicString(name));
    }
    
    /* Memory mapping */
    set_mappedPreferredRva(shdr->get_vaddr());
    set_mappedActualVa(0); /*will be assigned by Loader*/
    set_mappedSize(shdr->get_memsz());
    set_mappedAlignment(shdr->get_align());
    set_mappedReadPermission(shdr->get_flags() & SgAsmElfSegmentTableEntry::PF_RPERM ? true : false);
    set_mappedWritePermission(shdr->get_flags() & SgAsmElfSegmentTableEntry::PF_WPERM ? true : false);
    set_mappedExecutePermission(shdr->get_flags() & SgAsmElfSegmentTableEntry::PF_XPERM ? true : false);

    /* Add segment table entry to section */
    set_segmentEntry(shdr);
    shdr->set_parent(this);

    return this;
}

SgAsmElfFileHeader*
SgAsmElfSection::get_elf_header() const
{
    return get_elfHeader();
}

SgAsmElfFileHeader*
SgAsmElfSection::get_elfHeader() const
{
    return dynamic_cast<SgAsmElfFileHeader*>(get_header());
}

Rose::BinaryAnalysis::Address
SgAsmElfSection::calculate_sizes(size_t r32size, size_t r64size,       /*size of required parts*/
                                 const std::vector<size_t> &optsizes,  /*size of optional parts and number of parts parsed*/
                                 size_t *entsize, size_t *required, size_t *optional, size_t *entcount) const {
    return calculateSizes(r32size, r64size, optsizes, entsize, required, optional, entcount);
}

Rose::BinaryAnalysis::Address
SgAsmElfSection::calculateSizes(size_t r32size, size_t r64size,       /*size of required parts*/
                                const std::vector<size_t> &optsizes,  /*size of optional parts and number of parts parsed*/
                                size_t *entsize, size_t *required, size_t *optional, size_t *entcount) const
{
    size_t struct_size = 0;
    size_t extra_size = 0;
    size_t entry_size = 0;
    size_t nentries = 0;
    SgAsmElfFileHeader *fhdr = get_elfHeader();

    /* Assume ELF Section Table Entry is correct for now for the size of each entry in the table. */
    ROSE_ASSERT(get_sectionEntry()!=NULL);
    entry_size = get_sectionEntry()->get_sh_entsize();

    /* Size of required part of each entry */
    if (0==r32size && 0==r64size) {
        /* Probably called by four-argument SgAsmElfSection::calculateSizes and we don't know the sizes of the required parts
         * because there isn't a parser for this type of section, or the section doesn't contain a table. In the latter case
         * the ELF Section Table has a zero sh_entsize and we'll treat the section as if it were a table with one huge entry.
         * Otherwise we'll assume that the struct size is the same as the sh_entsize and there's no optional data. */
        struct_size = entry_size>0 ? entry_size : get_size();
    } else if (4==fhdr->get_wordSize()) {
        struct_size = r32size;
    } else if (8==fhdr->get_wordSize()) {
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

Rose::BinaryAnalysis::Address
SgAsmElfSection::calculate_sizes(size_t *entsize, size_t *required, size_t *optional, size_t *entcount) const
{
    return calculateSizes(entsize, required, optional, entcount);
}

Rose::BinaryAnalysis::Address
SgAsmElfSection::calculateSizes(size_t *entsize, size_t *required, size_t *optional, size_t *entcount) const
{
    return calculateSizes(0, 0, std::vector<size_t>(), entsize, required, optional, entcount);
}

bool
SgAsmElfSection::reallocate()
{
    bool reallocated = false;
    SgAsmElfSectionTableEntry *sechdr = get_sectionEntry();
    SgAsmElfSegmentTableEntry *seghdr = get_segmentEntry();

    /* Change section size if this section was defined in the ELF Section Table */
    if (sechdr!=NULL) {
        Rose::BinaryAnalysis::Address need = calculateSizes(NULL, NULL, NULL, NULL);
        if (need < get_size()) {
            if (isMapped()) {
                ROSE_ASSERT(get_mappedSize()==get_size());
                set_mappedSize(need);
            }
            set_size(need);
            reallocated = true;
        } else if (need > get_size()) {
            get_file()->shiftExtend(this, 0, need-get_size(), SgAsmGenericFile::ADDRSP_ALL, SgAsmGenericFile::ELASTIC_HOLE);
            reallocated = true;
        }
    }

    /* Update entry in the ELF Section Table and/or ELF Segment Table */
    if (sechdr)
        sechdr->updateFromSection(this);
    if (seghdr)
        seghdr->updateFromSection(this);
    
    return reallocated;
}

void
SgAsmElfSection::dump(FILE *f, const char *prefix, ssize_t idx) const
{
    char p[4096];
    if (idx>=0) {
        snprintf(p, sizeof(p), "%sElfSection[%zd].", prefix, idx);
    } else {
        snprintf(p, sizeof(p), "%sElfSection.", prefix);
    }
    int w = std::max(size_t{1}, Rose::DUMP_FIELD_WIDTH - strlen(p));
    
    SgAsmGenericSection::dump(f, p, -1);
    
    if (get_sectionEntry())
        get_sectionEntry()->dump(f, p, -1);
    if (get_segmentEntry())
        get_segmentEntry()->dump(f, p, -1);

    if (get_linkedSection()) {
        fprintf(f, "%s%-*s = [%d] \"%s\"\n", p, w, "linked_to",
                get_linkedSection()->get_id(), get_linkedSection()->get_name()->get_string(true).c_str());
    } else {
        fprintf(f, "%s%-*s = NULL\n",    p, w, "linked_to");
    }

    if (variantT() == V_SgAsmElfSection) //unless a base class
        Rose::BinaryAnalysis::hexdump(f, 0, std::string(p)+"data at ", p_data);
}

SgAsmElfSectionTableEntry*
SgAsmElfSection::get_section_entry() const {
    return get_sectionEntry();
}

void
SgAsmElfSection::set_section_entry(SgAsmElfSectionTableEntry *x) {
    set_sectionEntry(x);
}

SgAsmElfSegmentTableEntry*
SgAsmElfSection::get_segment_entry() const {
    return get_segmentEntry();
}

void
SgAsmElfSection::set_segment_entry(SgAsmElfSegmentTableEntry *x) {
    set_segmentEntry(x);
}

void
SgAsmElfSection::finish_parsing() {
    finishParsing();
}

void
SgAsmElfSection::finishParsing() {}

void
SgAsmElfSection::allocate_name_to_storage(SgAsmElfStringSection *strsec)
{
    allocateNameToStorage(strsec);
}

void
SgAsmElfSection::allocateNameToStorage(SgAsmElfStringSection *strsec)
{
    if (get_name()) {
        SgAsmStoredString *old_stored = dynamic_cast<SgAsmStoredString*>(get_name());
        if (!old_stored || old_stored->get_strtab()!=strsec->get_strtab()) {
            /* Reallocate string to new string table */
            SgAsmStoredString *new_stored = new SgAsmStoredString(strsec->get_strtab(), 0);
            new_stored->set_string(get_name()->get_string());
            get_name()->set_string(""); /*free old string*/
            set_name(new_stored);
        }
    }
}

#endif
