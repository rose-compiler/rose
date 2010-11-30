/* PE Import Directory. A PE Import Section is a list of PE Import Directories. */
#include "sage3basic.h"
#define __STDC_FORMAT_MACROS
#include <inttypes.h>

/** Constructor. The constructor makes @p section the parent of this new import directory, and adds this new import
 *  directory to the import section. */
void
SgAsmPEImportDirectory::ctor(SgAsmPEImportSection *section, const std::string &dll_name)
{
    ROSE_ASSERT(section!=NULL); /* needed for parsing */
    section->add_import_directory(this);
    p_time = time(NULL);

    p_dll_name = new SgAsmBasicString(dll_name);
}

/** Parse an import directory. The import directory is parsed from the specified virtual address via the PE header's loader
 *  map. Return value is this directory entry on success, or the null pointer if the entry is all zero (which marks the end of
 *  the directory list). */
SgAsmPEImportDirectory *
SgAsmPEImportDirectory::parse(rose_addr_t va)
{
    SgAsmPEImportSection *section = SageInterface::getEnclosingNode<SgAsmPEImportSection>(this);
    ROSE_ASSERT(section!=NULL);
    SgAsmPEFileHeader *fhdr = isSgAsmPEFileHeader(section->get_header());
    ROSE_ASSERT(fhdr!=NULL);

    /* Read the import directory from disk via loader map. */
    PEImportDirectory_disk disk, zero;
    memset(&zero, 0, sizeof zero);
    size_t nread = fhdr->get_loader_map()->read(&disk, va, sizeof disk);
    if (nread!=sizeof disk) {
        SgAsmPEImportSection::import_mesg("SgAsmPEImportDirectory::parse: short read (%zu byte%s) of directory "
                                          "at va 0x%08"PRIx64" (needed %zu bytes)\n",
                                          nread, 1==nread?"":"s", va, sizeof disk);
        memset(&disk, 0, sizeof disk);
    }

    /* An all-zero entry marks the end of the list. In this case return null. */
    if (!memcmp(&disk, &zero, sizeof zero))
        return NULL;

    p_ilt_rva         = le_to_host(disk.ilt_rva);
    p_time            = le_to_host(disk.time);
    p_forwarder_chain = le_to_host(disk.forwarder_chain);
    p_dll_name_rva    = le_to_host(disk.dll_name_rva);
    p_iat_rva         = le_to_host(disk.iat_rva);

    /* Bind RVAs to best sections */
    p_ilt_rva.bind(fhdr);
    p_dll_name_rva.bind(fhdr);
    p_iat_rva.bind(fhdr);

    /* Library name */
    ROSE_ASSERT(p_dll_name!=NULL);
    std::string dll_name;
    try {
        dll_name = section->read_content_str(fhdr->get_loader_map(), p_dll_name_rva);
    } catch (const MemoryMap::NotMapped &e) {
        if (SgAsmPEImportSection::import_mesg("SgAsmPEImportDirectory::parse: short read of dll name\n"
                                              "    PE Import Directory at va 0x%08"PRIx64"\n"
                                              "    Name at %s\n"
                                              "    Contains unmapped va 0x%08"PRIx64"\n",
                                              va, p_dll_name_rva.to_string().c_str(), e.va) &&
            e.map) {
            fprintf(stderr, "Memory map in effect at time of error is:\n");
            e.map->dump(stderr, "    ");
        }
    }
    p_dll_name->set_string(dll_name);

    return this;
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

/** Allocates space for this import directory's name, import lookup table, and import address table.  The items are allocated
 *  beginning at the specified relative virtual address. Items are reallocated if they are not allocated or if they are
 *  allocated in the same section to which start_rva points (the import section). The return value is the number of bytes
 *  allocated in the import section.  Upon return, this directory's address data members are initialized with possibly new
 *  values. */
size_t
SgAsmPEImportDirectory::reallocate(rose_rva_t start_rva)
{
    rose_rva_t end_rva = start_rva;

    /* Allocate space for the name if it hasn't been allocated already. */
    if (!get_dll_name()->get_string().empty() &&
        (0==p_dll_name_rva.get_rva() || p_dll_name_rva.get_section()==end_rva.get_section())) {
        p_dll_name_rva = end_rva;
        end_rva.increment(get_dll_name()->get_string().size() + 1);
    }

    /* Allocate space for the import lookup table if it hasn't been allocated yet. */
    if (0==p_ilt_rva.get_rva() || p_ilt_rva.get_section()==end_rva.get_section()) {
        p_ilt_rva = end_rva;
        end_rva.increment(get_ilt()->reallocate(end_rva));
    }

    /* Allocate space for the import address table if it hasn't been allocated yet. */
    if (0==p_iat_rva.get_rva() || p_iat_rva.get_section()==end_rva.get_section()) {
        p_iat_rva = end_rva;
        end_rva.increment(get_iat()->reallocate(end_rva));
    }

    return end_rva.get_rva() - start_rva.get_rva();
}

void
SgAsmPEImportDirectory::unparse(std::ostream &f, const SgAsmPEImportSection *section, size_t idx) const
{
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
    section->write(f, idx*sizeof disk, sizeof disk, &disk);
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
