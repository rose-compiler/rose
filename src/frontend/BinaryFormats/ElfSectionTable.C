/* ELF Section Tables (SgAsmElfSectionTable and related classes) */
#include "sage3basic.h"
#include "stringify.h"
#define __STDC_FORMAT_MACROS
#include <inttypes.h>

/** Converts 32-bit disk representation to host representation */
void
SgAsmElfSectionTableEntry::ctor(ByteOrder sex, const Elf32SectionTableEntry_disk *disk) 
{
    p_sh_name      = disk_to_host(sex, disk->sh_name);
    p_sh_type      = (SectionType)disk_to_host(sex, disk->sh_type);
    p_sh_flags     = disk_to_host(sex, disk->sh_flags);
    p_sh_addr      = disk_to_host(sex, disk->sh_addr);
    p_sh_offset    = disk_to_host(sex, disk->sh_offset);
    p_sh_size      = disk_to_host(sex, disk->sh_size);
    p_sh_link      = disk_to_host(sex, disk->sh_link);
    p_sh_info      = disk_to_host(sex, disk->sh_info);
    p_sh_addralign = disk_to_host(sex, disk->sh_addralign);
    p_sh_entsize   = disk_to_host(sex, disk->sh_entsize);
}
    
/** Converts 64-bit disk representation to host representation */
void
SgAsmElfSectionTableEntry::ctor(ByteOrder sex, const Elf64SectionTableEntry_disk *disk) 
{
    p_sh_name      = disk_to_host(sex, disk->sh_name);
    p_sh_type      = (SectionType)disk_to_host(sex, disk->sh_type);
    p_sh_flags     = disk_to_host(sex, disk->sh_flags);
    p_sh_addr      = disk_to_host(sex, disk->sh_addr);
    p_sh_offset    = disk_to_host(sex, disk->sh_offset);
    p_sh_size      = disk_to_host(sex, disk->sh_size);
    p_sh_link      = disk_to_host(sex, disk->sh_link);
    p_sh_info      = disk_to_host(sex, disk->sh_info);
    p_sh_addralign = disk_to_host(sex, disk->sh_addralign);
    p_sh_entsize   = disk_to_host(sex, disk->sh_entsize);
}

/** Encode a section table entry into the disk structure */
void *
SgAsmElfSectionTableEntry::encode(ByteOrder sex, Elf32SectionTableEntry_disk *disk) const
{
    host_to_disk(sex, p_sh_name,      &(disk->sh_name));
    host_to_disk(sex, p_sh_type,      &(disk->sh_type));
    host_to_disk(sex, p_sh_flags,     &(disk->sh_flags));
    host_to_disk(sex, p_sh_addr,      &(disk->sh_addr));
    host_to_disk(sex, p_sh_offset,    &(disk->sh_offset));
    host_to_disk(sex, p_sh_size,      &(disk->sh_size));
    host_to_disk(sex, p_sh_link,      &(disk->sh_link));
    host_to_disk(sex, p_sh_info,      &(disk->sh_info));
    host_to_disk(sex, p_sh_addralign, &(disk->sh_addralign));
    host_to_disk(sex, p_sh_entsize,   &(disk->sh_entsize));

    return disk;
}
void *
SgAsmElfSectionTableEntry::encode(ByteOrder sex, Elf64SectionTableEntry_disk *disk) const
{
    host_to_disk(sex, p_sh_name,      &(disk->sh_name));
    host_to_disk(sex, p_sh_type,      &(disk->sh_type));
    host_to_disk(sex, p_sh_flags,     &(disk->sh_flags));
    host_to_disk(sex, p_sh_addr,      &(disk->sh_addr));
    host_to_disk(sex, p_sh_offset,    &(disk->sh_offset));
    host_to_disk(sex, p_sh_size,      &(disk->sh_size));
    host_to_disk(sex, p_sh_link,      &(disk->sh_link));
    host_to_disk(sex, p_sh_info,      &(disk->sh_info));
    host_to_disk(sex, p_sh_addralign, &(disk->sh_addralign));
    host_to_disk(sex, p_sh_entsize,   &(disk->sh_entsize));

    return disk;
}

/** Non-parsing constructor for an ELF Section Table */
void
SgAsmElfSectionTable::ctor()
{
    /* There can be only one ELF Section Table */
    SgAsmElfFileHeader *fhdr = dynamic_cast<SgAsmElfFileHeader*>(get_header());
    ROSE_ASSERT(fhdr);
    ROSE_ASSERT(fhdr->get_section_table()==NULL);

    set_synthesized(true);                              /* the section table isn't really a section itself */
    set_name(new SgAsmBasicString("ELF Section Table"));
    set_purpose(SP_HEADER);

    /* Every section table has a first entry that's all zeros. We don't declare that section here (see parse()) but we do set
     * the section count in the header in order to reserve that first slot. */
    if (fhdr->get_e_shnum()<1)
        fhdr->set_e_shnum(1);

    fhdr->set_section_table(this);
}
    
/** Parses an ELF Section Table and constructs and parses all sections reachable from the table. The section is extended as
 *  necessary based on the number of entries and the size of each entry. */
SgAsmElfSectionTable *
SgAsmElfSectionTable::parse()
{
    SgAsmGenericSection::parse();

    SgAsmElfFileHeader *fhdr = dynamic_cast<SgAsmElfFileHeader*>(get_header());
    ROSE_ASSERT(fhdr!=NULL);
    ByteOrder sex = fhdr->get_sex();

    size_t ent_size, struct_size, opt_size, nentries;
    calculate_sizes(&ent_size, &struct_size, &opt_size, &nentries);
    ROSE_ASSERT(opt_size==fhdr->get_shextrasz() && nentries==fhdr->get_e_shnum());

    /* If the current size is very small (0 or 1 byte) then we're coming straight from the constructor and the parsing should
     * also extend this section to hold all the entries. Otherwise the caller must have assigned a specific size for a good
     * reason and we should leave that alone, reading zeros if the entries extend beyond the defined size. */
    if (get_size()<=1 && get_size()<nentries*ent_size)
        extend(nentries*ent_size - get_size());

    /* Read all the section headers. */
    std::vector<SgAsmElfSectionTableEntry*> entries;
    rose_addr_t offset = 0;
    for (size_t i=0; i<nentries; i++, offset+=ent_size) {
        SgAsmElfSectionTableEntry *shdr = NULL;
        if (4 == fhdr->get_word_size()) {
            SgAsmElfSectionTableEntry::Elf32SectionTableEntry_disk disk;
            read_content_local(offset, &disk, struct_size);
            shdr = new SgAsmElfSectionTableEntry(sex, &disk);
        } else {
            SgAsmElfSectionTableEntry::Elf64SectionTableEntry_disk disk;
            read_content_local(offset, &disk, struct_size);
            shdr = new SgAsmElfSectionTableEntry(sex, &disk);
        }
        if (opt_size>0)
            shdr->get_extra() = read_content_local_ucl(offset+struct_size, opt_size);
        entries.push_back(shdr);
    }

    /* This vector keeps track of which sections have already been parsed. We could get the same information by calling
     * fhdr->get_section_by_id() and passing the entry number since entry numbers and IDs are one and the same in ELF. However,
     * this is a bit easier. */
    std::vector<SgAsmElfSection*> is_parsed;
    is_parsed.resize(entries.size(), NULL);

    /* All sections implicitly depend on the section string table for their names. */
    SgAsmElfStringSection *section_name_strings=NULL;
    if (fhdr->get_e_shstrndx() > 0) {
        SgAsmElfSectionTableEntry *entry = entries[fhdr->get_e_shstrndx()];
        section_name_strings = new SgAsmElfStringSection(fhdr);
        section_name_strings->init_from_section_table(entry, section_name_strings, fhdr->get_e_shstrndx());
        section_name_strings->parse();
        is_parsed[fhdr->get_e_shstrndx()] = section_name_strings;
    }

    /* Read all the sections. Some sections depend on other sections, so we read them in such an order that all dependencies
     * are satisfied first. */
    while (1) {
        bool try_again=false;
        for (size_t i=0; i<entries.size(); i++) {
            SgAsmElfSectionTableEntry *entry = entries[i];
            ROSE_ASSERT(entry->get_sh_link()<entries.size());

            /* Some sections might reference another section through the sh_link member. */
            bool need_linked = entry->get_sh_link() > 0;
            ROSE_ASSERT(!need_linked || entry->get_sh_link()<entries.size());
            SgAsmElfSection *linked = need_linked ? is_parsed[entry->get_sh_link()] : NULL;

            /* Relocation sections might have a second linked section stored in sh_info. */
	    bool need_info_linked = (entry->get_sh_type() == SgAsmElfSectionTableEntry::SHT_REL ||
                                     entry->get_sh_type() == SgAsmElfSectionTableEntry::SHT_RELA) &&
                                    entry->get_sh_info() > 0;
            ROSE_ASSERT(!need_info_linked || entry->get_sh_info()<entries.size());
            SgAsmElfSection *info_linked = need_info_linked ? is_parsed[entry->get_sh_info()] : NULL;

            if (is_parsed[i]) {
                /* This section has already been parsed. */
            } else if ((need_linked && !linked) || (need_info_linked && !info_linked)) {
                /* Don't parse this section yet because it depends on something that's not parsed yet. */
                try_again = true;
            } else {
                switch (entry->get_sh_type()) {
                    case SgAsmElfSectionTableEntry::SHT_NULL:
                        /* Null entry. We still create the section just to hold the section header. */
                        is_parsed[i] = new SgAsmElfSection(fhdr);
                        break;
                    case SgAsmElfSectionTableEntry::SHT_NOBITS:
                        /* These types of sections don't occupy any file space (e.g., BSS) */
                        is_parsed[i] = new SgAsmElfSection(fhdr);
                        break;
                    case SgAsmElfSectionTableEntry::SHT_DYNAMIC: {
                        SgAsmElfStringSection *strsec = dynamic_cast<SgAsmElfStringSection*>(linked);
                        ROSE_ASSERT(strsec);
                        is_parsed[i] = new SgAsmElfDynamicSection(fhdr, strsec);
                        break;
                    }
                    case SgAsmElfSectionTableEntry::SHT_DYNSYM: {
                        SgAsmElfStringSection *strsec = dynamic_cast<SgAsmElfStringSection*>(linked);
                        ROSE_ASSERT(strsec);
                        SgAsmElfSymbolSection *symsec = new SgAsmElfSymbolSection(fhdr, strsec);
                        symsec->set_is_dynamic(true);
                        is_parsed[i] = symsec;
                        break;
                    }
                    case SgAsmElfSectionTableEntry::SHT_SYMTAB: {
                        SgAsmElfStringSection *strsec = dynamic_cast<SgAsmElfStringSection*>(linked);
                        ROSE_ASSERT(strsec);
                        SgAsmElfSymbolSection *symsec = new SgAsmElfSymbolSection(fhdr, strsec);
                        symsec->set_is_dynamic(false);
                        is_parsed[i] = symsec;
                        break;
                    }
                    case SgAsmElfSectionTableEntry::SHT_STRTAB:
                        is_parsed[i] = new SgAsmElfStringSection(fhdr);
                        break;
                    case SgAsmElfSectionTableEntry::SHT_REL: {
                        SgAsmElfSymbolSection *symbols = dynamic_cast<SgAsmElfSymbolSection*>(linked);
                        ROSE_ASSERT(symbols);
                        SgAsmElfRelocSection *relocsec = new SgAsmElfRelocSection(fhdr, symbols, info_linked);
                        relocsec->set_uses_addend(false);
                        is_parsed[i] = relocsec;
                        break;
                    }
                    case SgAsmElfSectionTableEntry::SHT_RELA: {
                        SgAsmElfSymbolSection *symbols = dynamic_cast<SgAsmElfSymbolSection*>(linked);
                        ROSE_ASSERT(symbols);
                        SgAsmElfRelocSection *relocsec = new SgAsmElfRelocSection(fhdr, symbols, info_linked);
                        relocsec->set_uses_addend(true);
                        is_parsed[i] = relocsec;
                        break;
                    }
                    case SgAsmElfSectionTableEntry::SHT_PROGBITS: {
                        std::string section_name = section_name_strings->read_content_local_str(entry->get_sh_name());
                        if (section_name == ".eh_frame") {
                            is_parsed[i] = new SgAsmElfEHFrameSection(fhdr);
                        } else {
                            is_parsed[i] = new SgAsmElfSection(fhdr);
                        }
                        break;
                    }
                    case SgAsmElfSectionTableEntry::SHT_GNU_versym: {
                        is_parsed[i] = new SgAsmElfSymverSection(fhdr);
                        break;
                    }
                    case SgAsmElfSectionTableEntry::SHT_GNU_verdef: {
                        SgAsmElfStringSection *strsec = dynamic_cast<SgAsmElfStringSection*>(linked);
                        ROSE_ASSERT(strsec);
                        is_parsed[i] = new SgAsmElfSymverDefinedSection(fhdr,strsec);
                        break;
                    }
                    case SgAsmElfSectionTableEntry::SHT_GNU_verneed: {
                        SgAsmElfStringSection *strsec = dynamic_cast<SgAsmElfStringSection*>(linked);
                        ROSE_ASSERT(strsec);
                        is_parsed[i] = new SgAsmElfSymverNeededSection(fhdr,strsec);
                        break;
                    }
                    default:
                        is_parsed[i] = new SgAsmElfSection(fhdr);
                        break;
                }
                is_parsed[i]->init_from_section_table(entry, section_name_strings, i);
                is_parsed[i]->parse();
            }
        }
        if (!try_again)
            break;
    }

#if 1 /*This will be going away shortly [RPM 2008-12-12]*/
    /* Initialize links between sections */
    for (size_t i = 0; i < entries.size(); i++) {
        SgAsmElfSectionTableEntry *shdr = entries[i];
        if (shdr->get_sh_link() > 0) {
            SgAsmElfSection *source = dynamic_cast<SgAsmElfSection*>(fhdr->get_file()->get_section_by_id(i));
            SgAsmElfSection *target = dynamic_cast<SgAsmElfSection*>(fhdr->get_file()->get_section_by_id(shdr->get_sh_link()));
            source->set_linked_section(target);
        }
    }
#endif

    /* Finish parsing sections now that we have basic info for all the sections. */
    for (size_t i=0; i<is_parsed.size(); i++)
        is_parsed[i]->finish_parsing();

    return this;
}

/** Attaches a previously unattached ELF Section to the section table. If @p section is an  ELF String Section
 *  (SgAsmElfStringSection) that contains an ELF String Table (SgAsmElfStringTable) and the ELF Section Table has no
 *  associated string table then the @p section will be used as the string table to hold the section names.
 *
 *  This method complements SgAsmElfSection::init_from_section_table. This method initializes the section table from the
 *  section while init_from_section_table() initializes the section from the section table.
 *
 *  Returns the new section table entry linked into the AST. */
SgAsmElfSectionTableEntry *
SgAsmElfSectionTable::add_section(SgAsmElfSection *section)
{
    ROSE_ASSERT(section!=NULL);
    ROSE_ASSERT(section->get_file()==get_file());
    ROSE_ASSERT(section->get_header()==get_header());
    ROSE_ASSERT(section->get_section_entry()==NULL);            /* must not be in the section table yet */
    
    SgAsmElfFileHeader *fhdr = dynamic_cast<SgAsmElfFileHeader*>(get_header());
    ROSE_ASSERT(fhdr!=NULL);

    /* Assign an ID if there isn't one yet */
    if (section->get_id()<0) {
        int id = fhdr->get_e_shnum();
        fhdr->set_e_shnum(id+1);
        section->set_id(id);
    }

    /* If the supplied section is a string table and the ELF Section Table doesn't have a string table associated with it yet,
     * then use the supplied section as the string table to hold the names of the sections. When this happens, all sections
     * that are already defined in the ELF Section Table should have their names moved into the new string table. */
    SgAsmElfStringSection *strsec = NULL;
    if (fhdr->get_e_shstrndx()==0) {
        strsec = dynamic_cast<SgAsmElfStringSection*>(section);
        if (strsec) {
            fhdr->set_e_shstrndx(section->get_id());
            SgAsmGenericSectionList *all = fhdr->get_sections();
            for (size_t i=0; i<all->get_sections().size(); i++) {
                SgAsmElfSection *s = dynamic_cast<SgAsmElfSection*>(all->get_sections()[i]);
                if (s && s->get_id()>=0 && s->get_section_entry()!=NULL) {
                    s->allocate_name_to_storage(strsec);
                }
            }
        }
    } else {
        strsec = dynamic_cast<SgAsmElfStringSection*>(fhdr->get_section_by_id(fhdr->get_e_shstrndx()));
        ROSE_ASSERT(strsec!=NULL);
    }

    /* Make sure the name is in the correct string table */
    if (strsec)
        section->allocate_name_to_storage(strsec);

    /* Create a new section table entry. */
    SgAsmElfSectionTableEntry *shdr = new SgAsmElfSectionTableEntry;
    shdr->update_from_section(section);
    section->set_section_entry(shdr);

    return shdr;
}

/** Make this section's name to be stored in the specified string table. */
void
SgAsmElfSection::allocate_name_to_storage(SgAsmElfStringSection *strsec)
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

/** Returns info about the size of the entries based on information already available. Any or all arguments may be null
 *  pointers if the caller is not interested in the value. */
rose_addr_t
SgAsmElfSectionTable::calculate_sizes(size_t *entsize, size_t *required, size_t *optional, size_t *entcount) const
{
    SgAsmElfFileHeader *fhdr = dynamic_cast<SgAsmElfFileHeader*>(get_header());
    ROSE_ASSERT(fhdr!=NULL);

    size_t struct_size = 0;
    size_t extra_size = fhdr->get_shextrasz();
    size_t entry_size = 0;
    size_t nentries = 0;

    /* Size of required part of each entry */
    if (4==fhdr->get_word_size()) {
        struct_size = sizeof(SgAsmElfSectionTableEntry::Elf32SectionTableEntry_disk);
    } else if (8==fhdr->get_word_size()) {
        struct_size = sizeof(SgAsmElfSectionTableEntry::Elf64SectionTableEntry_disk);
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
        if (elfsec && elfsec->get_section_entry()) {
            ROSE_ASSERT(elfsec->get_id()>=0);
            nentries = std::max(nentries, (size_t)elfsec->get_id()+1);
            extra_size = std::max(extra_size, elfsec->get_section_entry()->get_extra().size());
        }
    }

    /* Total number of entries. Either we haven't parsed the section table yet (nor created the sections it defines) or we
     * have. In the former case we use the setting from the ELF File Header. Otherwise the table has to be large enough to
     * store the section with the largest ID (ID also serves as the index into the ELF Section Table). */
    if (0==nentries)
        nentries = fhdr->get_e_shnum();

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

/** Update this section table entry with newer information from the section */
void
SgAsmElfSectionTableEntry::update_from_section(SgAsmElfSection *section)
{
    if (section->get_name()->get_offset()==SgAsmGenericString::unallocated) {
        p_sh_name = 0; /*not a stored string after all*/
    } else {
        p_sh_name = section->get_name()->get_offset();
    }

    set_sh_offset(section->get_offset());
    if (get_sh_type()==SHT_NOBITS && section->is_mapped()) {
        set_sh_size(section->get_mapped_size());
    } else {
        set_sh_size(section->get_size());
    }

    if (section->is_mapped()) {
        set_sh_addr(section->get_mapped_preferred_rva());
        set_sh_addralign(section->get_mapped_alignment());
        if (section->get_mapped_wperm()) {
            p_sh_flags |= SHF_WRITE;
        } else {
            p_sh_flags &= ~SHF_WRITE;
        }
        if (section->get_mapped_xperm()) {
            p_sh_flags |=  SHF_EXECINSTR;
        } else {
            p_sh_flags &= ~SHF_EXECINSTR;
        }
    } else {
        set_sh_addr(0);
        p_sh_flags &= ~(SHF_WRITE | SHF_EXECINSTR); /*clear write & execute bits*/
    }
    
    SgAsmElfSection *linked_to = section->get_linked_section();
    if (linked_to) {
        ROSE_ASSERT(linked_to->get_id()>0);
        set_sh_link(linked_to->get_id());
    } else {
        set_sh_link(0);
    }
}

/** Change symbol to string */
std::string
SgAsmElfSectionTableEntry::to_string(SectionType t)
{
#ifndef _MSC_VER
    std::string retval = stringifySgAsmElfSectionTableEntrySectionType(t);
#else
	ROSE_ASSERT(false);
	std::string retval = "";
#endif
	if ('('!=retval[0])
        return retval;
    
    char buf[128];
    if(t>=SHT_LOOS && t <= SHT_HIOS) {
        snprintf(buf,sizeof(buf),"os-specific (%zu)",size_t(t)) ;
        return buf;
    }
    if (t>=SHT_LOPROC && t<=SHT_HIPROC) {
        snprintf(buf,sizeof(buf),"processor-specific (%zu)",size_t(t)) ;
        return buf;
    }
    if (t>=SHT_LOUSER && t<=SHT_HIUSER) {
        snprintf(buf,sizeof(buf),"application-specific (%zu)",size_t(t)) ;
        return buf;
    }
    snprintf(buf,sizeof(buf),"unknown section type (%zu)",size_t(t)) ;
    return buf;
}

std::string
SgAsmElfSectionTableEntry::to_string(SectionFlags val)
{
  std::string str;
  static const uint32_t kBaseMask=0x3ff;
  for( size_t i=0; (1u <<i) <= SHF_TLS; ++i){
    if( i!= 0)
      str += ' ';
    if(val & (1 << i)){
      switch(1 << i){
        case SHF_NULL:      str += "NULL";break;
	case SHF_WRITE:     str += "WRITE";break;
	case SHF_ALLOC:     str += "ALLOC";break;
	case SHF_EXECINSTR: str +=  "CODE";break;
	case SHF_MERGE:     str += "MERGE";break;/** Might be merged */
	case SHF_STRINGS:   str += "STRINGS";break;
	case SHF_INFO_LINK: str += "INFO_LINK";break;
	case SHF_LINK_ORDER:str += "LINK_ORDER";break;
	case SHF_OS_NONCONFORMING:str += "OS SPECIAL";break;
	case SHF_GROUP:     str += "GROUP";break;
	case SHF_TLS:       str += "TLS";break;
      };
    }
  }
  uint32_t os   = (val & ~(uint32_t)(SHF_MASKOS));
  uint32_t proc = (val & ~(uint32_t)(SHF_MASKPROC));
  uint32_t rest = (val & ~(uint32_t)(kBaseMask | SHF_MASKOS | SHF_MASKPROC));

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

/** Print some debugging info */
void
SgAsmElfSectionTableEntry::dump(FILE *f, const char *prefix, ssize_t idx) const
{
    char p[4096];
    if (idx>=0) {
        sprintf(p, "%sElfSectionTableEntry[%zd].", prefix, idx);
    } else {
        sprintf(p, "%sElfSectionTableEntry.", prefix);
    }
    const int w = std::max(1, DUMP_FIELD_WIDTH-(int)strlen(p));
    
    fprintf(f, "%s%-*s = %u bytes into strtab\n",                      p, w, "sh_name",        p_sh_name);
    fprintf(f, "%s%-*s = 0x%x (%d) %s\n",                              p, w, "sh_type", 
            p_sh_type, p_sh_type, to_string(p_sh_type).c_str());
    fprintf(f, "%s%-*s = %lu\n",                                       p, w, "sh_link",        p_sh_link);
    fprintf(f, "%s%-*s = %lu\n",                                       p, w, "sh_info",        p_sh_info);
    fprintf(f, "%s%-*s = 0x%08"PRIx64"\n",                             p, w, "sh_flags",       p_sh_flags);
    fprintf(f, "%s%-*s = 0x%08"PRIx64" (%"PRIu64")\n",                 p, w, "sh_addr",        p_sh_addr, p_sh_addr);
    fprintf(f, "%s%-*s = 0x%08"PRIx64" (%"PRIu64") bytes into file\n", p, w, "sh_offset",      p_sh_offset, p_sh_offset);
    fprintf(f, "%s%-*s = 0x%08"PRIx64" (%"PRIu64") bytes\n",           p, w, "sh_size",        p_sh_size, p_sh_size);
    fprintf(f, "%s%-*s = 0x%08"PRIx64" (%"PRIu64") bytes\n",           p, w, "sh_addralign",   p_sh_addralign, p_sh_addralign);
    fprintf(f, "%s%-*s = 0x%08"PRIx64" (%"PRIu64") bytes\n",           p, w, "sh_entsize",     p_sh_entsize, p_sh_entsize);
    if (p_extra.size()>0) {
        fprintf(f, "%s%-*s = %zu bytes\n", p, w, "extra", p_extra.size());
        hexdump(f, 0, std::string(p)+"extra at ", p_extra);
    }
}


/** Pre-unparsing updates */
bool
SgAsmElfSectionTable::reallocate()
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
    fhdr->set_shextrasz(opt_size);
    fhdr->set_e_shnum(nentries);

    return reallocated;
}

/** Write the section table section back to disk */
void
SgAsmElfSectionTable::unparse(std::ostream &f) const
{
    SgAsmElfFileHeader *fhdr = dynamic_cast<SgAsmElfFileHeader*>(get_header());
    ROSE_ASSERT(fhdr!=NULL);
    ByteOrder sex = fhdr->get_sex();
    SgAsmGenericSectionPtrList sections = fhdr->get_sectab_sections();

    /* Write the sections first */
    for (size_t i=0; i<sections.size(); i++)
        sections[i]->unparse(f);
    unparse_holes(f);

    /* Calculate sizes. The ELF File Header should have been updated in reallocate() prior to unparsing. */
    size_t ent_size, struct_size, opt_size, nentries;
    calculate_sizes(&ent_size, &struct_size, &opt_size, &nentries);
    ROSE_ASSERT(fhdr->get_shextrasz()==opt_size);
    ROSE_ASSERT(fhdr->get_e_shnum()==nentries);
    
    /* Write the section table entries */
    for (size_t i=0; i<sections.size(); ++i) {
        SgAsmElfSection *section = dynamic_cast<SgAsmElfSection*>(sections[i]);
        ROSE_ASSERT(section!=NULL);
        SgAsmElfSectionTableEntry *shdr = section->get_section_entry();
        ROSE_ASSERT(shdr!=NULL);
        ROSE_ASSERT(shdr->get_sh_offset()==section->get_offset());/*section table entry should have been updated in reallocate()*/

        int id = section->get_id();
        ROSE_ASSERT(id>=0 && (size_t)id<nentries);

        SgAsmElfSectionTableEntry::Elf32SectionTableEntry_disk disk32;
        SgAsmElfSectionTableEntry::Elf64SectionTableEntry_disk disk64;
        void *disk  = NULL;

        if (4==fhdr->get_word_size()) {
            disk = shdr->encode(sex, &disk32);
        } else if (8==fhdr->get_word_size()) {
            disk = shdr->encode(sex, &disk64);
        } else {
            ROSE_ASSERT(!"invalid word size");
        }

        /* The disk struct */
        rose_addr_t spos = write(f, id*ent_size, struct_size, disk);
        if (shdr->get_extra().size() > 0) {
            ROSE_ASSERT(shdr->get_extra().size()<=opt_size);
            write(f, spos, shdr->get_extra());
        }
    }
}

/** Print some debugging info */
void
SgAsmElfSectionTable::dump(FILE *f, const char *prefix, ssize_t idx) const
{
    char p[4096];
    if (idx>=0) {
        sprintf(p, "%sSectionTable[%zd].", prefix, idx);
    } else {
        sprintf(p, "%sSectionTable.", prefix);
    }

    SgAsmGenericSection::dump(f, p, -1);

    if (variantT() == V_SgAsmElfSectionTable) //unless a base class
        hexdump(f, 0, std::string(p)+"data at ", p_data);
}

