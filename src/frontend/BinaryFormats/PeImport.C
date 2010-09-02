/* PE Import Directory (SgAsmPEImportSection and related classes). Normally in the ".idata" section. */
#include "sage3basic.h"
#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#include <stdarg.h>

/* Optionally prints an error/warning/info message regarding import tables. The messages are silenced after a certain amount
 * are printed. Returns true if printed; false if silenced. */
static bool
import_mesg(const char *fmt, ...)
{
    static size_t nprinted=0;
    static const size_t max_to_print=15;

    bool printed=false;
    va_list ap;
    va_start(ap, fmt);
    
    if (nprinted++ < max_to_print) {
        vfprintf(stderr, fmt, ap);
        if (nprinted==max_to_print)
            fprintf(stderr, "Subsequent import messages will be suppressed.\n");
        printed = true;
    }
    
    va_end(ap);
    return printed;
}


/* Constructor */
void
SgAsmPEImportDirectory::ctor(SgAsmPEImportSection *section, size_t idx, addr_t *idir_rva_p)
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
        if (import_mesg("SgAsmPEImportDirectory::ctor: error: in PE Import Directory entry %zu: "
                        "Name RVA starting at 0x%08"PRIx64" contains unmapped virtual address 0x%08"PRIx64"\n", 
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
            addr_t spos = name_section->write(f, p_dll_name_rva.get_rel(), p_dll_name->get_string());
            name_section->write(f, spos, '\0');
        } else {
            import_mesg("error: unable to locate section to contain Import Directory Name RVA 0x%08"PRIx64"\n", 
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
//#ifdef _MSC_VER
//	const int w = _cpp_max(1, DUMP_FIELD_WIDTH-(int)strlen(p));
//#else
	const int w = std::max(1, DUMP_FIELD_WIDTH-(int)strlen(p));
//#endif
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

/* Construct an Import Lookup Table Entry or a Lookup Address Table Entry */
void
SgAsmPEImportILTEntry::ctor(SgAsmPEImportSection *isec, uint64_t ilt_word)
{
    SgAsmPEFileHeader *fhdr = dynamic_cast<SgAsmPEFileHeader*>(isec->get_header());
    ROSE_ASSERT(fhdr!=NULL);
    p_hnt_entry = NULL;

    /* Initialize */
    p_ordinal = 0;
    p_hnt_entry_rva = 0;
    p_hnt_entry = NULL;
    p_extra_bits = 0;
    p_bound_rva = 0;

    /* Masks for different word sizes */
    uint64_t ordmask;                                           /* if bit is set then ILT Entry is an Ordinal */
    uint64_t hnrvamask = 0x7fffffff;                            /* Hint/Name RVA mask (both word sizes use 31 bits) */
    if (4==fhdr->get_word_size()) {
        ordmask = (uint64_t)1<<31;
    } else if (8==fhdr->get_word_size()) {
        ordmask = (uint64_t)1<<63;
    } else {
        throw FormatError("unsupported PE word size");
    }
    
    if (ilt_word & ordmask) {
        /* Ordinal */
        p_entry_type = ILT_ORDINAL;
        p_ordinal = ilt_word & 0xffff;
        p_extra_bits = ilt_word & ~(ordmask|0xffff);
    } else if (0!=(ilt_word & ~hnrvamask) || NULL==fhdr->get_best_section_by_va((ilt_word&hnrvamask) + fhdr->get_base_va())) {
        /* Bound address */
        p_entry_type = ILT_BOUND_RVA;
        p_bound_rva = ilt_word;
        p_bound_rva.bind(fhdr);
    } else {
        /* Hint/Name Pair RVA */
        p_entry_type = ILT_HNT_ENTRY_RVA;
        p_hnt_entry_rva = ilt_word & hnrvamask;
        p_hnt_entry_rva.bind(fhdr);
    }
}

/* Encode the PE Import Lookup Table or PE Import Address Table object into a word. */
uint64_t
SgAsmPEImportILTEntry::encode(const SgAsmPEFileHeader *fhdr) const
{
    uint64_t w = 0;

    /* Masks for different word sizes */
    uint64_t ordmask;                                           /* if bit is set then ILT Entry is an Ordinal */
    if (4==fhdr->get_word_size()) {
        ordmask = (uint64_t)1<<31;
    } else if (8==fhdr->get_word_size()) {
        ordmask = (uint64_t)1<<63;
    } else {
        throw FormatError("unsupported PE word size");
    }
    
    switch (p_entry_type) {
      case ILT_ORDINAL:
        w |= ordmask | p_extra_bits | p_ordinal;
        break;
      case ILT_BOUND_RVA:
        w |= p_bound_rva.get_rva();
        break;
      case ILT_HNT_ENTRY_RVA:
        w |= p_hnt_entry_rva.get_rva();
        break;
    }
    return w;
}

void
SgAsmPEImportILTEntry::unparse(std::ostream &f, const SgAsmPEFileHeader *fhdr, rva_t rva, size_t idx) const
{
    ROSE_ASSERT(rva.get_section()!=NULL);
    uint64_t ilt_entry_word = encode(fhdr);
    if (4==fhdr->get_word_size()) {
        uint32_t ilt_entry_disk;
        host_to_le(ilt_entry_word, &ilt_entry_disk);
        rva.get_section()->write(f, rva.get_rel()+idx*4, 4, &ilt_entry_disk);
    } else if (8==fhdr->get_word_size()) {
        uint64_t ilt_entry_disk;
        host_to_le(ilt_entry_word, &ilt_entry_disk);
        rva.get_section()->write(f, rva.get_rel()+idx*8, 8, &ilt_entry_disk);
    } else {
        throw FormatError("unsupported PE word size");
    }

    if (p_hnt_entry)
        p_hnt_entry->unparse(f, p_hnt_entry_rva);
}

/* Print debugging info for an Import Lookup Table Entry or an Import Address Table Entry */
void
SgAsmPEImportILTEntry::dump(FILE *f, const char *prefix, ssize_t idx) const
{
    char p[4096];
    if (idx>=0) {
        sprintf(p, "%sentry[%zd].", prefix, idx);
    } else {
        sprintf(p, "%sentry.", prefix);
    }
//#ifdef _MSC_VER
//    const int w = _cpp_max(1, DUMP_FIELD_WIDTH-(int)strlen(p));
//#else
    const int w = std::max(1, DUMP_FIELD_WIDTH-(int)strlen(p));
//#endif
    switch (p_entry_type) {
      case ILT_ORDINAL:
        fprintf(f, "%s%-*s = 0x%04x (%u)\n", p, w, "ordinal", p_ordinal, p_ordinal);
        break;
      case ILT_HNT_ENTRY_RVA:
        fprintf(f, "%s%-*s = %s\n", p, w, "hnt_entry_rva", p_hnt_entry_rva.to_string().c_str());
        break;
      case ILT_BOUND_RVA:
        fprintf(f, "%s%-*s = %s\n", p, w, "bound_rva", p_bound_rva.to_string().c_str());
        break;
      default:
        ROSE_ASSERT(!"PE Import Lookup Table entry type is not valid");
    }
    if (p_extra_bits)
        fprintf(f, "%s%-*s = 0x%08"PRIx64"\n", p, w, "extra_bits", p_extra_bits);
    if (p_hnt_entry)
        p_hnt_entry->dump(f, p, -1);
}

/* Constructor. rva is the address of the table and should be bound to a section (which may not necessarily be isec). This
 * C object represents one of two PE objects depending on the value of is_iat.
 *    true  => PE Import Address Table
 *    false => PE Import Lookup Table */
void
SgAsmPEImportLookupTable::ctor(SgAsmPEImportSection *isec, rva_t rva, size_t idir_idx, bool is_iat)
{
    ROSE_ASSERT(p_entries==NULL);
    p_entries = new SgAsmPEImportILTEntryList();
    p_entries->set_parent(this);
    p_is_iat = is_iat;
    const char *tname = is_iat ? "Import Address Table" : "Import Lookup Table";

    SgAsmPEFileHeader *fhdr = isSgAsmPEFileHeader(isec->get_header());
    ROSE_ASSERT(fhdr!=NULL);

    /* Read the Import Lookup (or Address) Table, an array of 32 or 64 bit values, the last of which is zero */
    if (rva.get_section()!=isec) {
        import_mesg("SgAsmPEImportSection::ctor: warning: %s RVA is outside PE Import Table\n"
                    "        Import Directory Entry #%zu\n"
                    "        %s RVA is %s\n"
                    "        PE Import Table mapped from 0x%08"PRIx64" to 0x%08"PRIx64"\n", 
                    tname, idir_idx, tname, rva.to_string().c_str(),
                    isec->get_mapped_actual_rva(), isec->get_mapped_actual_rva()+isec->get_mapped_size());
    }

    for (size_t i=0; 1; i++) {
        uint64_t ilt_entry_word=0;
        unsigned char buf[8];
        ROSE_ASSERT(fhdr->get_word_size() <= sizeof buf);
        try {
            isec->read_content(fhdr->get_loader_map(), rva.get_rva(), buf, fhdr->get_word_size());
        } catch (const MemoryMap::NotMapped &e) {
            if (import_mesg("SgAsmPEImportSection::ctor: error: in PE Import Directory entry %zu: "
                            "%s entry %zu starting at RVA 0x%08"PRIx64" contains unmapped virtual address 0x%08"PRIx64"\n",
                            idir_idx, tname, i, rva.get_rva(), e.va) &&
                e.map) {
                    fprintf(stderr, "Memory map in effect at time of error:\n");
                    e.map->dump(stderr, "    ");
            }
        }

        if (4==fhdr->get_word_size()) {
            ilt_entry_word = le_to_host(*(uint32_t*)buf);
        } else if (8==fhdr->get_word_size()) {
            ilt_entry_word = le_to_host(*(uint64_t*)buf);
        } else {
            throw FormatError("unsupported PE word size");
        }

        rva.set_rva(rva.get_rva()+fhdr->get_word_size()); /*advance to next entry of table*/
        if (0==ilt_entry_word)
            break;

        SgAsmPEImportILTEntry *ilt_entry = new SgAsmPEImportILTEntry(isec, ilt_entry_word);
        add_ilt_entry(ilt_entry);

        if (SgAsmPEImportILTEntry::ILT_HNT_ENTRY_RVA==ilt_entry->get_entry_type()) {
            SgAsmPEImportHNTEntry *hnt_entry = new SgAsmPEImportHNTEntry(isec, ilt_entry->get_hnt_entry_rva());
            ilt_entry->set_hnt_entry(hnt_entry);
            hnt_entry->set_parent(ilt_entry);
        }
    }
}

/* Adds another Import Lookup Table Entry or Import Address Table Entry to the Import Lookup Table */
void
SgAsmPEImportLookupTable::add_ilt_entry(SgAsmPEImportILTEntry *ilt_entry)
{
    ROSE_ASSERT(p_entries!=NULL);
    ROSE_ASSERT(ilt_entry);
    p_entries->set_isModified(true);
    p_entries->get_vector().push_back(ilt_entry);
    ROSE_ASSERT(p_entries->get_vector().size()>0);
    ilt_entry->set_parent(this);
}

void
SgAsmPEImportLookupTable::unparse(std::ostream &f, const SgAsmPEFileHeader *fhdr, rva_t rva) const
{
    if (rva!=0) {
        //const char *tname = p_is_iat ? "Import Address Table" : "Import Lookup Table";
        for (size_t i=0; i<p_entries->get_vector().size(); i++) {
            SgAsmPEImportILTEntry *ilt_entry = p_entries->get_vector()[i];
            try {
                ilt_entry->unparse(f, fhdr, rva, i);
            } catch (const ShortWrite&) {
                import_mesg("SgAsmPEImportLookupTable::unparse: error: ILT entry #%zu skipped (short write)\n", i);
            }
        }

        /* Zero terminated */
        uint64_t zero = 0;
        ROSE_ASSERT(fhdr->get_word_size()<=sizeof zero);
        addr_t spos = rva.get_rel() + p_entries->get_vector().size() * fhdr->get_word_size();
        rva.get_section()->write(f, spos, fhdr->get_word_size(), &zero);
    }
}

/* Print some debugging info for an Import Lookup Table or Import Address Table */
void
SgAsmPEImportLookupTable::dump(FILE *f, const char *prefix, ssize_t idx) const
{
    prefix = "    ...";
    const char *tabbr = p_is_iat ? "IAT" : "ILT";
    char p[4096];
    if (idx>=0) {
        sprintf(p, "%s%s[%zd].", prefix, tabbr, idx);
    } else {
        sprintf(p, "%s%s.", prefix, tabbr);
    }
//#ifdef _MSC_VER
//    const int w = _cpp_max(1, DUMP_FIELD_WIDTH-(int)strlen(p));
//#else
    const int w = std::max(1, DUMP_FIELD_WIDTH-(int)strlen(p));
//#endif
    fprintf(f, "%s%-*s = %zu\n", p, w, "nentries", p_entries->get_vector().size());
    for (size_t i=0; i<p_entries->get_vector().size(); i++) {
        SgAsmPEImportILTEntry *ilt_entry = p_entries->get_vector()[i];
        ilt_entry->dump(f, p, i);
    }
}

/* Constructor */
void
SgAsmPEImportHNTEntry::ctor(SgAsmPEImportSection *isec, rva_t rva)
{
    SgAsmPEFileHeader *fhdr = isSgAsmPEFileHeader(isec->get_header());
    ROSE_ASSERT(fhdr!=NULL);

    /* Hint */
    uint16_t hint_disk = 0;
    try {
        isec->read_content(fhdr->get_loader_map(), rva.get_rva(), &hint_disk, sizeof hint_disk);
    } catch (const MemoryMap::NotMapped &e) {
        if (import_mesg("SgAsmPEImportHNTEntry::ctor: warning: hint at RVA 0x%08"PRIx64
                        " contains unmapped virtual address 0x%08"PRIx64"\n", rva.get_rva(), e.va) &&
            e.map) {
            fprintf(stderr, "Memory map in effect at time of error:\n");
            e.map->dump(stderr, "    ");
        }
    }
    p_hint = le_to_host(hint_disk);
    
    /* Name */
    std::string s;
    try {
        s = isec->read_content_str(fhdr->get_loader_map(), rva.get_rva()+2);
    } catch (const MemoryMap::NotMapped &e) {
        if (import_mesg("SgAsmPEImportHNTEntry::ctor: warning: string at RVA 0x%08"PRIx64
                        " contains unmapped virtual address 0x%08"PRIx64"\n", rva.get_rva()+2, e.va) &&
            e.map) {
            fprintf(stderr, "Memory map in effect at time of error:\n");
            e.map->dump(stderr, "    ");
        }
    }
    p_name = new SgAsmBasicString(s);

    /* Padding */
    p_padding = 0;
    if (s.size()+1 % 2) {
        try {
            unsigned char byte;
            isec->read_content(fhdr->get_loader_map(), rva.get_rva()+2+s.size()+1, &byte, 1);
            p_padding = byte;
        } catch (const MemoryMap::NotMapped &e) {
            if (import_mesg("SgAsmPEImportHNTEntry::ctor: warning: padding at virtual address 0x%08"PRIx64
                            " is not mapped\n", e.va) &&
                e.map) {
                fprintf(stderr, "Memory map in effect at time of error:\n");
                e.map->dump(stderr, "    ");
            }
        }
    }
}

void
SgAsmPEImportHNTEntry::unparse(std::ostream &f, rva_t rva) const
{
    uint16_t hint_disk;
    host_to_le(p_hint, &hint_disk);
    addr_t spos = rva.get_rel();
    spos = rva.get_section()->write(f, spos, 2, &hint_disk);
    spos = rva.get_section()->write(f, spos, p_name->get_string());
    spos = rva.get_section()->write(f, spos, '\0');
    if ((p_name->get_string().size()+1) % 2)
        rva.get_section()->write(f, spos, p_padding);
}

/* Print debugging info */
void
SgAsmPEImportHNTEntry::dump(FILE *f, const char *prefix, ssize_t idx) const
{
    char p[4096];
    if (idx>=0) {
        sprintf(p, "%sHNTEntry[%zd].", prefix, idx);
    } else {
        sprintf(p, "%sHNTEntry.", prefix);
    }
//#ifdef _MSC_VER
//    const int w = _cpp_max(1, DUMP_FIELD_WIDTH-(int)strlen(p));
//#else
    const int w = std::max(1, DUMP_FIELD_WIDTH-(int)strlen(p));
//#endif
    fprintf(f, "%s%-*s = %u\t\"%s\"", p, w, "hint/name", p_hint, p_name->c_str());
    if ((p_name->get_string().size()+1)%2)
        fprintf(f, " + '\\%03o'", p_padding);
    fputc('\n', f);
}

/** Constructor for PE import data. Constructs an SgAsmPEImportSection that represents either a PE ".idata" section as defined
 *  by the PE Section Table, or a PE Import Table as described by the RVA/Size pairs at the end of the NT Optional Header. The
 *  ".idata" section and PE Import Table both have the same format, which is generally:
 * 
 * @code
 * +-------------------------------+  Starts at address zero of the .idata
 * | Import Directory Table:       |  section or PE Import Table. Each Directory
 * |   Import Directory Entry #0   |  represents one library and all  its
 * |   Import Directory Entry #1   |  associated symbols.
 * |   ...                         |
 * |   Zero-filled Directory Entry |
 * +-------------------------------+
 *
 * +-------------------------------+  One table per dynamic library, starting
 * | Import Lookup Table (ILT)     |  at arbitrary RVA specified in the
 * |   ILT Entry #0                |  Directory Table.
 * |   ILT Entry #1                |
 * |   Zero-filled ILTEntry        |
 * +-------------------------------+
 *
 * +-------------------------------+  There is no starting RVA for this table.
 * | Hint-Name Table               |  Rather, ILT Entries each contain an RVA
 * |                               |  to an entry in the Hint-Name Table.
 * +-------------------------------+
 * @endcode
 */
void
SgAsmPEImportSection::ctor()
{
    set_synthesized(true);
    set_name(new SgAsmBasicString("PE Section Table"));
    set_purpose(SP_HEADER);

    p_import_directories = new SgAsmPEImportDirectoryList();
    p_import_directories->set_parent(this);
}

SgAsmPEImportSection*
SgAsmPEImportSection::parse()
{
    SgAsmPESection::parse();

    SgAsmPEFileHeader *fhdr = isSgAsmPEFileHeader(get_header());
    ROSE_ASSERT(fhdr!=NULL);

    ROSE_ASSERT(is_mapped());
    rose_addr_t idir_rva = get_mapped_actual_rva();

    for (size_t i = 0; 1; i++) {
        /* Read idata directory entries. The list is terminated with a zero-filled entry whose idx will be negative */
        SgAsmPEImportDirectory *idir = new SgAsmPEImportDirectory(this, i, &idir_rva);
        if (idir->get_idx()<0) {
            delete idir;
            break;
        }

        rva_t rva = idir->get_dll_name_rva();
        if (rva.get_section()!=this) {
            import_mesg("SgAsmPEImportSection::ctor: warning: Name RVA is outside PE Import Table\n"
                        "        Import Directory Entry #%zu\n"
                        "        Name RVA is %s\n"
                        "        PE Import Table mapped from 0x%08"PRIx64" to 0x%08"PRIx64"\n",
                        i,
                        rva.to_string().c_str(),
                        get_mapped_actual_rva(), get_mapped_actual_rva()+get_mapped_size());
        }

        /* Import Lookup Table */
        SgAsmPEImportLookupTable *ilt = new SgAsmPEImportLookupTable(this, idir->get_ilt_rva(), i, false);
        idir->set_ilt(ilt);
        ilt->set_parent(idir);

        /* Import Address Table (same class as the Import Lookup Table) */
        SgAsmPEImportLookupTable *iat = new SgAsmPEImportLookupTable(this, idir->get_iat_rva(), i, true);
        idir->set_iat(iat);
        iat->set_parent(idir);

        add_import_directory(idir);

        /* Create the GenericDLL for this library */
        SgAsmGenericDLL *dll = new SgAsmGenericDLL(idir->get_dll_name());
        for (size_t j=0; j<ilt->get_entries()->get_vector().size(); j++) {
            SgAsmPEImportILTEntry *e = ilt->get_entries()->get_vector()[j];
            SgAsmPEImportHNTEntry *hn = e->get_hnt_entry();
            if (hn!=NULL)
                dll->add_symbol(hn->get_name()->c_str());
        }
        fhdr->add_dll(dll);
    }
    return this;
}

void
SgAsmPEImportSection::add_import_directory(SgAsmPEImportDirectory *d)
{
    ROSE_ASSERT(p_import_directories!=NULL);
    p_import_directories->set_isModified(true);
    p_import_directories->get_vector().push_back(d);
    d->set_parent(this);
}

/* Write the import section back to disk */
void
SgAsmPEImportSection::unparse(std::ostream &f) const
{
    unparse_holes(f);

    /* Import Directory Entries and all they point to (even in other sections) */
    for (size_t i=0; i<get_import_directories()->get_vector().size(); i++) {
        SgAsmPEImportDirectory *idir = get_import_directories()->get_vector()[i];
        try {
            idir->unparse(f, this);
        } catch(const ShortWrite&) {
            import_mesg("SgAsmImportSection::unparse: error: Import Directory #%zu skipped (short write)\n", i);
        }
    }

    /* Zero terminated */
    SgAsmPEImportDirectory::PEImportDirectory_disk zero;
    memset(&zero, 0, sizeof zero);
    write(f, get_import_directories()->get_vector().size()*sizeof(zero), sizeof zero, &zero);
}

/* Print debugging info */
void
SgAsmPEImportSection::dump(FILE *f, const char *prefix, ssize_t idx) const
{
    char p[4096];
    if (idx>=0) {
        sprintf(p, "%sPEImportSection[%zd].", prefix, idx);
    } else {
        sprintf(p, "%sPEImportSection.", prefix);
    }
//#ifdef _MSC_VER
//    const int w = _cpp_max(1, DUMP_FIELD_WIDTH-(int)strlen(p));
//#else
    const int w = std::max(1, DUMP_FIELD_WIDTH-(int)strlen(p));
//#endif
    SgAsmPESection::dump(f, p, -1);
    fprintf(f, "%s%-*s = %zu\n", p, w, "ndirectories", p_import_directories->get_vector().size());
    for (size_t i=0; i<p_import_directories->get_vector().size(); i++)
        p_import_directories->get_vector()[i]->dump(f, p, i);

    if (variantT() == V_SgAsmPEImportSection) //unless a base class
        hexdump(f, 0, std::string(p)+"data at ", p_data);
}
