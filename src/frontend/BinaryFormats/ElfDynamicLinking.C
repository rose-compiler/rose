/* ELF Dynamic Linking (SgAsmElfDynamicSection and related classes) */
#include <rosePublicConfig.h>
#ifdef ROSE_BUILD_BINARY_ANALYSIS_SUPPORT
#include "sage3basic.h"

#include "stringify.h"

using namespace Rose;

void
SgAsmElfDynamicEntry::ctor(SgAsmElfDynamicSection *dynsec)
{
    ROSE_ASSERT(dynsec!=NULL);

    set_name(NULL); /*only defined for DT_NEEDED entries; see SgAsmDynamicSection::parse*/

    ROSE_ASSERT(dynsec->get_entries()!=NULL);
    dynsec->get_entries()->get_entries().push_back(this);
    ROSE_ASSERT(dynsec->get_entries()->get_entries().size()>0);
    set_parent(dynsec->get_entries());
}

void
SgAsmElfDynamicEntry::parse(ByteOrder::Endianness sex, const Elf32DynamicEntry_disk *disk)
{
    p_d_tag = (EntryType)ByteOrder::disk_to_host(sex, disk->d_tag);
    p_d_val = ByteOrder::disk_to_host(sex, disk->d_val);
}
void
SgAsmElfDynamicEntry::parse(ByteOrder::Endianness sex, const Elf64DynamicEntry_disk *disk)
{
    p_d_tag = (EntryType)ByteOrder::disk_to_host(sex, disk->d_tag);
    p_d_val = ByteOrder::disk_to_host(sex, disk->d_val);
}

void *
SgAsmElfDynamicEntry::encode(ByteOrder::Endianness sex, Elf32DynamicEntry_disk *disk) const
{
    ByteOrder::host_to_disk(sex, p_d_tag, &(disk->d_tag));
    ByteOrder::host_to_disk(sex, p_d_val.get_rva(), &(disk->d_val));
    return disk;
}
void *
SgAsmElfDynamicEntry::encode(ByteOrder::Endianness sex, Elf64DynamicEntry_disk *disk) const
{
    ByteOrder::host_to_disk(sex, p_d_tag, &(disk->d_tag));
    ByteOrder::host_to_disk(sex, p_d_val.get_rva(), &(disk->d_val));
    return disk;
}

std::string
SgAsmElfDynamicEntry::to_string(EntryType t) 
{
#ifndef _MSC_VER
    return stringifySgAsmElfDynamicEntryEntryType(t);
#else
        ROSE_ASSERT(false);
        return "";
#endif

}

void
SgAsmElfDynamicEntry::set_name(SgAsmGenericString *name)
{
    if (name!=p_name) {
        if (p_name) {
            p_name->set_parent(NULL);
            SageInterface::deleteAST(p_name);
        }
        p_name = name;
        if (p_name)
            p_name->set_parent(this);
        set_isModified(true);
    }
}

void
SgAsmElfDynamicEntry::dump(FILE *f, const char *prefix, ssize_t idx) const
{
    char p[4096];
    if (idx>=0) {
        sprintf(p, "%sElfDynamicEntry[%zd].", prefix, idx);
    } else {
        sprintf(p, "%sElfDynamicEntry.", prefix);
    }
    const int w = std::max(1, DUMP_FIELD_WIDTH-(int)strlen(p));

    char label[256];
    strcpy(label, to_string(p_d_tag).c_str());
    for (char *s=label; *s; s++) *s = tolower(*s);

    fprintf(f, "%s%-*s = %s", p, w, label, p_d_val.to_string().c_str());
    if (p_name)
        fprintf(f, " \"%s\"", p_name->get_string(true).c_str());
    fputc('\n', f);

    if (p_extra.size()>0) {
        fprintf(f, "%s%-*s = %" PRIuPTR " bytes\n", p, w, "extra", p_extra.size());
        hexdump(f, 0, std::string(p)+"extra at ", p_extra);
    }
}

void
SgAsmElfDynamicSection::ctor(SgAsmElfStringSection *strings)
{
    p_entries = new SgAsmElfDynamicEntryList;
    p_entries->set_parent(this);
    ROSE_ASSERT(strings!=NULL);
    p_linked_section = strings;
}

SgAsmElfDynamicSection *
SgAsmElfDynamicSection::parse()
{
    SgAsmElfSection::parse();

    SgAsmElfFileHeader *fhdr = get_elf_header();
    ROSE_ASSERT(fhdr);
    SgAsmElfSectionTableEntry *shdr = get_section_entry();
    ROSE_ASSERT(shdr);
    SgAsmElfStringSection *strsec = dynamic_cast<SgAsmElfStringSection*>(get_linked_section());
    ROSE_ASSERT(strsec!=NULL);

    size_t entry_size, struct_size, extra_size, nentries;
    calculate_sizes(&entry_size, &struct_size, &extra_size, &nentries);
    ROSE_ASSERT(entry_size==shdr->get_sh_entsize());

    /* Parse each entry */
    for (size_t i=0; i<nentries; i++) {
        SgAsmElfDynamicEntry *entry=0;
        if (4==fhdr->get_word_size()) {
            entry = new SgAsmElfDynamicEntry(this);
            SgAsmElfDynamicEntry::Elf32DynamicEntry_disk disk;
            read_content_local(i*entry_size, &disk, struct_size);
            entry->parse(fhdr->get_sex(), &disk);
        } else if (8==fhdr->get_word_size()) {
            entry = new SgAsmElfDynamicEntry(this);
            SgAsmElfDynamicEntry::Elf64DynamicEntry_disk disk;
            read_content_local(i*entry_size, &disk, struct_size);
            entry->parse(fhdr->get_sex(), &disk);
        } else {
            throw FormatError("unsupported ELF word size");
        }
        if (extra_size>0)
            entry->get_extra() = read_content_local_ucl(i*entry_size+struct_size, extra_size);

        /* Set name */
        if (entry->get_d_tag()==SgAsmElfDynamicEntry::DT_NEEDED) {
            ROSE_ASSERT(entry->get_name()==NULL);
            SgAsmStoredString *name = new SgAsmStoredString(strsec->get_strtab(), entry->get_d_val().get_rva());
            entry->set_name(name);
            name->set_parent(entry);
#if 1       /* FIXME: Do we really want this stuff duplicated in the AST? [RPM 2008-12-12] */
            SgAsmStoredString *name2 = new SgAsmStoredString(strsec->get_strtab(), entry->get_d_val().get_rva());
            fhdr->add_dll(new SgAsmGenericDLL(name2));
#endif
        }
    }
    return this;
}

rose_addr_t
SgAsmElfDynamicSection::calculate_sizes(size_t *entsize, size_t *required, size_t *optional, size_t *entcount) const
{
    std::vector<size_t> extra_sizes;
    for (size_t i=0; i<p_entries->get_entries().size(); i++)
        extra_sizes.push_back(p_entries->get_entries()[i]->get_extra().size());
    return calculate_sizes(sizeof(SgAsmElfDynamicEntry::Elf32DynamicEntry_disk),
                           sizeof(SgAsmElfDynamicEntry::Elf64DynamicEntry_disk),
                           extra_sizes,
                           entsize, required, optional, entcount);
}
    
void
SgAsmElfDynamicSection::finish_parsing() 
{
    SgAsmElfFileHeader *fhdr = get_elf_header();
    ROSE_ASSERT(fhdr!=NULL);

    for (size_t i=0; i<p_entries->get_entries().size(); i++) {
        SgAsmElfDynamicEntry *entry = p_entries->get_entries()[i];
        switch (entry->get_d_tag()) {
          case SgAsmElfDynamicEntry::DT_NEEDED:
            break;
          case SgAsmElfDynamicEntry::DT_PLTGOT:
          case SgAsmElfDynamicEntry::DT_HASH:
          case SgAsmElfDynamicEntry::DT_STRTAB:
          case SgAsmElfDynamicEntry::DT_SYMTAB:
          case SgAsmElfDynamicEntry::DT_RELA:
          case SgAsmElfDynamicEntry::DT_INIT:
          case SgAsmElfDynamicEntry::DT_FINI:
          case SgAsmElfDynamicEntry::DT_REL:
          case SgAsmElfDynamicEntry::DT_DEBUG:
          case SgAsmElfDynamicEntry::DT_JMPREL:
          case SgAsmElfDynamicEntry::DT_INIT_ARRAY:
          case SgAsmElfDynamicEntry::DT_FINI_ARRAY:
          case SgAsmElfDynamicEntry::DT_PREINIT_ARRAY:
          case SgAsmElfDynamicEntry::DT_GNU_HASH:
          case SgAsmElfDynamicEntry::DT_TLSDESC_PLT:
          case SgAsmElfDynamicEntry::DT_TLSDESC_GOT:
          case SgAsmElfDynamicEntry::DT_GNU_CONFLICT:
          case SgAsmElfDynamicEntry::DT_GNU_LIBLIST:
          case SgAsmElfDynamicEntry::DT_CONFIG:
          case SgAsmElfDynamicEntry::DT_DEPAUDIT:
          case SgAsmElfDynamicEntry::DT_AUDIT:
          case SgAsmElfDynamicEntry::DT_PLTPAD:
          case SgAsmElfDynamicEntry::DT_MOVETAB:
          case SgAsmElfDynamicEntry::DT_SYMINFO:
          case SgAsmElfDynamicEntry::DT_VERSYM:
          case SgAsmElfDynamicEntry::DT_VERDEF:
          case SgAsmElfDynamicEntry::DT_VERNEED:
          case SgAsmElfDynamicEntry::DT_AUXILIARY:
          case SgAsmElfDynamicEntry::DT_FILTER: {
              /* d_val is relative to a section. We know that all ELF Sections (but perhaps not the ELF Segments) have been
               * created by this time. */
              ROSE_ASSERT(entry->get_d_val().get_section()==NULL);
              SgAsmGenericSectionPtrList containers = fhdr->get_sections_by_rva(entry->get_d_val().get_rva());
              SgAsmGenericSection *best = NULL;
              for (SgAsmGenericSectionPtrList::iterator i=containers.begin(); i!=containers.end(); ++i) {
                  if ((*i)->is_mapped()) {
                      if ((*i)->get_mapped_preferred_rva()==entry->get_d_val().get_rva()) {
                          best = *i;
                          break;
                      } else if (!best) {
                          best = *i;
                      } else if ((*i)->get_mapped_size() < best->get_mapped_size()) {
                          best = *i;
                      }
                  }
              }
              if (best)
                  entry->set_d_val(rose_rva_t(entry->get_d_val().get_rva(), best));
              break;
          }
          default:
            break;
        }
    }
}

bool
SgAsmElfDynamicSection::reallocate()
{
    bool reallocated = SgAsmElfSection::reallocate();

    /* Update parts of the section and segment tables not updated by superclass */
    SgAsmElfSectionTableEntry *secent = get_section_entry();
    if (secent)
        secent->set_sh_type(SgAsmElfSectionTableEntry::SHT_DYNAMIC);
    SgAsmElfSegmentTableEntry *segent = get_segment_entry();
    if (segent)
        segent->set_type(SgAsmElfSegmentTableEntry::PT_DYNAMIC);

    /* Update entries with name offsets. The name should point to the string table to which the dynamic section links. */
    for (size_t i=0; i<p_entries->get_entries().size(); i++) {
        SgAsmElfDynamicEntry *entry = p_entries->get_entries()[i];
        if (entry->get_name()) {
            SgAsmElfStringSection *strsec = dynamic_cast<SgAsmElfStringSection*>(get_linked_section());
            ROSE_ASSERT(strsec);
            SgAsmStoredString *stored_string = dynamic_cast<SgAsmStoredString*>(entry->get_name());
            if (!stored_string || stored_string->get_strtab()!=strsec->get_strtab()) {
                /* Not a stored string, or stored in the wrong string table. */
                stored_string = new SgAsmStoredString(strsec->get_strtab(), entry->get_name()->get_string());
                entry->get_name()->set_string(""); /*free old storage*/
                entry->set_name(stored_string);
            }
            entry->set_d_val(entry->get_name()->get_offset());
        }
    }
    
    return reallocated;
}

void
SgAsmElfDynamicSection::unparse(std::ostream &f) const
{
    SgAsmElfFileHeader *fhdr = get_elf_header();
    ROSE_ASSERT(fhdr);
    ByteOrder::Endianness sex = fhdr->get_sex();

    size_t entry_size, struct_size, extra_size, nentries;
    calculate_sizes(&entry_size, &struct_size, &extra_size, &nentries);

    /* Adjust the entry size stored in the ELF Section Table */
    get_section_entry()->set_sh_entsize(entry_size);

    /* Write each entry's required part followed by the optional part */
    for (size_t i=0; i<nentries; i++) {
        SgAsmElfDynamicEntry::Elf32DynamicEntry_disk disk32;
        SgAsmElfDynamicEntry::Elf64DynamicEntry_disk disk64;
        void *disk  = NULL;

        SgAsmElfDynamicEntry *entry = p_entries->get_entries()[i];

        if (4==fhdr->get_word_size()) {
            disk = entry->encode(sex, &disk32);
        } else if (8==fhdr->get_word_size()) {
            disk = entry->encode(sex, &disk64);
        } else {
            ROSE_ASSERT(!"unsupported word size");
        }

        rose_addr_t spos = i * entry_size;
        spos = write(f, spos, struct_size, disk);
        if (entry->get_extra().size()>0) {
            ROSE_ASSERT(entry->get_extra().size()<=extra_size);
            write(f, spos, entry->get_extra());
        }
    }

    unparse_holes(f);
}

void
SgAsmElfDynamicSection::dump(FILE *f, const char *prefix, ssize_t idx) const
{
    char p[4096];
    if (idx>=0) {
        sprintf(p, "%sDynamicSection[%zd].", prefix, idx);
    } else {
        sprintf(p, "%sDynamicSection.", prefix);
    }

    SgAsmElfSection::dump(f, p, -1);

    for (size_t i=0; i<p_entries->get_entries().size(); i++) {
        SgAsmElfDynamicEntry *ent = p_entries->get_entries()[i];
        ent->dump(f, p, i);
        dump_containing_sections(f, std::string(p)+"...", ent->get_d_val(), get_header()->get_sections()->get_sections());
    }

    if (variantT() == V_SgAsmElfDynamicSection) //unless a base class
        hexdump(f, 0, std::string(p)+"data at ", p_data);
}

#endif
