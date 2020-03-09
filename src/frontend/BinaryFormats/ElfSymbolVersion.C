/* ELF Symbol Versioning
 *
 * Symver Symbol Versioning (gnu extension) defines .gnu.version, .gnu.version_d, .gnu.version_r
 * For more information see
 * Sun: http://docs.sun.com/app/docs/doc/819-0690/chapter6-54676?a=view
 * LSB: http://refspecs.freestandards.org/LSB_3.1.0/LSB-Core-generic/LSB-Core-generic/symversion.html
 * Redhat: http://people.redhat.com/drepper/symbol-versioning
 *
 * Related SageIII classes:
 *    SgAsmElfSymverSection -- GNU Symvol Version Table; The ".gnu.version" section, subclass of SgAsmElfSection. This
 *                             table is parallel to the list of symbols in the ".dynsym" section.
 *    SgAsmElfSymverEntry   -- A single entry in an SgAsmElfSymverSection, usually an index into one of the other
 *                             version tables, but some special values are also defined.
 */
#include <rosePublicConfig.h>
#ifdef ROSE_BUILD_BINARY_ANALYSIS_SUPPORT
#include "sage3basic.h"

/*========================================================================================================================
 * Methods for classes related to the GNU Symbol Version Table
 *
 * SgAsmElfSymverSection, contains a list of
 * SgAsmElfSymverEntry
 *======================================================================================================================== */

void
SgAsmElfSymverEntry::ctor(SgAsmElfSymverSection *symver)
{
    ROSE_ASSERT(NULL != symver);
    
    ROSE_ASSERT(symver->get_entries()!=NULL);
    symver->get_entries()->get_entries().push_back(this);
    ROSE_ASSERT(symver->get_entries()->get_entries().size()>0);
    set_parent(symver->get_entries());

    set_value(0);
}

void
SgAsmElfSymverEntry::dump(FILE *f, const char *prefix, ssize_t idx) const
{
    char p[4096];
    if (idx>=0) {
        sprintf(p, "%sElfSymverEntry[%zd].", prefix, idx);
    } else {
        sprintf(p, "%sElfSymver.", prefix);
    }
    const int w = std::max(1, DUMP_FIELD_WIDTH-(int)strlen(p));
  
    fprintf(f, "%s%-*s = %" PRIuPTR "", p, w, "", p_value);
    switch (p_value) {
        case 0: fprintf(f, " (local)\n"); break;
        case 1: fprintf(f, " (global)\n"); break;
        default: fprintf(f, "\n"); break;
    }
}

/* Non-parsing constructor */
void
SgAsmElfSymverSection::ctor()
{
    p_entries = new SgAsmElfSymverEntryList;
    p_entries->set_parent(this);
}

SgAsmElfSymverSection *
SgAsmElfSymverSection::parse()
{
    SgAsmElfSection::parse();
  
    SgAsmElfFileHeader *fhdr = get_elf_header();
    ROSE_ASSERT(fhdr!=NULL);
    SgAsmElfSectionTableEntry *shdr = get_section_entry();
    ROSE_ASSERT(shdr!=NULL);
  
    size_t entry_size, struct_size, extra_size, nentries;
    calculate_sizes(&entry_size, &struct_size, &extra_size, &nentries);
    ROSE_ASSERT(entry_size==shdr->get_sh_entsize());
  
    /* Parse each entry */
    for (size_t i=0; i<nentries; ++i) {
        SgAsmElfSymverEntry *entry=0;
        entry = new SgAsmElfSymverEntry(this); /*adds symver to this symver table*/
        uint16_t value;
        read_content_local(i*entry_size, &value, struct_size);
        entry->set_value(disk_to_host(fhdr->get_sex(), value));
    }
    return this;
}

rose_addr_t
SgAsmElfSymverSection::calculate_sizes(size_t *entsize, size_t *required, size_t *optional, size_t *entcount) const
{
    std::vector<size_t> extra_sizes;
    return calculate_sizes(sizeof(uint16_t),
                           sizeof(uint16_t),
                           extra_sizes,
                           entsize, required, optional, entcount);
}

void
SgAsmElfSymverSection::unparse(std::ostream &f) const
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
        uint16_t val;
    
        SgAsmElfSymverEntry *entry = p_entries->get_entries()[i];
        ByteOrder::host_to_disk(sex,entry->get_value(),&val);

        rose_addr_t spos = i * entry_size;
        spos = write(f, spos, struct_size, &val);
    }
    unparse_holes(f);
}

void
SgAsmElfSymverSection::dump(FILE *f, const char *prefix, ssize_t idx) const
{
    char p[4096];
    if (idx>=0) {
        sprintf(p, "%sElfSymverSection[%zd].", prefix, idx);
    } else {
        sprintf(p, "%sElfSymverSection.", prefix);
    }
    const int w = std::max(1, DUMP_FIELD_WIDTH-(int)strlen(p));
  
    SgAsmElfSection::dump(f, p, -1);
    fprintf(f, "%s%-*s = %" PRIuPTR " entries\n", p, w, "entries.size", p_entries->get_entries().size());
    for (size_t i = 0; i < p_entries->get_entries().size(); i++) {
        p_entries->get_entries()[i]->dump(f, p, i);
    }
  
    if (variantT() == V_SgAsmElfSymverSection) /*unless a base class*/
        hexdump(f, 0, std::string(p)+"data at ", p_data);
}

/*========================================================================================================================
 * Methods for classes related to the GNU Symbol Version Definition Table
 *
 * SgAsmElfSymverDefinedSection, each containing a list of
 * SgAsmElfSymverDefinedEntry, each containing a list of
 * SgAsmElfSymverDefinedAux, each pointing to a name string
 *======================================================================================================================== */

void
SgAsmElfSymverDefinedAux::ctor(SgAsmElfSymverDefinedEntry* entry, SgAsmElfSymverDefinedSection* symver)
{
    SgAsmElfStringSection *strsec = isSgAsmElfStringSection(symver->get_linked_section());
    ROSE_ASSERT(strsec!=NULL);

    SgAsmStoredString *name = new SgAsmStoredString(strsec->get_strtab(), 0);
    set_name(name);
    name->set_parent(this);
  
    ROSE_ASSERT(NULL != entry);
    ROSE_ASSERT(NULL != entry->get_entries());
    
    entry->get_entries()->get_entries().push_back(this);
    set_parent(entry->get_entries());
}

void
SgAsmElfSymverDefinedAux::dump(FILE *f, const char *prefix, ssize_t idx) const
{
    char p[4096];
    if (idx>=0) {
        sprintf(p, "%sElfSymverDefinedAux[%zd].", prefix, idx);
    } else {
        sprintf(p, "%sElfSymverDefinedAux.", prefix);
    }
    const int w = std::max(1, DUMP_FIELD_WIDTH-(int)strlen(p));
  
    fprintf(f, "%s%-*s = %s \n", p, w, "name", get_name()->get_string(true).c_str());
}

void
SgAsmElfSymverDefinedAux::parse(ByteOrder::Endianness sex, const ElfSymverDefinedAux_disk* disk)
{
    rose_addr_t name_offset  = disk_to_host(sex, disk->vda_name);
    get_name()->set_string(name_offset);
}

void *
SgAsmElfSymverDefinedAux::encode(ByteOrder::Endianness sex, ElfSymverDefinedAux_disk* disk) const
{
    rose_addr_t name_offset = p_name->get_offset();
    ROSE_ASSERT(name_offset!=SgAsmGenericString::unallocated);
    ByteOrder::host_to_disk(sex, name_offset, &(disk->vda_name));
    return disk;
}

void
SgAsmElfSymverDefinedEntry::ctor(SgAsmElfSymverDefinedSection *section)
{
    ROSE_ASSERT(NULL != section);
  
    ROSE_ASSERT(NULL != section->get_entries());
    section->get_entries()->get_entries().push_back(this);

    ROSE_ASSERT(section->get_entries()->get_entries().size()>0);
    set_parent(section->get_entries());

    p_entries = new SgAsmElfSymverDefinedAuxList;
    p_version  = 0;
    p_flags  = 0;
    p_index = 0;
    p_hash  = 0;
}

void
SgAsmElfSymverDefinedEntry::parse(ByteOrder::Endianness sex, const ElfSymverDefinedEntry_disk *disk)
{
    p_version  = disk_to_host(sex, disk->vd_version);
    p_flags  = disk_to_host(sex, disk->vd_flags);
    p_index = disk_to_host(sex, disk->vd_ndx);
    p_hash  = disk_to_host(sex, disk->vd_hash);  
}

void *
SgAsmElfSymverDefinedEntry::encode(ByteOrder::Endianness sex, ElfSymverDefinedEntry_disk *disk) const
{
    ByteOrder::host_to_disk(sex, p_version, &(disk->vd_version));
    ByteOrder::host_to_disk(sex, p_flags, &(disk->vd_flags));
    ByteOrder::host_to_disk(sex, p_index, &(disk->vd_ndx));
    ByteOrder::host_to_disk(sex, p_hash, &(disk->vd_hash));  
    return disk;
}

void
SgAsmElfSymverDefinedEntry::dump(FILE *f, const char *prefix, ssize_t idx) const
{
    char p[4096];
    if (idx>=0) {
        sprintf(p, "%sElfSymverDefinedEntry[%zd].", prefix, idx);
    } else {
        sprintf(p, "%sElfSymverDefinedEntry.", prefix);
    }
    const int w = std::max(1, DUMP_FIELD_WIDTH-(int)strlen(p));

    /* compact one-line-per-entry format */
    if (0==idx)
        fprintf(f, "%s%-*s   %-8s %6s %10s %6s %-6s \n", p, w, "", "Version", "Index", "Hash", "Flags", "Names");
    fprintf(f,   "%s%-*s =  0x%04zx  0x%04zx 0x%08x 0x%04x", p, w, "", p_version, p_index, p_hash, (uint32_t)p_flags);
    const SgAsmElfSymverDefinedAuxPtrList &entries=get_entries()->get_entries();
    for (size_t i=0; i < entries.size(); ++i)
        fprintf(f, "%s %s", 0==i?"":",", entries[i]->get_name()->get_string(true).c_str());
    fputc('\n', f);
}

/* Non-parsing constructor */
void
SgAsmElfSymverDefinedSection::ctor(SgAsmElfStringSection *strings)
{
    p_entries = new SgAsmElfSymverDefinedEntryList;
    p_entries->set_parent(this);
  
    ROSE_ASSERT(NULL != strings);
    p_linked_section = strings;
}

SgAsmElfSymverDefinedSection *
SgAsmElfSymverDefinedSection::parse()
{
    SgAsmElfSection::parse();
  
    SgAsmElfFileHeader *fhdr = get_elf_header();
    ROSE_ASSERT(NULL!=fhdr);
    SgAsmElfSectionTableEntry *shdr = get_section_entry();
    ROSE_ASSERT(NULL!=shdr);
    SgAsmElfStringSection *strsec = dynamic_cast<SgAsmElfStringSection*>(get_linked_section());
    ROSE_ASSERT(NULL!=strsec);
  
    //size_t struct_size=sizeof(SgAsmElfSymverDefinedEntry::ElfSymverDefinedEntry_disk);
  
    rose_addr_t entry_addr=0;

    ByteOrder::Endianness sex=fhdr->get_sex();
    /* Parse each entry*/
    while (entry_addr < this->get_size()) {
        SgAsmElfSymverDefinedEntry *entry=new SgAsmElfSymverDefinedEntry(this); /*adds SymverDefinedEntry to this*/
        SgAsmElfSymverDefinedEntry::ElfSymverDefinedEntry_disk entryDisk;
        read_content_local(entry_addr, &entryDisk, sizeof(entryDisk));
        entry->parse(sex, &entryDisk);

        /* These are relative to the start of this entry - i.e. entry_addr */
        size_t num_aux = ByteOrder::disk_to_host(sex,entryDisk.vd_cnt);
        size_t first_aux = ByteOrder::disk_to_host(sex,entryDisk.vd_aux);
        size_t next_entry = ByteOrder::disk_to_host(sex,entryDisk.vd_next);  

        rose_addr_t aux_addr=entry_addr+first_aux;
        for (size_t i=0; i < num_aux; ++i) {
            SgAsmElfSymverDefinedAux *aux=new SgAsmElfSymverDefinedAux(entry,this); /*adds SymverDefinedAux to this entry*/
            SgAsmElfSymverDefinedAux::ElfSymverDefinedAux_disk auxDisk;
            read_content_local(aux_addr, &auxDisk, sizeof(auxDisk));
            aux->parse(fhdr->get_sex(), &auxDisk);

            size_t next_aux = ByteOrder::disk_to_host(sex,auxDisk.vda_next);      
            if (next_aux == 0)
                break;
            aux_addr+=next_aux;
        }

        if(next_entry == 0)
          break;
        entry_addr += next_entry;
    }

    return this;
}

rose_addr_t
SgAsmElfSymverDefinedSection::calculate_sizes(size_t *entsize, size_t *required, size_t *optional, size_t *entcount) const
{
    size_t struct_size = sizeof(SgAsmElfSymverDefinedEntry::ElfSymverDefinedEntry_disk);
    size_t aux_size = sizeof(SgAsmElfSymverDefinedAux::ElfSymverDefinedAux_disk);

    size_t extra_size = 0;
    size_t entry_size = 0;
    size_t nentries = get_entries()->get_entries().size();

    size_t total_size=0;
    /* Entire entry should be at least large enough for the required part. */
    entry_size = struct_size;
  
    /* Each entry has a list of 'aux' structures */    
    const SgAsmElfSymverDefinedEntryPtrList &entries = get_entries()->get_entries();
    for (size_t i=0; i<nentries; ++i) {
        size_t numAux = entries[i]->get_entries()->get_entries().size();
        extra_size = std::max(extra_size, numAux * aux_size);
        total_size += (struct_size + (numAux* aux_size));
    }
    /* Note - we could try to reach into the dynamic section for nentries */
  
    /* Return values */
    if (entsize)
        *entsize = entry_size;
    if (required)
        *required = struct_size;
    if (optional)
        *optional = extra_size;
    if (entcount)
        *entcount = nentries;
    return total_size;
}

void
SgAsmElfSymverDefinedSection::unparse(std::ostream &f) const
{
    SgAsmElfFileHeader *fhdr = get_elf_header();
    ROSE_ASSERT(NULL != fhdr);
    ByteOrder::Endianness sex = fhdr->get_sex();
    size_t nentries;
    calculate_sizes(NULL,NULL,NULL,&nentries);

    /* Adjust the entry size stored in the ELF Section Table */
    get_section_entry()->set_sh_entsize(0); /*This doesn't have consistently sized entries, zero it*/
  
    /* Write each entry's required part followed by the optional part */
    rose_addr_t entry_addr=0;/*as offset from section*/

    for (size_t ent=0; ent < nentries; ++ent) {
        SgAsmElfSymverDefinedEntry *entry=get_entries()->get_entries()[ent];
        SgAsmElfSymverDefinedEntry::ElfSymverDefinedEntry_disk entryDisk;
        entry->encode(sex, &entryDisk);
        const size_t entry_size=sizeof(SgAsmElfSymverDefinedEntry::ElfSymverDefinedEntry_disk);
        const size_t aux_size=sizeof(SgAsmElfSymverDefinedAux::ElfSymverDefinedAux_disk);
    
        const SgAsmElfSymverDefinedAuxPtrList& auxes = entry->get_entries()->get_entries();
        size_t num_aux = auxes.size();
        size_t first_aux = entry_size;/*we always stick Aux's just after Entry*/
        size_t next_entry = first_aux + ( num_aux * aux_size );
        if (nentries -1 == ent) {
            next_entry=0; /*final entry: next is null (0)*/
        }

        ByteOrder::host_to_disk(sex,    num_aux, &entryDisk.vd_cnt);
        ByteOrder::host_to_disk(sex,  first_aux, &entryDisk.vd_aux);
        ByteOrder::host_to_disk(sex, next_entry, &entryDisk.vd_next);

        write(f, entry_addr, entry_size,&entryDisk);

        rose_addr_t aux_addr=entry_addr+first_aux;
        for (size_t i=0; i < num_aux; ++i) {
            SgAsmElfSymverDefinedAux *aux=auxes[i];

            SgAsmElfSymverDefinedAux::ElfSymverDefinedAux_disk auxDisk;
            aux->encode(sex, &auxDisk);
      
            size_t next_aux = aux_size;/*all auxes are contigious, so the next one is always just 8*/
            if (num_aux-1 == i) {
                next_aux=0;/* ... unless it is the final aux, then the next is null (0) */
            }
            ByteOrder::host_to_disk(sex,next_aux, &auxDisk.vda_next);

            write(f,aux_addr,aux_size, &auxDisk);
            aux_addr+=next_aux;
        }

        entry_addr += next_entry;
    }

    unparse_holes(f);
}

void
SgAsmElfSymverDefinedSection::dump(FILE *f, const char *prefix, ssize_t idx) const
{
    char p[4096];
    if (idx>=0) {
        sprintf(p, "%sElfSymverDefinedSection[%zd].", prefix, idx);
    } else {
        sprintf(p, "%sElfSymverDefinedSection.", prefix);
    }
    const int w = std::max(1, DUMP_FIELD_WIDTH-(int)strlen(p));
  
    SgAsmElfSection::dump(f, p, -1);
    fprintf(f, "%s%-*s = %" PRIuPTR " entries\n", p, w, "ElfSymverDefined.size", p_entries->get_entries().size());
    for (size_t i = 0; i < p_entries->get_entries().size(); i++) {
        p_entries->get_entries()[i]->dump(f, p, i);
    }
  
    if (variantT() == V_SgAsmElfSymverDefinedSection) /*unless a base class*/
        hexdump(f, 0, std::string(p)+"data at ", p_data);
}

/*========================================================================================================================
 * Methods for section relating to the GNU Symbol Version Requirements Table.
 *
 * SgAsmElfSymverNeededSection, each containing a list of
 * SgAsmElfSymverNeededEntry, each containing a list of
 * SgAsmElfSymverNeededAux.
 *======================================================================================================================== */

void
SgAsmElfSymverNeededAux::ctor(SgAsmElfSymverNeededEntry* entry, SgAsmElfSymverNeededSection* symver)
{
    SgAsmElfStringSection *strsec = isSgAsmElfStringSection(symver->get_linked_section());
    ROSE_ASSERT(NULL != strsec);

    p_name = new SgAsmStoredString(strsec->get_strtab(), 0);
    p_name->set_parent(this);

    ROSE_ASSERT(NULL != entry);
    ROSE_ASSERT(NULL != entry->get_entries());
    entry->get_entries()->get_entries().push_back(this);
    set_parent(entry->get_entries());

    p_flags  = 0;
    p_other = 0;
    p_hash  = 0;
}

void
SgAsmElfSymverNeededAux::dump(FILE *f, const char *prefix, ssize_t idx) const
{
    char p[4096];
    if (idx>=0) {
        sprintf(p, "%sElfSymverNeededAux[%zd].", prefix, idx);
    } else {
        sprintf(p, "%sElfSymverNeededAux.", prefix);
    }
    const int w = std::max(1, DUMP_FIELD_WIDTH-(int)strlen(p));
    fprintf(f, "%s%-*s = %04zx\n", p, w, "other", get_other());
    fprintf(f, "%s%-*s = 0x%08x\n", p, w, "hash", get_hash());
    fprintf(f, "%s%-*s = 0x%04x\n", p, w, "flags", get_flags());
    fprintf(f, "%s%-*s = %s \n", p, w, "name", get_name()->get_string(true).c_str());
}

void
SgAsmElfSymverNeededAux::parse(ByteOrder::Endianness sex, const ElfSymverNeededAux_disk* disk)
{
    p_hash = ByteOrder::disk_to_host(sex,disk->vna_hash);
    p_flags= ByteOrder::disk_to_host(sex,disk->vna_flags);
    p_other= ByteOrder::disk_to_host(sex,disk->vna_other);

    rose_addr_t name_offset  = ByteOrder::disk_to_host(sex, disk->vna_name);
    get_name()->set_string(name_offset);
}

void *
SgAsmElfSymverNeededAux::encode(ByteOrder::Endianness sex, ElfSymverNeededAux_disk* disk) const
{
    ByteOrder::host_to_disk(sex,p_hash,&disk->vna_hash);
    ByteOrder::host_to_disk(sex,p_flags,&disk->vna_flags);
    ByteOrder::host_to_disk(sex,p_other,&disk->vna_other);

    rose_addr_t name_offset = p_name->get_offset();
    ROSE_ASSERT(name_offset!=SgAsmGenericString::unallocated);
    ByteOrder::host_to_disk(sex, name_offset, &(disk->vna_name));
    return disk;
}

void
SgAsmElfSymverNeededEntry::ctor(SgAsmElfSymverNeededSection *section)
{
    ROSE_ASSERT(NULL != section);  
    ROSE_ASSERT(NULL != section->get_entries());
    section->get_entries()->get_entries().push_back(this);

    ROSE_ASSERT(section->get_entries()->get_entries().size()>0);
    set_parent(section->get_entries());

    SgAsmElfStringSection *strsec = isSgAsmElfStringSection(section->get_linked_section());
    ROSE_ASSERT(NULL != strsec);

    SgAsmStoredString *name = new SgAsmStoredString(strsec->get_strtab(), 0);
    set_file_name(name);
    name->set_parent(this);

    p_entries = new SgAsmElfSymverNeededAuxList;
    p_entries->set_parent(this);

    p_version  = 0;
}

void
SgAsmElfSymverNeededEntry::parse(ByteOrder::Endianness sex, const ElfSymverNeededEntry_disk *disk)
{
    p_version  = ByteOrder::disk_to_host(sex, disk->vn_version);
    rose_addr_t file_offset  = ByteOrder::disk_to_host(sex, disk->vn_file);
    get_file_name()->set_string(file_offset);
}

void *
SgAsmElfSymverNeededEntry::encode(ByteOrder::Endianness sex, ElfSymverNeededEntry_disk *disk) const
{
    ByteOrder::host_to_disk(sex, p_version, &(disk->vn_version));

    rose_addr_t file_offset = p_file_name->get_offset();
    ROSE_ASSERT(file_offset!=SgAsmGenericString::unallocated);
    ByteOrder::host_to_disk(sex, file_offset, &(disk->vn_file));

    return disk;
}

void
SgAsmElfSymverNeededEntry::dump(FILE *f, const char *prefix, ssize_t idx) const
{
    char p[4096];
    if (idx>=0) {
        sprintf(p, "%sElfSymverNeededEntry[%zd].", prefix, idx);
    } else {
        sprintf(p, "%sElfSymverNeededEntry.", prefix);
    }
    const int w = std::max(1, DUMP_FIELD_WIDTH-(int)strlen(p));

    /* compact one-line-per-entry format */
    if (0==idx)
        fprintf(f, "%s%-*s   %-8s %-22s %6s %10s %6s %s\n", p, w, "", "Version", "File", "Other", "Hash", "Flags", "Name");
    fprintf(f,   "%s%-*s =   0x%04zx %s", p, w, "", p_version, get_file_name()->get_string(true).c_str());
    const SgAsmElfSymverNeededAuxPtrList &entries=get_entries()->get_entries();
    if (entries.empty()) {
        fprintf(f, "<no auxiliary entries>\n");
    } else {
        fprintf(f, "\n");
    }
    for (size_t i=0; i<entries.size(); ++i) {
        SgAsmElfSymverNeededAux* aux = entries[i];
        char auxname[32];
        sprintf(auxname, "aux[%" PRIuPTR "]", i);
        fprintf(f,   "%s%-*s =                                 0x%04zx 0x%08x 0x%04x %s\n", p, w, auxname, 
                aux->get_other(), aux->get_hash(), aux->get_flags(), aux->get_name()->get_string(true).c_str());
    }
    fprintf(f, "\n");
}

/* Non-parsing constructor */
void
SgAsmElfSymverNeededSection::ctor(SgAsmElfStringSection *strings)
{
    p_entries = new SgAsmElfSymverNeededEntryList;
    p_entries->set_parent(this);
  
    ROSE_ASSERT(NULL != strings);
    p_linked_section = strings;
}

SgAsmElfSymverNeededSection *
SgAsmElfSymverNeededSection::parse()
{
    SgAsmElfSection::parse();
  
    SgAsmElfFileHeader *fhdr = get_elf_header();
    ROSE_ASSERT(NULL!=fhdr);
    SgAsmElfSectionTableEntry *shdr = get_section_entry();
    ROSE_ASSERT(NULL!=shdr);
  
    //size_t struct_size=sizeof(SgAsmElfSymverNeededEntry::ElfSymverNeededEntry_disk);
  
    rose_addr_t entry_addr=0;

    ByteOrder::Endianness sex=fhdr->get_sex();
    /* Parse each entry*/
    while (entry_addr < this->get_size()) {
        SgAsmElfSymverNeededEntry *entry=new SgAsmElfSymverNeededEntry(this); /*adds SymverNeededEntry to this*/
        SgAsmElfSymverNeededEntry::ElfSymverNeededEntry_disk entryDisk;
        read_content_local(entry_addr, &entryDisk, sizeof(entryDisk));
        entry->parse(sex, &entryDisk);

        /* These are relative to the start of this entry - i.e. entry_addr */
        size_t num_aux = ByteOrder::disk_to_host(sex,entryDisk.vn_cnt);
        size_t first_aux = ByteOrder::disk_to_host(sex,entryDisk.vn_aux);
        size_t next_entry = ByteOrder::disk_to_host(sex,entryDisk.vn_next);  

        rose_addr_t aux_addr=entry_addr+first_aux;
        for (size_t i=0; i < num_aux; ++i) {
            SgAsmElfSymverNeededAux *aux=new SgAsmElfSymverNeededAux(entry,this); /*adds SymverNeededAux to this entry*/
            SgAsmElfSymverNeededAux::ElfSymverNeededAux_disk auxDisk;
            read_content_local(aux_addr, &auxDisk, sizeof(auxDisk));
            aux->parse(sex, &auxDisk);

            size_t next_aux = ByteOrder::disk_to_host(sex,auxDisk.vna_next);      
            if (next_aux == 0)
                break;
            aux_addr+=next_aux;
        }

        if(next_entry == 0)
            break;
        entry_addr += next_entry;
    }
    return this;
}

rose_addr_t
SgAsmElfSymverNeededSection::calculate_sizes(size_t *entsize, size_t *required, size_t *optional, size_t *entcount) const
{
    size_t struct_size = sizeof(SgAsmElfSymverNeededEntry::ElfSymverNeededEntry_disk);
    size_t aux_size = sizeof(SgAsmElfSymverNeededAux::ElfSymverNeededAux_disk);

    size_t extra_size = 0;
    size_t entry_size = 0;
    size_t nentries = get_entries()->get_entries().size();

    size_t total_size=0;
    /* Entire entry should be at least large enough for the required part. */
    entry_size = struct_size;
  
    /* Each entry has a list of 'aux' structures */
    const SgAsmElfSymverNeededEntryPtrList &entries = get_entries()->get_entries();
    for (size_t i=0; i<nentries; ++i) {
        size_t numAux = entries[i]->get_entries()->get_entries().size();
        extra_size = std::max(extra_size, numAux * aux_size);
        total_size += (struct_size + (numAux* aux_size));
    }
    /* Note - we could try to reach into the dynamic section for nentries */
  
    /* Return values */
    if (entsize)
        *entsize = entry_size;
    if (required)
        *required = struct_size;
    if (optional)
        *optional = extra_size;
    if (entcount)
        *entcount = nentries;
    return total_size;
}

void
SgAsmElfSymverNeededSection::unparse(std::ostream &f) const
{
    SgAsmElfFileHeader *fhdr = get_elf_header();
    ROSE_ASSERT(NULL != fhdr);
    ByteOrder::Endianness sex = fhdr->get_sex();
    size_t nentries;
    calculate_sizes(NULL,NULL,NULL,&nentries);

    /* Adjust the entry size stored in the ELF Section Table */
    get_section_entry()->set_sh_entsize(0); /* This doesn't have consistently sized entries, zero it */
  
    /* Write each entry's required part followed by the optional part */
    /* Parse each entry*/
    rose_addr_t entry_addr=0; /* as offset from section */

    for (size_t ent=0; ent < nentries; ++ent) {
        SgAsmElfSymverNeededEntry *entry=get_entries()->get_entries()[ent];
        SgAsmElfSymverNeededEntry::ElfSymverNeededEntry_disk entryDisk;
        entry->encode(sex, &entryDisk);
        const size_t entry_size=sizeof(SgAsmElfSymverNeededEntry::ElfSymverNeededEntry_disk);
        const size_t aux_size=sizeof(SgAsmElfSymverNeededAux::ElfSymverNeededAux_disk);
    
        const SgAsmElfSymverNeededAuxPtrList& auxes = entry->get_entries()->get_entries();
        size_t num_aux = auxes.size();
        size_t first_aux = entry_size; /* we always stick Aux's just after Entry */
        size_t next_entry = first_aux + ( num_aux * aux_size );
        if (nentries -1 == ent) {
            next_entry=0; /* final entry: next is null (0) */
        }

        ByteOrder::host_to_disk(sex,    num_aux, &entryDisk.vn_cnt);
        ByteOrder::host_to_disk(sex,  first_aux, &entryDisk.vn_aux);
        ByteOrder::host_to_disk(sex, next_entry, &entryDisk.vn_next);

        write(f, entry_addr, entry_size,&entryDisk);

        rose_addr_t aux_addr=entry_addr+first_aux;
        for (size_t i=0; i < num_aux; ++i) {
            SgAsmElfSymverNeededAux *aux=auxes[i];

            SgAsmElfSymverNeededAux::ElfSymverNeededAux_disk auxDisk;
            aux->encode(sex, &auxDisk);
      
            size_t next_aux = aux_size;/* all auxes are contigious, so the next one is always just 8 */
            if (num_aux-1 == i) {
                next_aux=0; /* ... unless it is the final aux, then the next is null (0) */
            }
            ByteOrder::host_to_disk(sex,next_aux, &auxDisk.vna_next);

            write(f,aux_addr,aux_size, &auxDisk);
            aux_addr+=next_aux;
        }
        entry_addr += next_entry;
    }
    unparse_holes(f);
}

void
SgAsmElfSymverNeededSection::dump(FILE *f, const char *prefix, ssize_t idx) const
{
    char p[4096];
    if (idx>=0) {
        sprintf(p, "%sElfSymverNeededSection[%zd].", prefix, idx);
    } else {
        sprintf(p, "%sElfSymverNeededSection.", prefix);
    }
    const int w = std::max(1, DUMP_FIELD_WIDTH-(int)strlen(p));
  
    SgAsmElfSection::dump(f, p, -1);
    fprintf(f, "%s%-*s = %" PRIuPTR " entries\n", p, w, "ElfSymverNeeded.size", p_entries->get_entries().size());
    for (size_t i = 0; i < p_entries->get_entries().size(); i++) {
        p_entries->get_entries()[i]->dump(f, p, i);
    }
  
    if (variantT() == V_SgAsmElfSymverNeededSection) /*unless a base class*/
        hexdump(f, 0, std::string(p)+"data at ", p_data);
}

#endif
