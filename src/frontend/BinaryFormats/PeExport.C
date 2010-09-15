/* Windows PE Export Sections (SgAsmPEExportSection and related classes) */

// tps (01/14/2010) : Switching from rose.h to sage3.
#include "sage3basic.h"
#define __STDC_FORMAT_MACROS
#include <inttypes.h>

/* Constructor */
void
SgAsmPEExportDirectory::ctor(SgAsmPEExportSection *section)
{
    SgAsmPEFileHeader *fhdr = isSgAsmPEFileHeader(section->get_header());
    ROSE_ASSERT(fhdr!=NULL);
    set_parent(section);

    /* Read disk-formatted export directory */
    PEExportDirectory_disk disk;
    try {
        section->read_content(fhdr->get_loader_map(), section->get_mapped_actual_va(), &disk, sizeof disk);
    } catch (const MemoryMap::NotMapped &e) {
        fprintf(stderr, "SgAsmPEExportDirectory::ctor: error: export directory at RVA 0x%08"PRIx64
                " contains unmapped virtual address 0x%08"PRIx64"\n", section->get_mapped_actual_va(), e.va);
        if (e.map) {
            fprintf(stderr, "Memory map in effect at time of error:\n");
            e.map->dump(stderr, "    ");
        }
        memset(&disk, 0, sizeof disk);
    }
    
    /* Convert disk-format data members to native format */
    p_res1         = le_to_host(disk.res1);
    p_timestamp    = le_to_host(disk.timestamp);
    p_vmajor       = le_to_host(disk.vmajor);
    p_vminor       = le_to_host(disk.vminor);
    p_name_rva     = le_to_host(disk.name_rva);       p_name_rva.set_section(section);
    p_ord_base     = le_to_host(disk.ord_base);
    p_expaddr_n    = le_to_host(disk.expaddr_n);
    p_nameptr_n    = le_to_host(disk.nameptr_n);
    p_expaddr_rva  = le_to_host(disk.expaddr_rva);    p_expaddr_rva.set_section(section);
    p_nameptr_rva  = le_to_host(disk.nameptr_rva);    p_nameptr_rva.set_section(section);
    p_ordinals_rva = le_to_host(disk.ordinals_rva);   p_ordinals_rva.set_section(section);

    /* Read the name */
    std::string name;
    try {
        name = section->read_content_str(fhdr->get_loader_map(), p_name_rva.get_rva());
    } catch (const MemoryMap::NotMapped &e) {
        fprintf(stderr, "SgAsmPEExportDirectory::ctor: warning: directory name at RVA 0x%08"PRIx64
                " contains unmapped virtual address 0x%08"PRIx64"\n", p_name_rva.get_rva(), e.va);
        if (e.map) {
            fprintf(stderr, "Memory map in effect at time of error:\n");
            e.map->dump(stderr, "    ");
        }
        memset(&disk, 0, sizeof disk);
    }
    p_name = new SgAsmBasicString(name);
}

/* Print debugging info */
void
SgAsmPEExportDirectory::dump(FILE *f, const char *prefix, ssize_t idx) const
{
    char p[4096];
    if (idx>=0) {
        sprintf(p, "%sPEExportDirectory[%zd].", prefix, idx);
    } else {
        sprintf(p, "%sPEExportDirectory.", prefix);
    }
    const int w = std::max(1, DUMP_FIELD_WIDTH-(int)strlen(p));
    
    fprintf(f, "%s%-*s = \"%s\"\n",                    p, w, "name", p_name->c_str());
    fprintf(f, "%s%-*s = 0x%08x (%u)\n",               p, w, "res1", p_res1, p_res1);
    fprintf(f, "%s%-*s = %lu %s",                      p, w, "timestamp", (unsigned long)p_timestamp, ctime(&p_timestamp));
    fprintf(f, "%s%-*s = %u\n",                        p, w, "vmajor", p_vmajor);
    fprintf(f, "%s%-*s = %u\n",                        p, w, "vminor", p_vminor);
    fprintf(f, "%s%-*s = %s\n",                        p, w, "name_rva", p_name_rva.to_string().c_str());
    fprintf(f, "%s%-*s = %u\n",                        p, w, "ord_base", p_ord_base);
    fprintf(f, "%s%-*s = %zu\n",                       p, w, "expaddr_n", p_expaddr_n);
    fprintf(f, "%s%-*s = %zu\n",                       p, w, "nameptr_n", p_nameptr_n);
    fprintf(f, "%s%-*s = %s\n",                        p, w, "expaddr_rva", p_expaddr_rva.to_string().c_str());
    fprintf(f, "%s%-*s = %s\n",                        p, w, "nameptr_rva", p_nameptr_rva.to_string().c_str());
    fprintf(f, "%s%-*s = %s\n",                        p, w, "ordinals_rva", p_ordinals_rva.to_string().c_str());
}

/* Constructor */
void
SgAsmPEExportEntry::ctor(SgAsmGenericString *fname, unsigned ordinal, rva_t expaddr, SgAsmGenericString *forwarder)
{
    set_name(fname);
    set_ordinal(ordinal);
    set_export_rva(expaddr);
    set_forwarder(forwarder);
}

/* Print debugging info */
void
SgAsmPEExportEntry::dump(FILE *f, const char *prefix, ssize_t idx) const
{
    char p[4096];
    if (idx>=0) {
        sprintf(p, "%sPEExportEntry[%zd].", prefix, idx);
    } else {
        sprintf(p, "%sPEExportEntry.", prefix);
    }
    const int w = std::max(1, DUMP_FIELD_WIDTH-(int)strlen(p));

    fprintf(f, "%s%-*s = [ord %u] rva=%s \"%s\"", p, w, "info", p_ordinal, p_export_rva.to_string().c_str(), p_name->c_str());
    if (p_forwarder)
        fprintf(f, " -> \"%s\"", p_forwarder->c_str());
    fputc('\n', f);
}

/* Override ROSETTA to set parent */
void
SgAsmPEExportEntry::set_name(SgAsmGenericString *fname)
{
    if (p_name!=fname)
        set_isModified(true);
    p_name = fname;
    if (p_name) p_name->set_parent(this);
}
void
SgAsmPEExportEntry::set_forwarder(SgAsmGenericString *forwarder)
{
    if (p_forwarder!=forwarder)
        set_isModified(true);
    p_forwarder = forwarder;
    if (p_forwarder) p_forwarder->set_parent(this);
}
    
/* Constructor */
void
SgAsmPEExportSection::ctor()
{
    ROSE_ASSERT(p_exports  == NULL);
    p_exports = new SgAsmPEExportEntryList();
    p_exports->set_parent(this);
}

SgAsmPEExportSection*
SgAsmPEExportSection::parse()
{
    SgAsmPESection::parse();

    SgAsmPEFileHeader *fhdr = dynamic_cast<SgAsmPEFileHeader*>(get_header());
    ROSE_ASSERT(fhdr!=NULL);

    p_export_dir = new SgAsmPEExportDirectory(this);

    /* The export directory points to three parallel arrays:
     *   1. An array of RVAs that point to NUL-terminated names.
     *   2. An array of "ordinals" which serve as indices into the Export Address Table.
     *   3. An array of export addresses (see note below). */
    for (size_t i=0; i<p_export_dir->get_nameptr_n(); i++) {
        /* Function name RVA (nameptr)*/
        ExportNamePtr_disk nameptr_disk = 0;
        addr_t nameptr_rva = p_export_dir->get_nameptr_rva().get_rva() + i*sizeof(nameptr_disk);
        try {
            read_content(fhdr->get_loader_map(), nameptr_rva, &nameptr_disk, sizeof nameptr_disk);
        } catch (const MemoryMap::NotMapped &e) {
            fprintf(stderr, "SgAsmPEExportSection::parse: error: export name rva %zu at RVA 0x%08"PRIx64
                    " contains unmapped virtual address 0x%08"PRIx64"\n", i, nameptr_rva, e.va);
            if (e.map) {
                fprintf(stderr, "Memory map in effect at time of error:\n");
                e.map->dump(stderr, "    ");
            }
            nameptr_disk = 0;
        }
        addr_t nameptr = le_to_host(nameptr_disk);

        /* Function name (fname) */
        std::string s;
        try {
            s = read_content_str(fhdr->get_loader_map(), nameptr);
        } catch (const MemoryMap::NotMapped &e) {
            fprintf(stderr, "SgAsmPEExportSection::parse: error: export name %zu at RVA 0x%08"PRIx64
                    " contains unmapped virtual address 0x%08"PRIx64"\n", i, nameptr, e.va);
            if (e.map) {
                fprintf(stderr, "Memory map in effect at time of error:\n");
                e.map->dump(stderr, "    ");
            }
        }
        SgAsmGenericString *fname = new SgAsmBasicString(s);

        /* Ordinal (sort of an index into the Export Address Table) */
        ExportOrdinal_disk ordinal_disk = 0;
        addr_t ordinal_rva = p_export_dir->get_ordinals_rva().get_rva() + i*sizeof(ordinal_disk);
        try {
            read_content(fhdr->get_loader_map(), ordinal_rva, &ordinal_disk, sizeof ordinal_disk);
        } catch (const MemoryMap::NotMapped &e) {
            fprintf(stderr, "SgAsmPEExportSection::parse: error: ordinal %zu at RVA 0x%08"PRIx64
                    " contains unmapped virtual address 0x%08"PRIx64"\n", i, ordinal_rva, e.va);
            if (e.map) {
                fprintf(stderr, "Memory map in effect at time of error:\n");
                e.map->dump(stderr, "    ");
            }
            ordinal_disk = 0;
        }
        unsigned ordinal = le_to_host(ordinal_disk);

        /* Export address. Convert the symbol's Ordinal into an index into the Export Address Table. The spec says to subtract
         * the ord_base from the Ordinal to get the index, but testing has shown this to be off by one (e.g., Windows-XP file
         * /WINDOWS/system32/msacm32.dll's Export Table's first symbol has the name "XRegThunkEntry" with an Ordinal of zero
         * and the ord_base is one. The index according to spec would be -1 rather than the correct value of zero.) */
        rva_t expaddr = 0;
        if (ordinal >= (p_export_dir->get_ord_base()-1)) {
            unsigned expaddr_idx = ordinal - (p_export_dir->get_ord_base()-1);
            ROSE_ASSERT(expaddr_idx < p_export_dir->get_expaddr_n());
            ExportAddress_disk expaddr_disk = 0;
            addr_t expaddr_rva = p_export_dir->get_expaddr_rva().get_rva() + expaddr_idx*sizeof(expaddr_disk);
            try {
                read_content(fhdr->get_loader_map(), expaddr_rva, &expaddr_disk, sizeof expaddr_disk);
            } catch (const MemoryMap::NotMapped &e) {
                fprintf(stderr, "SgAsmPEExportSection::parse: error: export address %zu at RVA 0x%08"PRIx64
                        " contains unmapped virtual address 0x%08"PRIx64"\n", i, expaddr_rva, e.va);
                if (e.map) {
                    fprintf(stderr, "Memory map in effect at time of error:\n");
                    e.map->dump(stderr, "    ");
                }
            }
            expaddr = le_to_host(expaddr_disk);
            expaddr.bind(fhdr);
        } else {
            expaddr = 0xffffffff; /*Ordinal out of range!*/
        }

        /* If export address is within this section then it points to a NUL-terminated forwarder name.
         * FIXME: Is this the proper precondition? [RPM 2009-08-20] */
        SgAsmGenericString *forwarder = NULL;
        if (expaddr.get_va()>=get_mapped_actual_va() && expaddr.get_va()<get_mapped_actual_va()+get_mapped_size()) {
            std::string s;
            try {
                s = read_content_str(fhdr->get_loader_map(), expaddr.get_rva());
            } catch (const MemoryMap::NotMapped &e) {
                fprintf(stderr, "SgAsmPEExportSection::parse: error: forwarder %zu at RVA 0x%08"PRIx64
                        " contains unmapped virtual address 0x%08"PRIx64"\n", i, expaddr.get_rva(), e.va);
                if (e.map) {
                    fprintf(stderr, "Memory map in effect at time of error:\n");
                    e.map->dump(stderr, "    ");
                }
            }
            forwarder = new SgAsmBasicString(s);
        }

        SgAsmPEExportEntry *entry = new SgAsmPEExportEntry(fname, ordinal, expaddr, forwarder);
        add_entry(entry);
    }
    return this;
}

void
SgAsmPEExportSection::add_entry(SgAsmPEExportEntry *entry)
{
    ROSE_ASSERT(p_exports!=NULL);
    p_exports->set_isModified(true);
    p_exports->get_exports().push_back(entry);
}

/* Print debugging info */
void
SgAsmPEExportSection::dump(FILE *f, const char *prefix, ssize_t idx) const
{
    char p[4096];
    if (idx>=0) {
        sprintf(p, "%sPEExportSection[%zd].", prefix, idx);
    } else {
        sprintf(p, "%sPEExportSection.", prefix);
    }
    
    SgAsmPESection::dump(f, p, -1);

    if (p_export_dir)
        p_export_dir->dump(f, p, -1);
    for (size_t i=0; i<p_exports->get_exports().size(); i++)
        p_exports->get_exports()[i]->dump(f, p, i);

    if (variantT() == V_SgAsmPEExportSection) //unless a base class
        hexdump(f, 0, std::string(p)+"data at ", p_data);
}
