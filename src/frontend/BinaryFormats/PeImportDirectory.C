/* PE Import Directory. A PE Import Section is a list of PE Import Directories. */
#include "sage3basic.h"
#define __STDC_FORMAT_MACROS
#include <inttypes.h>

/* Constructor */
void
SgAsmPEImportDirectory::ctor(SgAsmPEImportSection *section, size_t idx, rose_addr_t *idir_rva_p)
{
    ROSE_ASSERT(idir_rva_p!=NULL);
    SgAsmPEFileHeader *fhdr = isSgAsmPEFileHeader(section->get_header());
    ROSE_ASSERT(fhdr!=NULL);

    set_parent(section);

    size_t entry_size = sizeof(PEImportDirectory_disk);
    PEImportDirectory_disk disk, zero;
    memset(&zero, 0, sizeof zero);
    section->read_content(fhdr->get_loader_map(), *idir_rva_p, &disk, entry_size); /*may throw MemoryMap::NotMapped*/

    *idir_rva_p += entry_size;

    if (0==memcmp(&disk, &zero, sizeof zero)) {
        p_idx = -1;
    } else {
        p_idx = idx;
    }
    
    p_ilt_rva         = le_to_host(disk.ilt_rva);
    p_time            = le_to_host(disk.time);
    p_forwarder_chain = le_to_host(disk.forwarder_chain);
    p_dll_name_rva    = le_to_host(disk.dll_name_rva);
    p_iat_rva         = le_to_host(disk.iat_rva);

    /* Bind RVAs to best sections */
    p_ilt_rva.bind(fhdr);
    p_dll_name_rva.bind(fhdr);
    p_iat_rva.bind(fhdr);

    p_dll_name = NULL;
    try {
        if (p_idx>=0)
            p_dll_name = new SgAsmBasicString(section->read_content_str(fhdr->get_loader_map(), p_dll_name_rva));
    } catch (const MemoryMap::NotMapped &e) {
        if (SgAsmPEImportSection::import_mesg("SgAsmPEImportDirectory::ctor: error: in PE Import Directory entry %zu: "
                                              "Name RVA starting at 0x%08"PRIx64
                                              " contains unmapped virtual address 0x%08"PRIx64"\n", 
                                              idx, p_dll_name_rva.get_rva(), e.va)) {
            if (e.map) {
                fprintf(stderr, "Memory map in effect at time of error is:\n");
                e.map->dump(stderr, "    ");
            }
        }
    }
    if (!p_dll_name)
        p_dll_name = new SgAsmBasicString("");
}

/* Encode a directory entry back into disk format */
void *
SgAsmPEImportDirectory::encode(PEImportDirectory_disk *disk) const
{
    host_to_le(p_ilt_rva.get_rva(),      &(disk->ilt_rva));
    host_to_le(p_time,                   &(disk->time));
    host_to_le(p_forwarder_chain,        &(disk->forwarder_chain));
    host_to_le(p_dll_name_rva.get_rva(), &(disk->dll_name_rva));
    host_to_le(p_iat_rva.get_rva(),      &(disk->iat_rva));
    return disk;
}

void
SgAsmPEImportDirectory::unparse(std::ostream &f, const SgAsmPEImportSection *section) const
{
    ROSE_ASSERT(get_idx()>=0);

    SgAsmPEFileHeader *fhdr = dynamic_cast<SgAsmPEFileHeader*>(section->get_header());
    ROSE_ASSERT(fhdr!=NULL);

    if (p_dll_name_rva>0) {
        SgAsmGenericSection *name_section = p_dll_name_rva.get_section();
        if (name_section!=NULL) {
            rose_addr_t spos = name_section->write(f, p_dll_name_rva.get_rel(), p_dll_name->get_string());
            name_section->write(f, spos, '\0');
        } else {
            SgAsmPEImportSection::import_mesg("error: unable to locate section to contain "
                                              "Import Directory Name RVA 0x%08"PRIx64"\n", 
                                              p_dll_name_rva.get_rva());
        }
    }
    if (p_ilt)
        p_ilt->unparse(f, fhdr, p_ilt_rva);
    if (p_iat)
        p_iat->unparse(f, fhdr, p_iat_rva);
    
    PEImportDirectory_disk disk;
    encode(&disk);
    section->write(f, get_idx()*sizeof disk, sizeof disk, &disk);
}


/* Print debugging info */
void
SgAsmPEImportDirectory::dump(FILE *f, const char *prefix, ssize_t idx) const
{
    char p[4096];
    if (idx>=0) {
        sprintf(p, "%sPEImportDirectory[%zd].", prefix, idx);
    } else {
        sprintf(p, "%sPEImportDirectory.", prefix);
    }

	const int w = std::max(1, DUMP_FIELD_WIDTH-(int)strlen(p));

    fprintf(f, "%s%-*s = %s",          p, w, "dll_name_rva", p_dll_name_rva.to_string().c_str());
    if (p_dll_name)
        fprintf(f, " \"%s\"", p_dll_name->c_str());
    fputc('\n', f);

    fprintf(f, "%s%-*s = %s\n",          p, w, "ilt_rva", p_ilt_rva.to_string().c_str());
    fprintf(f, "%s%-*s = %s\n",          p, w, "iat_rva", p_iat_rva.to_string().c_str());
    fprintf(f, "%s%-*s = %lu %s",        p, w, "time", (unsigned long)p_time, ctime(&p_time));
    fprintf(f, "%s%-*s = 0x%08x (%u)\n", p, w, "forwarder_chain", p_forwarder_chain, p_forwarder_chain);
    if (p_ilt)
        p_ilt->dump(f, p, -1);
    if (p_iat)
        p_iat->dump(f, p, -1);
}
