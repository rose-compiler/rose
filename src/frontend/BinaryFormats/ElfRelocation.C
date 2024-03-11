/* ELF Relocations (SgAsmElfRelocSection and related classes) */
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include "sage3basic.h"

#include <Rose/BinaryAnalysis/Hexdump.h>
#include "stringify.h"

// In order to efficiently (in terms of amount of code) parse a file format that's defined for a different architecture, we
// need to occassionally take addresses of structs that don't follow alignment rules for this architecture.
#if defined(__GNUC__) && __GNUC__ >= 9
#pragma GCC diagnostic ignored "-Waddress-of-packed-member"
#endif

using namespace Rose;

SgAsmElfRelocEntry::SgAsmElfRelocEntry(SgAsmElfRelocSection *section) {
    initializeProperties();
    ASSERT_not_null(section);
    ASSERT_not_null(section->get_entries());
    section->get_entries()->get_entries().push_back(this);
    ASSERT_require(section->get_entries()->get_entries().size() > 0);
    set_parent(section->get_entries());
}

void
SgAsmElfRelocEntry::parse(Rose::BinaryAnalysis::ByteOrder::Endianness sex, const Elf32RelaEntry_disk *disk)
{
    p_r_offset    = Rose::BinaryAnalysis::ByteOrder::diskToHost(sex, disk->r_offset);
    p_r_addend    = Rose::BinaryAnalysis::ByteOrder::diskToHost(sex, disk->r_addend);
    uint32_t info = Rose::BinaryAnalysis::ByteOrder::diskToHost(sex, disk->r_info);
    p_sym = info >> 8;
    p_type = (RelocType)(info & 0xff);
}
void
SgAsmElfRelocEntry::parse(Rose::BinaryAnalysis::ByteOrder::Endianness sex, const Elf64RelaEntry_disk *disk)
{
    p_r_offset    = Rose::BinaryAnalysis::ByteOrder::diskToHost(sex, disk->r_offset);
    p_r_addend    = Rose::BinaryAnalysis::ByteOrder::diskToHost(sex, disk->r_addend);
    uint64_t info = Rose::BinaryAnalysis::ByteOrder::diskToHost(sex, disk->r_info);
    p_sym = info >> 32;
    p_type = (RelocType)(info & 0xffffffff);
}
void
SgAsmElfRelocEntry::parse(Rose::BinaryAnalysis::ByteOrder::Endianness sex, const Elf32RelEntry_disk *disk)
{
    p_r_offset    = Rose::BinaryAnalysis::ByteOrder::diskToHost(sex, disk->r_offset);
    p_r_addend    = 0;
    uint32_t info = Rose::BinaryAnalysis::ByteOrder::diskToHost(sex, disk->r_info);
    p_sym = info >> 8;
    p_type = (RelocType)(info & 0xff);
}
void
SgAsmElfRelocEntry::parse(Rose::BinaryAnalysis::ByteOrder::Endianness sex, const Elf64RelEntry_disk *disk)
{
    p_r_offset    = Rose::BinaryAnalysis::ByteOrder::diskToHost(sex, disk->r_offset);
    p_r_addend    = 0;
    uint64_t info = Rose::BinaryAnalysis::ByteOrder::diskToHost(sex, disk->r_info);
    p_sym = info >> 32;
    p_type = (RelocType)(info & 0xffffffff);
}

void *
SgAsmElfRelocEntry::encode(Rose::BinaryAnalysis::ByteOrder::Endianness sex, Elf32RelaEntry_disk *disk) const
{
    hostToDisk(sex, p_r_offset, &(disk->r_offset));
    hostToDisk(sex, p_r_addend, &(disk->r_addend));
    uint32_t info = (p_sym<<8) | (p_type & 0xff);
    hostToDisk(sex, info, &(disk->r_info));
    return disk;
}
void *
SgAsmElfRelocEntry::encode(Rose::BinaryAnalysis::ByteOrder::Endianness sex, Elf64RelaEntry_disk *disk) const
{
    hostToDisk(sex, p_r_offset, &(disk->r_offset));
    hostToDisk(sex, p_r_addend, &(disk->r_addend));
    uint64_t info = ((uint64_t)p_sym<<32) | (p_type & 0xffffffff);
    hostToDisk(sex, info, &(disk->r_info));
    return disk;
}
void *
SgAsmElfRelocEntry::encode(Rose::BinaryAnalysis::ByteOrder::Endianness sex, Elf32RelEntry_disk *disk) const
{
    hostToDisk(sex, p_r_offset, &(disk->r_offset));
    ROSE_ASSERT(0==p_r_addend);
    uint64_t info = (p_sym<<8) | (p_type & 0xff);
    hostToDisk(sex, info, &(disk->r_info));
    return disk;
}
void *
SgAsmElfRelocEntry::encode(Rose::BinaryAnalysis::ByteOrder::Endianness sex, Elf64RelEntry_disk *disk) const
{
    hostToDisk(sex, p_r_offset, &(disk->r_offset));
    ROSE_ASSERT(0==p_r_addend);
    uint64_t info = ((uint64_t)p_sym<<32) | (p_type & 0xffffffff);
    hostToDisk(sex, info, &(disk->r_info));
    return disk;
}

std::string
SgAsmElfRelocEntry::reloc_name() const {
    return toString();
}

/* TODO, needs to handle multiple architectures [MCB] */
std::string
SgAsmElfRelocEntry::toString() const
{
#ifndef _MSC_VER
        return stringifySgAsmElfRelocEntryRelocType(get_type());
#else
        ROSE_ABORT();
#endif
}

void
SgAsmElfRelocEntry::dump(FILE *f, const char *prefix, ssize_t idx, SgAsmElfSymbolSection *symtab) const
{
    char p[4096];
    if (idx>=0) {
        snprintf(p, sizeof(p), "%sElfRelocEntry[%zd]", prefix, idx);
    } else {
        snprintf(p, sizeof(p), "%sElfRelocEntry", prefix);
    }
    const int w = std::max(size_t{1}, DUMP_FIELD_WIDTH - strlen(p));

    /* compact one-line-per-reloc format */
    if (0==idx) {
        fprintf(f, "%s%-*s   %-10s %-20s %-10s %4s %-10s %s\n",
                p, w, "", "Offset", "Type", "Addend", "Sym", "Value", "Name + Addend");
    }

    /* Offset */
    fprintf(f, "%s%-*s = 0x%08" PRIx64, p, w, "", p_r_offset);

    /* Type */
    SgAsmGenericHeader* header = SageInterface::getEnclosingNode<SgAsmGenericHeader>(this);
    if (header) {
        fprintf(f, " %-20s", toString().c_str());
    } else {
        fprintf(f, " 0x%02zx                ", (size_t)p_type);
    }

    /* Addend */
    if (p_r_addend) {
        fprintf(f, " 0x%08" PRIx64, p_r_addend);
    } else {
        fprintf(f, " %10s", "");
    }

    /* Symbol index */
    fprintf(f, " %4lu", p_sym);

    /* Symbol value and name */
    if (!symtab) {
        fprintf(f, " %10s <no symtab>", "");
    } else if (p_sym>=symtab->get_symbols()->get_symbols().size()) {
        fprintf(f, " %10s <out of range>", "");
    } else {
        SgAsmGenericSymbol *sym = symtab->get_symbols()->get_symbols()[p_sym];
        fprintf(f, " 0x%08" PRIx64 " %s", sym->get_value(), sym->get_name()->get_string(true).c_str());
    }

    /* Addend in decimal */
    if (p_r_addend)
        fprintf(f, " + %" PRIu64, p_r_addend);
    fputc('\n', f);

    /* Auxiliary data */
    if (p_extra.size()>0) {
        fprintf(f, "%s%-*s = %" PRIuPTR " bytes\n", p, w, ".extra", p_extra.size());
        Rose::BinaryAnalysis::hexdump(f, 0, std::string(p)+"extra at ", p_extra);
    }
}

SgAsmElfRelocSection::SgAsmElfRelocSection(SgAsmElfFileHeader *fhdr, SgAsmElfSymbolSection *symsec,SgAsmElfSection* targetsec)
    : SgAsmElfSection(fhdr) {
    initializeProperties();
    set_linkedSection(symsec);                        // may be null
    set_targetSection(targetsec);
}

SgAsmElfRelocSection *
SgAsmElfRelocSection::parse()
{
    SgAsmElfSection::parse();

    SgAsmElfFileHeader *fhdr = get_elfHeader();
    ROSE_ASSERT(fhdr);

    size_t entry_size, struct_size, extra_size, nentries;
    calculateSizes(&entry_size, &struct_size, &extra_size, &nentries);
    ROSE_ASSERT(extra_size==0);
    
    /* Parse each entry */
    for (size_t i=0; i<nentries; i++) {
        SgAsmElfRelocEntry *entry = 0;
        if (4==fhdr->get_wordSize()) {
            if (get_usesAddend()) {
                SgAsmElfRelocEntry::Elf32RelaEntry_disk disk;
                readContentLocal(i*entry_size, &disk, struct_size);
                entry = new SgAsmElfRelocEntry(this);
                entry->parse(fhdr->get_sex(), &disk);
            } else {
                SgAsmElfRelocEntry::Elf32RelEntry_disk disk;
                readContentLocal(i*entry_size, &disk, struct_size);
                entry = new SgAsmElfRelocEntry(this);
                entry->parse(fhdr->get_sex(), &disk);
            }
        } else if (8==fhdr->get_wordSize()) {
            if (get_usesAddend()) {
                SgAsmElfRelocEntry::Elf64RelaEntry_disk disk;
                readContentLocal(i*entry_size, &disk, struct_size);
                entry = new SgAsmElfRelocEntry(this);
                entry->parse(fhdr->get_sex(), &disk);
            } else {
                SgAsmElfRelocEntry::Elf64RelEntry_disk disk;
                readContentLocal(i*entry_size, &disk, struct_size);
                entry = new SgAsmElfRelocEntry(this);
                entry->parse(fhdr->get_sex(), &disk);
            }
        } else {
            throw FormatError("unsupported ELF word size");
        }
        if (extra_size>0)
            entry->get_extra() = readContentLocalUcl(i*entry_size+struct_size, extra_size);
    }
    return this;
}

rose_addr_t
SgAsmElfRelocSection::calculate_sizes(size_t *entsize, size_t *required, size_t *optional, size_t *entcount) const {
    return calculateSizes(entsize, required, optional, entcount);
}

rose_addr_t
SgAsmElfRelocSection::calculateSizes(size_t *entsize, size_t *required, size_t *optional, size_t *entcount) const
{
    rose_addr_t retval=0;
    std::vector<size_t> extra_sizes;
    for (size_t i=0; i<p_entries->get_entries().size(); i++)
        extra_sizes.push_back(p_entries->get_entries()[i]->get_extra().size());
    if (get_usesAddend()) {
        retval =  calculateSizes(sizeof(SgAsmElfRelocEntry::Elf32RelaEntry_disk),
                                  sizeof(SgAsmElfRelocEntry::Elf64RelaEntry_disk),
                                  extra_sizes, entsize, required, optional, entcount);
    } else {
        retval =  calculateSizes(sizeof(SgAsmElfRelocEntry::Elf32RelEntry_disk),
                                  sizeof(SgAsmElfRelocEntry::Elf64RelEntry_disk),
                                  extra_sizes, entsize, required, optional, entcount);
    }
    return retval;
}

bool
SgAsmElfRelocSection::reallocate()
{
    bool reallocated = SgAsmElfSection::reallocate();
    
    /* Update parts of the section and segment tables not updated by superclass */
    SgAsmElfSectionTableEntry *secent = get_sectionEntry();
    if (secent)
        secent->set_sh_type(get_usesAddend() ?
                            SgAsmElfSectionTableEntry::SHT_RELA :
                            SgAsmElfSectionTableEntry::SHT_REL);

    return reallocated;
}

void
SgAsmElfRelocSection::unparse(std::ostream &f) const
{
    SgAsmElfFileHeader *fhdr = get_elfHeader();
    ROSE_ASSERT(fhdr);
    Rose::BinaryAnalysis::ByteOrder::Endianness sex = fhdr->get_sex();

    size_t entry_size, struct_size, extra_size, nentries;
    calculateSizes(&entry_size, &struct_size, &extra_size, &nentries);

    /* Adjust the entry size stored in the ELF Section Table */
    get_sectionEntry()->set_sh_entsize(entry_size);

    /* Write each entry's required part followed by the optional part */
    for (size_t i=0; i<nentries; i++) {
        SgAsmElfRelocEntry::Elf32RelaEntry_disk diska32;
        SgAsmElfRelocEntry::Elf64RelaEntry_disk diska64;
        SgAsmElfRelocEntry::Elf32RelEntry_disk  disk32;
        SgAsmElfRelocEntry::Elf64RelEntry_disk  disk64;
        void *disk  = NULL;

        SgAsmElfRelocEntry *entry = p_entries->get_entries()[i];

        if (4==fhdr->get_wordSize()) {
            if (get_usesAddend()) {
                disk = entry->encode(sex, &diska32);
            } else {
                disk = entry->encode(sex, &disk32);
            }
        } else if (8==fhdr->get_wordSize()) {
            if (get_usesAddend()) {
                disk = entry->encode(sex, &diska64);
            } else {
                disk = entry->encode(sex, &disk64);
            }
        } else {
            ROSE_ASSERT(!"unsupported word size");
        }

        rose_addr_t spos = i * entry_size;
        spos = write(f, spos, struct_size, disk);
#if 0 /*FIXME: padding not supported here yet (RPM 2008-10-13)*/
        if (entry->get_extra().size()>0) {
            ROSE_ASSERT(entry->get_extra().size()<=extra_size);
            write(f, spos, entry->get_extra());
        }
#endif
    }

    unparseHoles(f);
}

void
SgAsmElfRelocSection::dump(FILE *f, const char *prefix, ssize_t idx) const
{
    char p[4096];
    if (idx>=0) {
        snprintf(p, sizeof(p), "%sRelocSection[%zd].", prefix, idx);
    } else {
        snprintf(p, sizeof(p), "%sRelocSection.", prefix);
    }
    const int w = std::max(size_t{1}, DUMP_FIELD_WIDTH - strlen(p));

    SgAsmElfSection::dump(f, p, -1);
    SgAsmElfSymbolSection *symtab = dynamic_cast<SgAsmElfSymbolSection*>(get_linkedSection());
    fprintf(f, "%s%-*s = %s\n", p, w, "uses_addend", get_usesAddend() ? "yes" : "no");

    if (get_targetSection()) {
        fprintf(f, "%s%-*s = [%d] \"%s\"\n", p, w, "target_section",
                get_targetSection()->get_id(), get_targetSection()->get_name()->get_string(true).c_str());
    } else {
        fprintf(f, "%s%-*s = NULL\n", p, w, "target_section");
    }

    for (size_t i=0; i<p_entries->get_entries().size(); i++) {
        SgAsmElfRelocEntry *ent = p_entries->get_entries()[i];
        ent->dump(f, p, i, symtab);
    }

    if (variantT() == V_SgAsmElfRelocSection) //unless a base class
        Rose::BinaryAnalysis::hexdump(f, 0, std::string(p)+"data at ", p_data);
}

bool
SgAsmElfRelocSection::get_uses_addend() const {
    return get_usesAddend();
}

void
SgAsmElfRelocSection::set_uses_addend(bool x) {
    set_usesAddend(x);
}

SgAsmElfSection*
SgAsmElfRelocSection::get_target_section() const {
    return get_targetSection();
}

void
SgAsmElfRelocSection::set_target_section(SgAsmElfSection *x) {
    set_targetSection(x);
}

#endif
