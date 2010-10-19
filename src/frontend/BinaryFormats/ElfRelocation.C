/* ELF Relocations (SgAsmElfRelocSection and related classes) */
#include "sage3basic.h"
#include "stringify.h"
#define __STDC_FORMAT_MACROS
#include <inttypes.h>

/** Constructor adds the new entry to the relocation table. */
void
SgAsmElfRelocEntry::ctor(SgAsmElfRelocSection *section)
{
    ROSE_ASSERT(section->get_entries()!=NULL);
    section->get_entries()->get_entries().push_back(this);
    ROSE_ASSERT(section->get_entries()->get_entries().size()>0);
    set_parent(section->get_entries());
}

/* Parsers */
void
SgAsmElfRelocEntry::parse(ByteOrder sex, const Elf32RelaEntry_disk *disk)
{
    p_r_offset    = disk_to_host(sex, disk->r_offset);
    p_r_addend    = disk_to_host(sex, disk->r_addend);
    uint64_t info = disk_to_host(sex, disk->r_info);
    p_sym = info >> 8;
    p_type = (RelocType)(info & 0xff);
}
void
SgAsmElfRelocEntry::parse(ByteOrder sex, const Elf64RelaEntry_disk *disk)
{
    p_r_offset    = disk_to_host(sex, disk->r_offset);
    p_r_addend    = disk_to_host(sex, disk->r_addend);
    uint64_t info = disk_to_host(sex, disk->r_info);
    p_sym = info >> 32;
    p_type = (RelocType)(info & 0xffffffff);
}
void
SgAsmElfRelocEntry::parse(ByteOrder sex, const Elf32RelEntry_disk *disk)
{
    p_r_offset    = disk_to_host(sex, disk->r_offset);
    p_r_addend    = 0;
    uint64_t info = disk_to_host(sex, disk->r_info);
    p_sym = info >> 8;
    p_type = (RelocType)(info & 0xff);
}
void
SgAsmElfRelocEntry::parse(ByteOrder sex, const Elf64RelEntry_disk *disk)
{
    p_r_offset    = disk_to_host(sex, disk->r_offset);
    p_r_addend    = 0;
    uint64_t info = disk_to_host(sex, disk->r_info);
    p_sym = info >> 32;
    p_type = (RelocType)(info & 0xffffffff);
}

/* Encode a native entry back into disk format */
void *
SgAsmElfRelocEntry::encode(ByteOrder sex, Elf32RelaEntry_disk *disk) const
{
    host_to_disk(sex, p_r_offset, &(disk->r_offset));
    host_to_disk(sex, p_r_addend, &(disk->r_addend));
    uint64_t info = (p_sym<<8) | (p_type & 0xff);
    host_to_disk(sex, info, &(disk->r_info));
    return disk;
}
void *
SgAsmElfRelocEntry::encode(ByteOrder sex, Elf64RelaEntry_disk *disk) const
{
    host_to_disk(sex, p_r_offset, &(disk->r_offset));
    host_to_disk(sex, p_r_addend, &(disk->r_addend));
    uint64_t info = ((uint64_t)p_sym<<32) | (p_type & 0xffffffff);
    host_to_disk(sex, info, &(disk->r_info));
    return disk;
}
void *
SgAsmElfRelocEntry::encode(ByteOrder sex, Elf32RelEntry_disk *disk) const
{
    host_to_disk(sex, p_r_offset, &(disk->r_offset));
    ROSE_ASSERT(0==p_r_addend);
    uint64_t info = (p_sym<<8) | (p_type & 0xff);
    host_to_disk(sex, info, &(disk->r_info));
    return disk;
}
void *
SgAsmElfRelocEntry::encode(ByteOrder sex, Elf64RelEntry_disk *disk) const
{
    host_to_disk(sex, p_r_offset, &(disk->r_offset));
    ROSE_ASSERT(0==p_r_addend);
    uint64_t info = ((uint64_t)p_sym<<32) | (p_type & 0xffffffff);
    host_to_disk(sex, info, &(disk->r_info));
    return disk;
}

/* Change symbol to string. TODO, needs to handle multiple architectures [MCB] */
std::string
SgAsmElfRelocEntry::reloc_name() const
{
    return stringifySgAsmElfRelocEntryRelocType(get_type());
}

/** Print some debugging info */
void
SgAsmElfRelocEntry::dump(FILE *f, const char *prefix, ssize_t idx, SgAsmElfSymbolSection *symtab) const
{
    char p[4096];
    if (idx>=0) {
        sprintf(p, "%sElfRelocEntry[%zd]", prefix, idx);
    } else {
        sprintf(p, "%sElfRelocEntry", prefix);
    }
    const int w = std::max(1, DUMP_FIELD_WIDTH-(int)strlen(p));

    /* compact one-line-per-reloc format */
    if (0==idx) {
        fprintf(f, "%s%-*s   %-10s %-20s %-10s %4s %-10s %s\n",
                p, w, "", "Offset", "Type", "Addend", "Sym", "Value", "Name + Addend");
    }

    /* Offset */
    fprintf(f, "%s%-*s = 0x%08"PRIx64, p, w, "", p_r_offset);

    /* Type */
    SgAsmGenericHeader* header = SageInterface::getEnclosingNode<SgAsmGenericHeader>(this);
    if (header) {
        fprintf(f, " %-20s", reloc_name().c_str());
    } else {
        fprintf(f, " 0x%02zx                ", (size_t)p_type);
    }

    /* Addend */
    if (p_r_addend) {
        fprintf(f, " 0x%08"PRIx64, p_r_addend);
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
        fprintf(f, " 0x%08"PRIx64" %s", sym->get_value(), sym->get_name()->c_str());
    }

    /* Addend in decimal */
    if (p_r_addend)
        fprintf(f, " + %"PRIu64, p_r_addend);
    fputc('\n', f);

    /* Auxiliary data */
    if (p_extra.size()>0) {
        fprintf(f, "%s%-*s = %zu bytes\n", p, w, ".extra", p_extra.size());
        hexdump(f, 0, std::string(p)+"extra at ", p_extra);
    }
}

/** Non-parsing constructor */
void
SgAsmElfRelocSection::ctor(SgAsmElfSymbolSection *symbols, SgAsmElfSection *targetsec/*=NULL*/)
{
    p_entries = new SgAsmElfRelocEntryList;
    p_entries->set_parent(this);
    ROSE_ASSERT(symbols!=NULL);
    p_linked_section = symbols;
    p_target_section = targetsec;
}

/** Parse an existing ELF Rela Section */
SgAsmElfRelocSection *
SgAsmElfRelocSection::parse()
{
    SgAsmElfSection::parse();

    SgAsmElfFileHeader *fhdr = get_elf_header();
    ROSE_ASSERT(fhdr);

    size_t entry_size, struct_size, extra_size, nentries;
    calculate_sizes(&entry_size, &struct_size, &extra_size, &nentries);
    ROSE_ASSERT(extra_size==0);
    
    /* Parse each entry */
    for (size_t i=0; i<nentries; i++) {
        SgAsmElfRelocEntry *entry = 0;
        if (4==fhdr->get_word_size()) {
            if (p_uses_addend) {
                SgAsmElfRelocEntry::Elf32RelaEntry_disk disk;
                read_content_local(i*entry_size, &disk, struct_size);
                entry = new SgAsmElfRelocEntry(this);
                entry->parse(fhdr->get_sex(), &disk);
            } else {
                SgAsmElfRelocEntry::Elf32RelEntry_disk disk;
                read_content_local(i*entry_size, &disk, struct_size);
                entry = new SgAsmElfRelocEntry(this);
                entry->parse(fhdr->get_sex(), &disk);
            }
        } else if (8==fhdr->get_word_size()) {
            if (p_uses_addend) {
                SgAsmElfRelocEntry::Elf64RelaEntry_disk disk;
                read_content_local(i*entry_size, &disk, struct_size);
                entry = new SgAsmElfRelocEntry(this);
                entry->parse(fhdr->get_sex(), &disk);
            } else {
                SgAsmElfRelocEntry::Elf64RelEntry_disk disk;
                read_content_local(i*entry_size, &disk, struct_size);
                entry = new SgAsmElfRelocEntry(this);
                entry->parse(fhdr->get_sex(), &disk);
            }
        } else {
            throw FormatError("unsupported ELF word size");
        }
        if (extra_size>0)
            entry->get_extra() = read_content_local_ucl(i*entry_size+struct_size, extra_size);
    }
    return this;
}

/** Return sizes for various parts of the table. See doc for SgAsmElfSection::calculate_sizes. */
rose_addr_t
SgAsmElfRelocSection::calculate_sizes(size_t *entsize, size_t *required, size_t *optional, size_t *entcount) const
{
    rose_addr_t retval=0;
    std::vector<size_t> extra_sizes;
    for (size_t i=0; i<p_entries->get_entries().size(); i++)
        extra_sizes.push_back(p_entries->get_entries()[i]->get_extra().size());
    if (p_uses_addend) {
        retval =  calculate_sizes(sizeof(SgAsmElfRelocEntry::Elf32RelaEntry_disk),
                                  sizeof(SgAsmElfRelocEntry::Elf64RelaEntry_disk),
                                  extra_sizes, entsize, required, optional, entcount);
    } else {
        retval =  calculate_sizes(sizeof(SgAsmElfRelocEntry::Elf32RelEntry_disk),
                                  sizeof(SgAsmElfRelocEntry::Elf64RelEntry_disk),
                                  extra_sizes, entsize, required, optional, entcount);
    }
    return retval;
}

/** Pre-unparsing adjustments */
bool
SgAsmElfRelocSection::reallocate()
{
    bool reallocated = SgAsmElfSection::reallocate();
    
    /* Update parts of the section and segment tables not updated by superclass */
    SgAsmElfSectionTableEntry *secent = get_section_entry();
    if (secent)
        secent->set_sh_type(p_uses_addend ?
                            SgAsmElfSectionTableEntry::SHT_RELA :
                            SgAsmElfSectionTableEntry::SHT_REL);

    return reallocated;
}

/** Write section back to disk */
void
SgAsmElfRelocSection::unparse(std::ostream &f) const
{
    SgAsmElfFileHeader *fhdr = get_elf_header();
    ROSE_ASSERT(fhdr);
    ByteOrder sex = fhdr->get_sex();

    size_t entry_size, struct_size, extra_size, nentries;
    calculate_sizes(&entry_size, &struct_size, &extra_size, &nentries);

    /* Adjust the entry size stored in the ELF Section Table */
    get_section_entry()->set_sh_entsize(entry_size);

    /* Write each entry's required part followed by the optional part */
    for (size_t i=0; i<nentries; i++) {
        SgAsmElfRelocEntry::Elf32RelaEntry_disk diska32;
        SgAsmElfRelocEntry::Elf64RelaEntry_disk diska64;
        SgAsmElfRelocEntry::Elf32RelEntry_disk  disk32;
        SgAsmElfRelocEntry::Elf64RelEntry_disk  disk64;
        void *disk  = NULL;

        SgAsmElfRelocEntry *entry = p_entries->get_entries()[i];

        if (4==fhdr->get_word_size()) {
            if (p_uses_addend) {
                disk = entry->encode(sex, &diska32);
            } else {
                disk = entry->encode(sex, &disk32);
            }
        } else if (8==fhdr->get_word_size()) {
            if (p_uses_addend) {
                disk = entry->encode(sex, &diska64);
            } else {
                disk = entry->encode(sex, &disk64);
            }
        } else {
            ROSE_ASSERT(!"unsupported word size");
        }

        addr_t spos = i * entry_size;
        spos = write(f, spos, struct_size, disk);
#if 0 /*FIXME: padding not supported here yet (RPM 2008-10-13)*/
        if (entry->get_extra().size()>0) {
            ROSE_ASSERT(entry->get_extra().size()<=extra_size);
            write(f, spos, entry->get_extra());
        }
#endif
    }

    unparse_holes(f);
}

/** Print some debugging info */
void
SgAsmElfRelocSection::dump(FILE *f, const char *prefix, ssize_t idx) const
{
    char p[4096];
    if (idx>=0) {
        sprintf(p, "%sRelocSection[%zd].", prefix, idx);
    } else {
        sprintf(p, "%sRelocSection.", prefix);
    }
    const int w = std::max(1, DUMP_FIELD_WIDTH-(int)strlen(p));

    SgAsmElfSection::dump(f, p, -1);
    SgAsmElfSymbolSection *symtab = dynamic_cast<SgAsmElfSymbolSection*>(get_linked_section());
    fprintf(f, "%s%-*s = %s\n", p, w, "uses_addend", p_uses_addend ? "yes" : "no");

    if (p_target_section) {
        fprintf(f, "%s%-*s = [%d] \"%s\"\n", p, w, "target_section",
                p_target_section->get_id(), p_target_section->get_name()->c_str());
    } else {
        fprintf(f, "%s%-*s = NULL\n", p, w, "target_section");
    }

    for (size_t i=0; i<p_entries->get_entries().size(); i++) {
        SgAsmElfRelocEntry *ent = p_entries->get_entries()[i];
        ent->dump(f, p, i, symtab);
    }

    if (variantT() == V_SgAsmElfRelocSection) //unless a base class
        hexdump(f, 0, std::string(p)+"data at ", p_data);
}
