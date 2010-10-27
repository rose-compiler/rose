/* ELF Segment Tables (SgAsmElfSegmentTable and related classes) */
#include "sage3basic.h"
#include "stringify.h"
#define __STDC_FORMAT_MACROS
#include <inttypes.h>

/** Converts 32-bit disk representation to host representation */
void
SgAsmElfSegmentTableEntry::ctor(ByteOrder sex, const struct Elf32SegmentTableEntry_disk *disk) 
{
    p_type      = (SegmentType)disk_to_host(sex, disk->p_type);
    p_offset    = disk_to_host(sex, disk->p_offset);
    p_vaddr     = disk_to_host(sex, disk->p_vaddr);
    p_paddr     = disk_to_host(sex, disk->p_paddr);
    p_filesz    = disk_to_host(sex, disk->p_filesz);
    p_memsz     = disk_to_host(sex, disk->p_memsz);
    p_flags     = (SegmentFlags)disk_to_host(sex, disk->p_flags);
    p_align     = disk_to_host(sex, disk->p_align);
}

/** Converts 64-bit disk representation to host representation */
void
SgAsmElfSegmentTableEntry::ctor(ByteOrder sex, const Elf64SegmentTableEntry_disk *disk) 
{
    p_type      = (SegmentType)disk_to_host(sex, disk->p_type);
    p_offset    = disk_to_host(sex, disk->p_offset);
    p_vaddr     = disk_to_host(sex, disk->p_vaddr);
    p_paddr     = disk_to_host(sex, disk->p_paddr);
    p_filesz    = disk_to_host(sex, disk->p_filesz);
    p_memsz     = disk_to_host(sex, disk->p_memsz);
    p_flags     = (SegmentFlags)disk_to_host(sex, disk->p_flags);
    p_align     = disk_to_host(sex, disk->p_align);
}

/** Converts segment table entry back into disk structure */
void *
SgAsmElfSegmentTableEntry::encode(ByteOrder sex, Elf32SegmentTableEntry_disk *disk) const
{
    host_to_disk(sex, p_type, &(disk->p_type));
    host_to_disk(sex, p_offset, &(disk->p_offset));
    host_to_disk(sex, p_vaddr, &(disk->p_vaddr));
    host_to_disk(sex, p_paddr, &(disk->p_paddr));
    host_to_disk(sex, p_filesz, &(disk->p_filesz));
    host_to_disk(sex, p_memsz, &(disk->p_memsz));
    host_to_disk(sex, p_flags, &(disk->p_flags));
    host_to_disk(sex, p_align, &(disk->p_align));
    return disk;
}
void *
SgAsmElfSegmentTableEntry::encode(ByteOrder sex, Elf64SegmentTableEntry_disk *disk) const
{
    host_to_disk(sex, p_type, &(disk->p_type));
    host_to_disk(sex, p_offset, &(disk->p_offset));
    host_to_disk(sex, p_vaddr, &(disk->p_vaddr));
    host_to_disk(sex, p_paddr, &(disk->p_paddr));
    host_to_disk(sex, p_filesz, &(disk->p_filesz));
    host_to_disk(sex, p_memsz, &(disk->p_memsz));
    host_to_disk(sex, p_flags, &(disk->p_flags));
    host_to_disk(sex, p_align, &(disk->p_align));
    return disk;
}

/** Update this segment table entry with newer information from the section */
void
SgAsmElfSegmentTableEntry::update_from_section(SgAsmElfSection *section)
{
    set_offset(section->get_offset());
    set_filesz(section->get_size());
    set_vaddr(section->get_mapped_preferred_va());
    set_memsz(section->get_mapped_size());
    set_align(section->is_mapped() ? section->get_mapped_alignment() : section->get_file_alignment());

    if (section->get_mapped_rperm()) {
        set_flags((SegmentFlags)(p_flags | PF_RPERM));
    } else {
        set_flags((SegmentFlags)(p_flags & ~PF_RPERM));
    }
    if (section->get_mapped_wperm()) {
        set_flags((SegmentFlags)(p_flags | PF_WPERM));
    } else {
        set_flags((SegmentFlags)(p_flags & ~PF_WPERM));
    }
    if (section->get_mapped_xperm()) {
        set_flags((SegmentFlags)(p_flags | PF_XPERM));
    } else {
        set_flags((SegmentFlags)(p_flags & ~PF_XPERM));
    }

    if (isSgAsmElfNoteSection(section)) {
        set_type(PT_NOTE);
    }
}

/** Print some debugging info */
void
SgAsmElfSegmentTableEntry::dump(FILE *f, const char *prefix, ssize_t idx) const
{
    char p[4096];
    if (idx>=0) {
        sprintf(p, "%sElfSegmentTableEntry[%zd].", prefix, idx);
    } else {
        sprintf(p, "%sElfSegmentTableEntry.", prefix);
    }
    const int w = std::max(1, DUMP_FIELD_WIDTH-(int)strlen(p));

    fprintf(f, "%s%-*s = %zu\n",                             p, w, "index",  p_index);
    fprintf(f, "%s%-*s = 0x%08x = %s\n",                     p, w, "type",   p_type,  to_string(p_type).c_str());
    fprintf(f, "%s%-*s = 0x%08x ",                           p, w, "flags",  p_flags);
    fputc(p_flags & PF_RPERM ? 'r' : '-', f);
    fputc(p_flags & PF_WPERM ? 'w' : '-', f);
    fputc(p_flags & PF_XPERM ? 'x' : '-', f);
    if (p_flags & PF_OS_MASK) fputs(" os", f);
    if (p_flags & PF_PROC_MASK) fputs(" proc", f);
    if (p_flags & PF_RESERVED) fputs(" *", f);
    fputc('\n', f);
    fprintf(f, "%s%-*s = 0x%08"PRIx64" (%"PRIu64") bytes into file\n", p, w, "offset", p_offset, p_offset);
    fprintf(f, "%s%-*s = 0x%08"PRIx64" (%"PRIu64")\n",                 p, w, "vaddr",  p_vaddr, p_vaddr);
    fprintf(f, "%s%-*s = 0x%08"PRIx64" (%"PRIu64")\n",                 p, w, "paddr",  p_paddr, p_paddr);
    fprintf(f, "%s%-*s = 0x%08"PRIx64" (%"PRIu64") bytes\n",           p, w, "filesz", p_filesz, p_filesz);
    fprintf(f, "%s%-*s = 0x%08"PRIx64" (%"PRIu64") bytes\n",           p, w, "memsz",  p_memsz, p_memsz);
    fprintf(f, "%s%-*s = 0x%08"PRIx64" (%"PRIu64") bytes\n",           p, w, "align",  p_align, p_align);
    if (p_extra.size()>0) {
        fprintf(f, "%s%-*s = %zu bytes\n", p, w, "extra", p_extra.size());
        hexdump(f, 0, std::string(p)+"extra at ", p_extra);
    }
}

std::string
SgAsmElfSegmentTableEntry::to_string(SegmentType kind) 
{
#ifndef _MSC_VER
    return stringifySgAsmElfSegmentTableEntrySegmentType(kind);
#else
	ROSE_ASSERT(false);
	return "";
#endif
}

std::string
SgAsmElfSegmentTableEntry::to_string(SegmentFlags val)
{
    std::string str;
    for( size_t i=0; (1u << i) <= PF_RPERM; ++i){
        if( i!= 0)
            str += ' ';
        if(val & (1 << i)){
            switch(1 << i){
                case PF_XPERM: str += "EXECUTE";break;
                case PF_WPERM: str += "WRITE"; break;
                case PF_RPERM: str += "READ";break;
            };
        }
    }
    uint32_t os   = (val & ~(uint32_t)(PF_OS_MASK));
    uint32_t proc = (val & ~(uint32_t)(PF_PROC_MASK));
    uint32_t rest = (val & ~(uint32_t)(PF_RESERVED));

    if(os){
        char buf[64];
        snprintf(buf,sizeof(buf),"os flags(%2x)", os >> 20);
        str += buf;    
    }

    if(proc){
        char buf[64];
        snprintf(buf,sizeof(buf),"proc flags(%1x)", proc >> 28);
        str += buf;    
    }

    if(rest){
        char buf[64];
        snprintf(buf,sizeof(buf),"unknown(%x)", rest);
        str += buf;
    }

    return str;

}


/** Non-parsing constructor for an ELF Segment (Program Header) Table */
void
SgAsmElfSegmentTable::ctor()
{
    /* There can be only one ELF Segment Table */
    SgAsmElfFileHeader *fhdr = dynamic_cast<SgAsmElfFileHeader*>(get_header());
    ROSE_ASSERT(fhdr);
    ROSE_ASSERT(fhdr->get_segment_table()==NULL);
    fhdr->set_segment_table(this);
    
    set_synthesized(true);                              /* the segment table isn't part of any explicit section */
    set_name(new SgAsmBasicString("ELF Segment Table"));
    set_purpose(SP_HEADER);

    fhdr->set_segment_table(this);
}

/** Parses an ELF Segment (Program Header) Table and constructs and parses all segments reachable from the table. The section
 *  is extended as necessary based on the number of entries and teh size of each entry. */
SgAsmElfSegmentTable *
SgAsmElfSegmentTable::parse()
{
    SgAsmGenericSection::parse();

    SgAsmElfFileHeader *fhdr = dynamic_cast<SgAsmElfFileHeader*>(get_header());
    ROSE_ASSERT(fhdr!=NULL);
    ByteOrder sex = fhdr->get_sex();

    size_t ent_size, struct_size, opt_size, nentries;
    calculate_sizes(&ent_size, &struct_size, &opt_size, &nentries);
    ROSE_ASSERT(opt_size==fhdr->get_phextrasz() && nentries==fhdr->get_e_phnum());

    /* If the current size is very small (0 or 1 byte) then we're coming straight from the constructor and the parsing should
     * also extend this section to hold all the entries. Otherwise the caller must have assigned a specific size for a good
     * reason and we should leave that alone, reading zeros if the entries extend beyond the defined size. */
    if (get_size()<=1 && get_size()<nentries*ent_size)
        extend(nentries*ent_size - get_size());
    
    rose_addr_t offset=0;                                /* w.r.t. the beginning of this section */
    for (size_t i=0; i<nentries; i++, offset+=ent_size) {
        /* Read/decode the segment header */
        SgAsmElfSegmentTableEntry *shdr = NULL;
        if (4==fhdr->get_word_size()) {
            SgAsmElfSegmentTableEntry::Elf32SegmentTableEntry_disk disk;
            read_content_local(offset, &disk, struct_size);
            shdr = new SgAsmElfSegmentTableEntry(sex, &disk);
        } else {
            SgAsmElfSegmentTableEntry::Elf64SegmentTableEntry_disk disk;
            read_content_local(offset, &disk, struct_size);
            shdr = new SgAsmElfSegmentTableEntry(sex, &disk);
        }
        shdr->set_index(i);
        if (opt_size>0)
            shdr->get_extra() = read_content_local_ucl(offset+struct_size, opt_size);

        /* Null segments are just unused slots in the table; no real section to create */
        if (SgAsmElfSegmentTableEntry::PT_NULL == shdr->get_type())
            continue;

        /* Create SgAsmElfSection objects for each ELF Segment. However, if the ELF Segment Table describes a segment
         * that's the same offset and size as a section from the Elf Section Table (and the memory mappings are
         * consistent) then use the preexisting section instead of creating a new one. */
        SgAsmElfSection *s = NULL;
        SgAsmGenericSectionPtrList possible = fhdr->get_file()->get_sections_by_offset(shdr->get_offset(), shdr->get_filesz());
        for (size_t j=0; !s && j<possible.size(); j++) {
            if (possible[j]->get_offset()!=shdr->get_offset() || possible[j]->get_size()!=shdr->get_filesz())
                continue; /*different file extent*/
            if (possible[j]->is_mapped()) {
                if (possible[j]->get_mapped_preferred_rva()!=shdr->get_vaddr() ||
                    possible[j]->get_mapped_size()!=shdr->get_memsz())
                    continue; /*different mapped address or size*/
                unsigned section_perms = (possible[j]->get_mapped_rperm() ? 0x01 : 0x00) |
                                         (possible[j]->get_mapped_wperm() ? 0x02 : 0x00) |
                                         (possible[j]->get_mapped_xperm() ? 0x04 : 0x00);
                unsigned segment_perms = (shdr->get_flags() & SgAsmElfSegmentTableEntry::PF_RPERM ? 0x01 : 0x00) |
                                         (shdr->get_flags() & SgAsmElfSegmentTableEntry::PF_WPERM ? 0x02 : 0x00) |
                                         (shdr->get_flags() & SgAsmElfSegmentTableEntry::PF_XPERM ? 0x04 : 0x00);
                if (section_perms != segment_perms)
                    continue; /*different mapped permissions*/
            }

            /* Found a match. Set memory mapping params only. */
            s = dynamic_cast<SgAsmElfSection*>(possible[j]);
            if (!s) continue; /*potential match was not from the ELF Section or Segment table*/
            if (s->get_segment_entry()) continue; /*potential match is assigned to some other segment table entry*/
            s->init_from_segment_table(shdr, true); /*true=>set memory mapping params only*/
        }

        /* Create a new segment if no matching section was found. */
        if (!s) {
            if (SgAsmElfSegmentTableEntry::PT_NOTE == shdr->get_type()) {
                s = new SgAsmElfNoteSection(fhdr);
            } else {
                s = new SgAsmElfSection(fhdr);
            }
            s->init_from_segment_table(shdr);
            s->parse();
        }
    }
    return this;
}

/** Attaches a previously unattached ELF Segment (SgAsmElfSection) to the ELF Segment Table (SgAsmElfSegmentTable). This
 *  method complements SgAsmElfSection::init_from_segment_table. This method initializes the segment table from the segment
 *  while init_from_segment_table() initializes the segment from the segment table.
 *  
 *  ELF Segments are represented by SgAsmElfSection objects since ELF Segments and ELF Sections overlap very much in their
 *  features and thus should share an interface. An SgAsmElfSection can appear in the ELF Section Table and/or the ELF Segment
 *  Table and you can determine where it was located by calling get_section_entry() and get_segment_entry().
 *
 *  Returns the new segment table entry linked into the AST. */
SgAsmElfSegmentTableEntry *
SgAsmElfSegmentTable::add_section(SgAsmElfSection *section)
{
    ROSE_ASSERT(section!=NULL);
    ROSE_ASSERT(section->get_file()==get_file());
    ROSE_ASSERT(section->get_header()==get_header());
    ROSE_ASSERT(section->get_segment_entry()==NULL);            /* must not be in the segment table yet */

    SgAsmElfFileHeader *fhdr = dynamic_cast<SgAsmElfFileHeader*>(get_header());
    ROSE_ASSERT(fhdr);
    
    /* Assign a slot in the segment table */
    int idx = fhdr->get_e_phnum();
    fhdr->set_e_phnum(idx+1);

    /* Create a new segment table entry */
    SgAsmElfSegmentTableEntry *shdr = new SgAsmElfSegmentTableEntry;
    shdr->set_index(idx);
    shdr->update_from_section(section);
    section->set_segment_entry(shdr);

    return shdr;
}

/** Returns info about the size of the entries based on information already available. Any or all arguments may be null
 *  pointers if the caller is not interested in the value. */
rose_addr_t
SgAsmElfSegmentTable::calculate_sizes(size_t *entsize, size_t *required, size_t *optional, size_t *entcount) const
{
    SgAsmElfFileHeader *fhdr = dynamic_cast<SgAsmElfFileHeader*>(get_header());
    ROSE_ASSERT(fhdr!=NULL);

    size_t struct_size = 0;
    size_t extra_size = fhdr->get_phextrasz();
    size_t entry_size = 0;
    size_t nentries = 0;

    /* Size of required part of each entry */
    if (4==fhdr->get_word_size()) {
        struct_size = sizeof(SgAsmElfSegmentTableEntry::Elf32SegmentTableEntry_disk);
    } else if (8==fhdr->get_word_size()) {
        struct_size = sizeof(SgAsmElfSegmentTableEntry::Elf64SegmentTableEntry_disk);
    } else {
        throw FormatError("bad ELF word size");
    }

    /* Entire entry should be at least large enough for the required part. */
    entry_size = struct_size;

    /* Size of optional parts. If we've parsed the table then use the largest optional part, otherwise assume the entry from
     * the ELF File Header is correct. */
    SgAsmGenericSectionPtrList sections = fhdr->get_sections()->get_sections();
    for (size_t i=0; i<sections.size(); i++) {
        SgAsmElfSection *elfsec = dynamic_cast<SgAsmElfSection*>(sections[i]);
        if (elfsec && elfsec->get_segment_entry()) {
            nentries++;
            extra_size = std::max(extra_size, elfsec->get_segment_entry()->get_extra().size());
        }
    }

    /* Total number of entries. Either we haven't parsed the segment table yet (nor created the segments it defines) or we
     * have. In the former case we use the setting from the ELF File Header, otherwise we just count the number of segments
     * that have associated segment table entry pointers. */
    if (0==nentries)
        nentries = fhdr->get_e_phnum();

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

/** Pre-unparsing updates */
bool
SgAsmElfSegmentTable::reallocate()
{
    bool reallocated = false;

    /* Resize based on word size from ELF File Header */
    size_t opt_size, nentries;
    rose_addr_t need = calculate_sizes(NULL, NULL, &opt_size, &nentries);
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

    /* Update data members in the ELF File Header. No need to return true for these changes. */
    SgAsmElfFileHeader *fhdr = dynamic_cast<SgAsmElfFileHeader*>(get_header());
    fhdr->set_phextrasz(opt_size);
    fhdr->set_e_phnum(nentries);

    return reallocated;
}

/** Write the segment table to disk. */
void
SgAsmElfSegmentTable::unparse(std::ostream &f) const
{
    SgAsmElfFileHeader *fhdr = dynamic_cast<SgAsmElfFileHeader*>(get_header());
    ROSE_ASSERT(fhdr!=NULL);
    ByteOrder sex = fhdr->get_sex();
    SgAsmGenericSectionPtrList sections = fhdr->get_segtab_sections();

    /* Write the segments first */
    for (size_t i=0; i<sections.size(); i++)
        sections[i]->unparse(f);
    unparse_holes(f);

    /* Calculate sizes. The ELF File Header should have been updated in reallocate() prior to unparsing. */
    size_t ent_size, struct_size, opt_size, nentries;
    calculate_sizes(&ent_size, &struct_size, &opt_size, &nentries);
    ROSE_ASSERT(fhdr->get_phextrasz()==opt_size);
    ROSE_ASSERT(fhdr->get_e_phnum()==nentries);
    
    /* Write the segment table entries */
    for (size_t i=0; i < sections.size(); ++i) {
        SgAsmElfSection *section = dynamic_cast<SgAsmElfSection*>(sections[i]);
        ROSE_ASSERT(section!=NULL);
        SgAsmElfSegmentTableEntry *shdr = section->get_segment_entry();
        ROSE_ASSERT(shdr!=NULL);
        ROSE_ASSERT(shdr->get_offset()==section->get_offset()); /*segment table entry should have been updated in reallocate()*/

        int id = shdr->get_index();
        ROSE_ASSERT(id>=0 && (size_t)id<nentries);
            
        SgAsmElfSegmentTableEntry::Elf32SegmentTableEntry_disk disk32;
        SgAsmElfSegmentTableEntry::Elf64SegmentTableEntry_disk disk64;
        void *disk = NULL;
        
        if (4==fhdr->get_word_size()) {
            disk = shdr->encode(sex, &disk32);
        } else if (8==fhdr->get_word_size()) {
            disk = shdr->encode(sex, &disk64);
        } else {
            ROSE_ASSERT(!"invalid word size");
        }
        
        /* The disk struct */
        rose_addr_t spos = write(f, id*ent_size, struct_size, disk);
        if (shdr->get_extra().size() > 0)
            write(f, spos, shdr->get_extra());
    }
}

/** Print some debugging info */
void
SgAsmElfSegmentTable::dump(FILE *f, const char *prefix, ssize_t idx) const
{
    char p[4096];
    if (idx>=0) {
        sprintf(p, "%sSegmentTable[%zd].", prefix, idx);
    } else {
        sprintf(p, "%sSegmentTable.", prefix);
    }

    SgAsmGenericSection::dump(f, p, -1);

    if (variantT() == V_SgAsmElfSegmentTable) //unless a base class
        hexdump(f, 0, std::string(p)+"data at ", p_data);
}
