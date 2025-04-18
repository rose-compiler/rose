/* ELF Segment Tables (SgAsmElfSegmentTable and related classes) */
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include "sage3basic.h"

#include <Rose/BinaryAnalysis/ByteOrder.h>
#include <Rose/BinaryAnalysis/Hexdump.h>

#include "stringify.h"

// In order to efficiently (in terms of amount of code) parse a file format that's defined for a different architecture, we
// need to occassionally take addresses of structs that don't follow alignment rules for this architecture.
#if defined(__GNUC__) && __GNUC__ >= 9
#pragma GCC diagnostic ignored "-Waddress-of-packed-member"
#endif

using namespace Rose;

/* Converts 32-bit disk representation to host representation */
SgAsmElfSegmentTableEntry::SgAsmElfSegmentTableEntry(Rose::BinaryAnalysis::ByteOrder::Endianness sex,
                                                     const SgAsmElfSegmentTableEntry::Elf32SegmentTableEntry_disk *disk) {
    initializeProperties();
    ASSERT_not_null(disk);
    set_index (0);
    set_type  ((SegmentType)Rose::BinaryAnalysis::ByteOrder::diskToHost(sex, disk->p_type));
    set_offset(Rose::BinaryAnalysis::ByteOrder::diskToHost(sex, disk->p_offset));
    set_vaddr (Rose::BinaryAnalysis::ByteOrder::diskToHost(sex, disk->p_vaddr));
    set_paddr (Rose::BinaryAnalysis::ByteOrder::diskToHost(sex, disk->p_paddr));
    set_filesz(Rose::BinaryAnalysis::ByteOrder::diskToHost(sex, disk->p_filesz));
    set_memsz (Rose::BinaryAnalysis::ByteOrder::diskToHost(sex, disk->p_memsz));
    set_flags ((SegmentFlags)Rose::BinaryAnalysis::ByteOrder::diskToHost(sex, disk->p_flags));
    set_align (Rose::BinaryAnalysis::ByteOrder::diskToHost(sex, disk->p_align));
}

SgAsmElfSegmentTableEntry::SgAsmElfSegmentTableEntry(Rose::BinaryAnalysis::ByteOrder::Endianness sex,
                                                     const SgAsmElfSegmentTableEntry::Elf64SegmentTableEntry_disk *disk) {
    initializeProperties();
    ASSERT_not_null(disk);
    set_type  ((SegmentType)Rose::BinaryAnalysis::ByteOrder::diskToHost(sex, disk->p_type));
    set_offset(Rose::BinaryAnalysis::ByteOrder::diskToHost(sex, disk->p_offset));
    set_vaddr (Rose::BinaryAnalysis::ByteOrder::diskToHost(sex, disk->p_vaddr));
    set_paddr (Rose::BinaryAnalysis::ByteOrder::diskToHost(sex, disk->p_paddr));
    set_filesz(Rose::BinaryAnalysis::ByteOrder::diskToHost(sex, disk->p_filesz));
    set_memsz (Rose::BinaryAnalysis::ByteOrder::diskToHost(sex, disk->p_memsz));
    set_flags ((SegmentFlags)Rose::BinaryAnalysis::ByteOrder::diskToHost(sex, disk->p_flags));
    set_align (Rose::BinaryAnalysis::ByteOrder::diskToHost(sex, disk->p_align));
}

void *
SgAsmElfSegmentTableEntry::encode(Rose::BinaryAnalysis::ByteOrder::Endianness sex, Elf32SegmentTableEntry_disk *disk) const
{
    Rose::BinaryAnalysis::ByteOrder::hostToDisk(sex, (uint32_t)p_type, &(disk->p_type));
    Rose::BinaryAnalysis::ByteOrder::hostToDisk(sex, p_offset, &(disk->p_offset));
    Rose::BinaryAnalysis::ByteOrder::hostToDisk(sex, p_vaddr, &(disk->p_vaddr));
    Rose::BinaryAnalysis::ByteOrder::hostToDisk(sex, p_paddr, &(disk->p_paddr));
    Rose::BinaryAnalysis::ByteOrder::hostToDisk(sex, p_filesz, &(disk->p_filesz));
    Rose::BinaryAnalysis::ByteOrder::hostToDisk(sex, p_memsz, &(disk->p_memsz));
    Rose::BinaryAnalysis::ByteOrder::hostToDisk(sex, (uint32_t)p_flags, &(disk->p_flags));
    Rose::BinaryAnalysis::ByteOrder::hostToDisk(sex, p_align, &(disk->p_align));
    return disk;
}
void *
SgAsmElfSegmentTableEntry::encode(Rose::BinaryAnalysis::ByteOrder::Endianness sex, Elf64SegmentTableEntry_disk *disk) const
{
    Rose::BinaryAnalysis::ByteOrder::hostToDisk(sex, (uint32_t)p_type, &(disk->p_type));
    Rose::BinaryAnalysis::ByteOrder::hostToDisk(sex, p_offset, &(disk->p_offset));
    Rose::BinaryAnalysis::ByteOrder::hostToDisk(sex, p_vaddr, &(disk->p_vaddr));
    Rose::BinaryAnalysis::ByteOrder::hostToDisk(sex, p_paddr, &(disk->p_paddr));
    Rose::BinaryAnalysis::ByteOrder::hostToDisk(sex, p_filesz, &(disk->p_filesz));
    Rose::BinaryAnalysis::ByteOrder::hostToDisk(sex, p_memsz, &(disk->p_memsz));
    Rose::BinaryAnalysis::ByteOrder::hostToDisk(sex, (uint32_t)p_flags, &(disk->p_flags));
    Rose::BinaryAnalysis::ByteOrder::hostToDisk(sex, p_align, &(disk->p_align));
    return disk;
}

void
SgAsmElfSegmentTableEntry::update_from_section(SgAsmElfSection *section)
{
    updateFromSection(section);
}

void
SgAsmElfSegmentTableEntry::updateFromSection(SgAsmElfSection *section)
{
    set_offset(section->get_offset());
    set_filesz(section->get_size());
    set_vaddr(section->get_mappedPreferredVa());
    set_memsz(section->get_mappedSize());
    set_align(section->isMapped() ? section->get_mappedAlignment() : section->get_fileAlignment());

    if (section->get_mappedReadPermission()) {
        set_flags((SegmentFlags)(p_flags | PF_RPERM));
    } else {
        set_flags((SegmentFlags)(p_flags & ~PF_RPERM));
    }
    if (section->get_mappedWritePermission()) {
        set_flags((SegmentFlags)(p_flags | PF_WPERM));
    } else {
        set_flags((SegmentFlags)(p_flags & ~PF_WPERM));
    }
    if (section->get_mappedExecutePermission()) {
        set_flags((SegmentFlags)(p_flags | PF_XPERM));
    } else {
        set_flags((SegmentFlags)(p_flags & ~PF_XPERM));
    }

    if (isSgAsmElfNoteSection(section)) {
        set_type(PT_NOTE);
    }
}

void
SgAsmElfSegmentTableEntry::dump(FILE *f, const char *prefix, ssize_t idx) const
{
    char p[4096];
    if (idx>=0) {
        snprintf(p, sizeof(p), "%sElfSegmentTableEntry[%zd].", prefix, idx);
    } else {
        snprintf(p, sizeof(p), "%sElfSegmentTableEntry.", prefix);
    }
    const int w = std::max(size_t{1}, DUMP_FIELD_WIDTH - strlen(p));

    fprintf(f, "%s%-*s = %" PRIuPTR "\n",                             p, w, "index",  p_index);
    fprintf(f, "%s%-*s = 0x%08x = %s\n",                     p, w, "type",   p_type,  toString(p_type).c_str());
    fprintf(f, "%s%-*s = 0x%08x ",                           p, w, "flags",  p_flags);
    fputc(p_flags & PF_RPERM ? 'r' : '-', f);
    fputc(p_flags & PF_WPERM ? 'w' : '-', f);
    fputc(p_flags & PF_XPERM ? 'x' : '-', f);
    if (p_flags & PF_OS_MASK) fputs(" os", f);
    if (p_flags & PF_PROC_MASK) fputs(" proc", f);
    if (p_flags & PF_RESERVED) fputs(" *", f);
    fputc('\n', f);
    fprintf(f, "%s%-*s = 0x%08" PRIx64 " (%" PRIu64 ") bytes into file\n", p, w, "offset", p_offset, p_offset);
    fprintf(f, "%s%-*s = 0x%08" PRIx64 " (%" PRIu64 ")\n",                 p, w, "vaddr",  p_vaddr, p_vaddr);
    fprintf(f, "%s%-*s = 0x%08" PRIx64 " (%" PRIu64 ")\n",                 p, w, "paddr",  p_paddr, p_paddr);
    fprintf(f, "%s%-*s = 0x%08" PRIx64 " (%" PRIu64 ") bytes\n",           p, w, "filesz", p_filesz, p_filesz);
    fprintf(f, "%s%-*s = 0x%08" PRIx64 " (%" PRIu64 ") bytes\n",           p, w, "memsz",  p_memsz, p_memsz);
    fprintf(f, "%s%-*s = 0x%08" PRIx64 " (%" PRIu64 ") bytes\n",           p, w, "align",  p_align, p_align);
    if (p_extra.size()>0) {
        fprintf(f, "%s%-*s = %" PRIuPTR " bytes\n", p, w, "extra", p_extra.size());
        Rose::BinaryAnalysis::hexdump(f, 0, std::string(p)+"extra at ", p_extra);
    }
}

std::string
SgAsmElfSegmentTableEntry::to_string(SegmentType kind)
{
    return toString(kind);
}

std::string
SgAsmElfSegmentTableEntry::toString(SegmentType kind)
{
#ifndef _MSC_VER
    return stringifySgAsmElfSegmentTableEntrySegmentType(kind);
#else
        ROSE_ABORT();
#endif
}

std::string
SgAsmElfSegmentTableEntry::to_string(SegmentFlags val)
{
    return toString(val);
}

std::string
SgAsmElfSegmentTableEntry::toString(SegmentFlags val)
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

/* Non-parsing constructor for an ELF Segment (Program Header) Table */
SgAsmElfSegmentTable::SgAsmElfSegmentTable(SgAsmElfFileHeader *fhdr)
    : SgAsmGenericSection(fhdr->get_file(), fhdr) {
    initializeProperties();

    /* There can be only one ELF Segment Table */
    ASSERT_require(fhdr->get_segmentTable() == nullptr);
    fhdr->set_segmentTable(this);
    
    set_synthesized(true);                              /* the segment table isn't part of any explicit section */
    set_name(new SgAsmBasicString("ELF Segment Table"));
    set_purpose(SP_HEADER);

    fhdr->set_segmentTable(this);
}

SgAsmElfSegmentTable *
SgAsmElfSegmentTable::parse()
{
    SgAsmGenericSection::parse();

    SgAsmElfFileHeader *fhdr = dynamic_cast<SgAsmElfFileHeader*>(get_header());
    ROSE_ASSERT(fhdr!=NULL);
    Rose::BinaryAnalysis::ByteOrder::Endianness sex = fhdr->get_sex();

    size_t ent_size, struct_size, opt_size, nentries;
    calculateSizes(&ent_size, &struct_size, &opt_size, &nentries);
    ROSE_ASSERT(opt_size==fhdr->get_phextrasz() && nentries==fhdr->get_e_phnum());

    /* If the current size is very small (0 or 1 byte) then we're coming straight from the constructor and the parsing should
     * also extend this section to hold all the entries. Otherwise the caller must have assigned a specific size for a good
     * reason and we should leave that alone, reading zeros if the entries extend beyond the defined size. */
    if (get_size()<=1 && get_size()<nentries*ent_size)
        extend(nentries*ent_size - get_size());
    
    Rose::BinaryAnalysis::Address offset=0;                                /* w.r.t. the beginning of this section */
    for (size_t i=0; i<nentries; i++, offset+=ent_size) {
        /* Read/decode the segment header */
        SgAsmElfSegmentTableEntry *shdr = NULL;
        if (4==fhdr->get_wordSize()) {
            SgAsmElfSegmentTableEntry::Elf32SegmentTableEntry_disk disk;
            readContentLocal(offset, &disk, struct_size);
            shdr = new SgAsmElfSegmentTableEntry(sex, &disk);
        } else {
            SgAsmElfSegmentTableEntry::Elf64SegmentTableEntry_disk disk;
            readContentLocal(offset, &disk, struct_size);
            shdr = new SgAsmElfSegmentTableEntry(sex, &disk);
        }
        shdr->set_index(i);
        if (opt_size>0)
            shdr->get_extra() = readContentLocalUcl(offset+struct_size, opt_size);

        /* Null segments are just unused slots in the table; no real section to create */
        if (SgAsmElfSegmentTableEntry::PT_NULL == shdr->get_type())
            continue;

        /* Create SgAsmElfSection objects for each ELF Segment. However, if the ELF Segment Table describes a segment
         * that's the same offset and size as a section from the Elf Section Table (and the memory mappings are
         * consistent) then use the preexisting section instead of creating a new one. */
        SgAsmElfSection *s = NULL;
        SgAsmGenericSectionPtrList possible = fhdr->get_file()->get_sectionsByOffset(shdr->get_offset(), shdr->get_filesz());
        for (size_t j=0; !s && j<possible.size(); j++) {
            if (possible[j]->get_offset()!=shdr->get_offset() || possible[j]->get_size()!=shdr->get_filesz())
                continue; /*different file extent*/
            if (possible[j]->isMapped()) {
                if (possible[j]->get_mappedPreferredRva()!=shdr->get_vaddr() ||
                    possible[j]->get_mappedSize()!=shdr->get_memsz())
                    continue; /*different mapped address or size*/
                unsigned section_perms = (possible[j]->get_mappedReadPermission() ? 0x01 : 0x00) |
                                         (possible[j]->get_mappedWritePermission() ? 0x02 : 0x00) |
                                         (possible[j]->get_mappedExecutePermission() ? 0x04 : 0x00);
                unsigned segment_perms = (shdr->get_flags() & SgAsmElfSegmentTableEntry::PF_RPERM ? 0x01 : 0x00) |
                                         (shdr->get_flags() & SgAsmElfSegmentTableEntry::PF_WPERM ? 0x02 : 0x00) |
                                         (shdr->get_flags() & SgAsmElfSegmentTableEntry::PF_XPERM ? 0x04 : 0x00);
                if (section_perms != segment_perms)
                    continue; /*different mapped permissions*/
            }

            /* Found a match. Set memory mapping params only. */
            s = dynamic_cast<SgAsmElfSection*>(possible[j]);
            if (!s) continue; /*potential match was not from the ELF Section or Segment table*/
            if (s->get_segmentEntry()) continue; /*potential match is assigned to some other segment table entry*/
            s->initFromSegmentTable(shdr, true); /*true=>set memory mapping params only*/
        }

        /* Create a new segment if no matching section was found. */
        if (!s) {
            if (SgAsmElfSegmentTableEntry::PT_NOTE == shdr->get_type()) {
                s = new SgAsmElfNoteSection(fhdr);
            } else {
                s = new SgAsmElfSection(fhdr);
            }
            s->initFromSegmentTable(shdr);
            s->parse();
        }
    }
    return this;
}

SgAsmElfSegmentTableEntry *
SgAsmElfSegmentTable::add_section(SgAsmElfSection *section)
{
    return addSection(section);
}

SgAsmElfSegmentTableEntry *
SgAsmElfSegmentTable::addSection(SgAsmElfSection *section)
{
    ROSE_ASSERT(section!=NULL);
    ROSE_ASSERT(section->get_file()==get_file());
    ROSE_ASSERT(section->get_header()==get_header());
    ROSE_ASSERT(section->get_segmentEntry()==NULL);            /* must not be in the segment table yet */

    SgAsmElfFileHeader *fhdr = dynamic_cast<SgAsmElfFileHeader*>(get_header());
    ROSE_ASSERT(fhdr);
    
    /* Assign a slot in the segment table */
    int idx = fhdr->get_e_phnum();
    fhdr->set_e_phnum(idx+1);

    /* Create a new segment table entry */
    SgAsmElfSegmentTableEntry *shdr = new SgAsmElfSegmentTableEntry;
    shdr->set_index(idx);
    shdr->updateFromSection(section);
    section->set_segmentEntry(shdr);

    return shdr;
}

Rose::BinaryAnalysis::Address
SgAsmElfSegmentTable::calculate_sizes(size_t *entsize, size_t *required, size_t *optional, size_t *entcount) const
{
    return calculateSizes(entsize, required, optional, entcount);
}

Rose::BinaryAnalysis::Address
SgAsmElfSegmentTable::calculateSizes(size_t *entsize, size_t *required, size_t *optional, size_t *entcount) const
{
    SgAsmElfFileHeader *fhdr = dynamic_cast<SgAsmElfFileHeader*>(get_header());
    ROSE_ASSERT(fhdr!=NULL);

    size_t struct_size = 0;
    size_t extra_size = fhdr->get_phextrasz();
    size_t entry_size = 0;
    size_t nentries = 0;

    /* Size of required part of each entry */
    if (4==fhdr->get_wordSize()) {
        struct_size = sizeof(SgAsmElfSegmentTableEntry::Elf32SegmentTableEntry_disk);
    } else if (8==fhdr->get_wordSize()) {
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
        if (elfsec && elfsec->get_segmentEntry()) {
            nentries++;
            extra_size = std::max(extra_size, elfsec->get_segmentEntry()->get_extra().size());
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

bool
SgAsmElfSegmentTable::reallocate()
{
    bool reallocated = false;

    /* Resize based on word size from ELF File Header */
    size_t opt_size, nentries;
    Rose::BinaryAnalysis::Address need = calculateSizes(NULL, NULL, &opt_size, &nentries);
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

    /* Update data members in the ELF File Header. No need to return true for these changes. */
    SgAsmElfFileHeader *fhdr = dynamic_cast<SgAsmElfFileHeader*>(get_header());
    fhdr->set_phextrasz(opt_size);
    fhdr->set_e_phnum(nentries);

    return reallocated;
}

void
SgAsmElfSegmentTable::unparse(std::ostream &f) const
{
    SgAsmElfFileHeader *fhdr = dynamic_cast<SgAsmElfFileHeader*>(get_header());
    ROSE_ASSERT(fhdr!=NULL);
    Rose::BinaryAnalysis::ByteOrder::Endianness sex = fhdr->get_sex();
    SgAsmGenericSectionPtrList sections = fhdr->get_segmentTableSections();

    /* Write the segments first */
    for (size_t i=0; i<sections.size(); i++)
        sections[i]->unparse(f);
    unparseHoles(f);

    /* Calculate sizes. The ELF File Header should have been updated in reallocate() prior to unparsing. */
    size_t ent_size, struct_size, opt_size, nentries;
    calculateSizes(&ent_size, &struct_size, &opt_size, &nentries);
    ROSE_ASSERT(fhdr->get_phextrasz() == opt_size);
    ROSE_ASSERT(fhdr->get_e_phnum() >= nentries);
    ROSE_ASSERT(nentries == sections.size());

    // What entries were written by the following loop
    std::vector<bool> wasWritten(fhdr->get_e_phnum(), false);

    /* Write the segment table entries */
    for (size_t i=0; i < sections.size(); ++i) {
        SgAsmElfSection *section = dynamic_cast<SgAsmElfSection*>(sections[i]);
        ROSE_ASSERT(section!=NULL);
        SgAsmElfSegmentTableEntry *shdr = section->get_segmentEntry();
        ROSE_ASSERT(shdr!=NULL);
        ROSE_ASSERT(shdr->get_offset()==section->get_offset()); /*segment table entry should have been updated in reallocate()*/

        int id = shdr->get_index();
        ROSE_ASSERT(id >= 0 && (size_t)id < wasWritten.size());

        SgAsmElfSegmentTableEntry::Elf32SegmentTableEntry_disk disk32;
        SgAsmElfSegmentTableEntry::Elf64SegmentTableEntry_disk disk64;
        void *disk = NULL;
        
        if (4==fhdr->get_wordSize()) {
            disk = shdr->encode(sex, &disk32);
        } else if (8==fhdr->get_wordSize()) {
            disk = shdr->encode(sex, &disk64);
        } else {
            ROSE_ASSERT(!"invalid word size");
        }
        
        // Write the disk struct and extra data to the table slot
        Rose::BinaryAnalysis::Address spos = write(f, id*ent_size, struct_size, disk);
        if (shdr->get_extra().size() > 0)
            write(f, spos, shdr->get_extra());
        wasWritten[id] = true;
    }

    // Write zeroed entries to the unused slots of the segment table
    for (size_t i = 0; i < fhdr->get_e_phnum(); ++i) {
        if (!wasWritten[i]) {
            std::vector<uint8_t> empty(ent_size, 0);
            write(f, i*ent_size, ent_size, empty.data());
        }
    }
}

void
SgAsmElfSegmentTable::dump(FILE *f, const char *prefix, ssize_t idx) const
{
    char p[4096];
    if (idx>=0) {
        snprintf(p, sizeof(p), "%sSegmentTable[%zd].", prefix, idx);
    } else {
        snprintf(p, sizeof(p), "%sSegmentTable.", prefix);
    }

    SgAsmGenericSection::dump(f, p, -1);

    if (variantT() == V_SgAsmElfSegmentTable) //unless a base class
        Rose::BinaryAnalysis::hexdump(f, 0, std::string(p)+"data at ", p_data);
}

#endif
