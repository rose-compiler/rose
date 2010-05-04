/* ELF Dynamic Linking (SgAsmElfDynamicSection and related classes) */
// tps (01/14/2010) : Switching from rose.h to sage3.
#include "sage3basic.h"
#define __STDC_FORMAT_MACROS
#include <inttypes.h>

/** Adds the newly constructed entry to the dynamic section. */
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

/** Initialize a dynamic section entry by parsing something stored in the file. */
void
SgAsmElfDynamicEntry::parse(ByteOrder sex, const Elf32DynamicEntry_disk *disk)
{
    p_d_tag = (EntryType)disk_to_host(sex, disk->d_tag);
    p_d_val = disk_to_host(sex, disk->d_val);
}
void
SgAsmElfDynamicEntry::parse(ByteOrder sex, const Elf64DynamicEntry_disk *disk)
{
    p_d_tag = (EntryType)disk_to_host(sex, disk->d_tag);
    p_d_val = disk_to_host(sex, disk->d_val);
}

/** Encode a native entry back into disk format */
void *
SgAsmElfDynamicEntry::encode(ByteOrder sex, Elf32DynamicEntry_disk *disk) const
{
    host_to_disk(sex, p_d_tag, &(disk->d_tag));
    host_to_disk(sex, p_d_val.get_rva(), &(disk->d_val));
    return disk;
}
void *
SgAsmElfDynamicEntry::encode(ByteOrder sex, Elf64DynamicEntry_disk *disk) const
{
    host_to_disk(sex, p_d_tag, &(disk->d_tag));
    host_to_disk(sex, p_d_val.get_rva(), &(disk->d_val));
    return disk;
}

/** Convert Dynamic Entry Tag to a string */
std::string
SgAsmElfDynamicEntry::to_string(EntryType t) 
{
    switch (t) {
      case DT_NULL:             return "DT_NULL";
      case DT_NEEDED:           return "DT_NEEDED";
      case DT_PLTRELSZ:         return "DT_PLTRELSZ";
      case DT_PLTGOT:           return "DT_PLTGOT";
      case DT_HASH:             return "DT_HASH";
      case DT_STRTAB:           return "DT_STRTAB";
      case DT_SYMTAB:           return "DT_SYMTAB";
      case DT_RELA:             return "DT_RELA";
      case DT_RELASZ:           return "DT_RELASZ";
      case DT_RELAENT:          return "DT_RELAENT";
      case DT_STRSZ:            return "DT_STRSZ";
      case DT_SYMENT:           return "DT_SYMENT";
      case DT_INIT:             return "DT_INIT";
      case DT_FINI:             return "DT_FINI";
      case DT_SONAME:           return "DT_SONAME";
      case DT_RPATH:            return "DT_RPATH";
      case DT_SYMBOLIC:         return "DT_SYMBOLIC";
      case DT_REL:              return "DT_REL";
      case DT_RELSZ:            return "DT_RELSZ";
      case DT_RELENT:           return "DT_RELENT";
      case DT_PLTREL:           return "DT_PLTREL";
      case DT_DEBUG:            return "DT_DEBUG";
      case DT_TEXTREL:          return "DT_TEXTREL";
      case DT_JMPREL:           return "DT_JMPREL";
      case DT_BIND_NOW:         return "DT_BIND_NOW";
      case DT_INIT_ARRAY:       return "DT_INIT_ARRAY";
      case DT_FINI_ARRAY:       return "DT_FINI_ARRAY";
      case DT_INIT_ARRAYSZ:     return "DT_INIT_ARRAYSZ";
      case DT_FINI_ARRAYSZ:     return "DT_FINI_ARRAYSZ";
      case DT_RUNPATH:          return "DT_RUNPATH";
      case DT_FLAGS:            return "DT_FLAGS";
      case DT_PREINIT_ARRAY:    return "DT_PREINIT_ARRAY";
      case DT_PREINIT_ARRAYSZ:  return "DT_PREINIT_ARRAYSZ";
      case DT_NUM:              return "DT_NUM";
      case DT_GNU_PRELINKED:    return "DT_GNU_PRELINKED";
      case DT_GNU_CONFLICTSZ:   return "DT_GNU_CONFLICTSZ";
      case DT_GNU_LIBLISTSZ:    return "DT_GNU_LIBLISTSZ";
      case DT_CHECKSUM:         return "DT_CHECKSUM";
      case DT_PLTPADSZ:         return "DT_PLTPADSZ";
      case DT_MOVEENT:          return "DT_MOVEENT";
      case DT_MOVESZ:           return "DT_MOVESZ";
      case DT_FEATURE_1:        return "DT_FEATURE_1";
      case DT_POSFLAG_1:        return "DT_POSFLAG_1";
      case DT_SYMINSZ:          return "DT_SYMINSZ";
      case DT_SYMINENT:         return "DT_SYMINENT";
      case DT_GNU_HASH:         return "DT_GNU_HASH";
      case DT_TLSDESC_PLT:      return "DT_TLSDESC_PLT";
      case DT_TLSDESC_GOT:      return "DT_TLSDESC_GOT";
      case DT_GNU_CONFLICT:     return "DT_GNU_CONFLICT";
      case DT_GNU_LIBLIST:      return "DT_GNU_LIBLIST";
      case DT_CONFIG:           return "DT_CONFIG";
      case DT_DEPAUDIT:         return "DT_DEPAUDIT";
      case DT_AUDIT:            return "DT_AUDIT";
      case DT_PLTPAD:           return "DT_PLTPAD";
      case DT_MOVETAB:          return "DT_MOVETAB";
      case DT_SYMINFO:          return "DT_SYMINFO";
      case DT_VERSYM:           return "DT_VERSYM";
      case DT_RELACOUNT:        return "DT_RELACOUNT";
      case DT_RELCOUNT:         return "DT_RELCOUNT";
      case DT_FLAGS_1:          return "DT_FLAGS_1";
      case DT_VERDEF:           return "DT_VERDEF";
      case DT_VERDEFNUM:        return "DT_VERDEFNUM";
      case DT_VERNEED:          return "DT_VERNEED";
      case DT_VERNEEDNUM:       return "DT_VERNEEDNUM";
      case DT_AUXILIARY:        return "DT_AUXILIARY";
      case DT_FILTER:           return "DT_FILTER";

      default:
        static char s[64];
        sprintf(s, "0x%08lx", (unsigned long)t);
        return s;
    }
}

/** Set name and adjust parent */
void
SgAsmElfDynamicEntry::set_name(SgAsmGenericString *name)
{
    if (name!=p_name) {
        if (p_name) {
            p_name->set_parent(NULL);
            delete p_name;
        }
        p_name = name;
        if (p_name)
            p_name->set_parent(this);
        set_isModified(true);
    }
}

/** Print some debugging info */
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
        fprintf(f, " \"%s\"", p_name->c_str());
    fputc('\n', f);

    if (p_extra.size()>0) {
        fprintf(f, "%s%-*s = %zu bytes\n", p, w, "extra", p_extra.size());
        hexdump(f, 0, std::string(p)+"extra at ", p_extra);
    }
}

/** Non-parsing constructor */
void
SgAsmElfDynamicSection::ctor(SgAsmElfStringSection *strings)
{
    p_entries = new SgAsmElfDynamicEntryList;
    p_entries->set_parent(this);
    ROSE_ASSERT(strings!=NULL);
    p_linked_section = strings;
}

/** Parse an existing section of a file in order to initialize this ELF Dynamic Section. */
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
#if 1       /* FIXME: Do we really want this stuff duplicated in the AST? [RPM 2008-12-12] */
            SgAsmStoredString *name2 = new SgAsmStoredString(strsec->get_strtab(), entry->get_d_val().get_rva());
            fhdr->add_dll(new SgAsmGenericDLL(name2));
#endif
        }
    }
    return this;
}

/** Return sizes for various parts of the table. See doc for SgAsmElfSection::calculate_sizes. */
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
    
/** Finish initializing the section entries. */
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

/** Called prior to unparse to make things consistent. */
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

/** Write the dynamic section back to disk */
void
SgAsmElfDynamicSection::unparse(std::ostream &f) const
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

        addr_t spos = i * entry_size;
        spos = write(f, spos, struct_size, disk);
        if (entry->get_extra().size()>0) {
            ROSE_ASSERT(entry->get_extra().size()<=extra_size);
            write(f, spos, entry->get_extra());
        }
    }

    unparse_holes(f);
}

/** Print some debugging info */
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
