/* Windows PE Export Sections (SgAsmPEExportSection and related classes) */
#include <rosePublicConfig.h>
#ifdef ROSE_BUILD_BINARY_ANALYSIS_SUPPORT
#include "sage3basic.h"

#include "MemoryMap.h"
#include "Diagnostics.h"

using namespace Rose;
using namespace Rose::Diagnostics;
using namespace Rose::BinaryAnalysis;

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
        if (mlog[ERROR]) {
            mlog[ERROR] <<"SgAsmPEExportDirectory::ctor: export directory at va "
                        <<StringUtility::addrToString(section->get_mapped_actual_va())
                        <<" contains unmapped va " <<StringUtility::addrToString(e.va) <<"\n";
            if (e.map) {
                mlog[ERROR] <<"Memory map in effect at time of error:\n";
                e.map->dump(mlog[ERROR], "    ");
            }
        }
        memset(&disk, 0, sizeof disk);
    }

    /* Convert disk-format data members to native format */
    p_res1         = ByteOrder::le_to_host(disk.res1);
    p_timestamp    = ByteOrder::le_to_host(disk.timestamp);
    p_vmajor       = ByteOrder::le_to_host(disk.vmajor);
    p_vminor       = ByteOrder::le_to_host(disk.vminor);
    p_name_rva     = ByteOrder::le_to_host(disk.name_rva);       p_name_rva.set_section(section);
    p_ord_base     = ByteOrder::le_to_host(disk.ord_base);
    p_expaddr_n    = ByteOrder::le_to_host(disk.expaddr_n);
    p_nameptr_n    = ByteOrder::le_to_host(disk.nameptr_n);
    p_expaddr_rva  = ByteOrder::le_to_host(disk.expaddr_rva);    p_expaddr_rva.set_section(section);
    p_nameptr_rva  = ByteOrder::le_to_host(disk.nameptr_rva);    p_nameptr_rva.set_section(section);
    p_ordinals_rva = ByteOrder::le_to_host(disk.ordinals_rva);   p_ordinals_rva.set_section(section);

    /* Read the name */
    std::string name;
    try {
        name = section->read_content_str(fhdr->get_loader_map(), p_name_rva.get_va());
    } catch (const MemoryMap::NotMapped &e) {
        if (mlog[WARN]) {
            mlog[WARN] <<"SgAsmPEExportDirectory::ctor: directory name at rva "
                       <<StringUtility::addrToString(p_name_rva.get_rva())
                       <<" contains unmapped va " <<StringUtility::addrToString(e.va) <<"\n";
            if (e.map) {
                mlog[WARN] <<"Memory map in effect at time of error:\n";
                e.map->dump(mlog[WARN], "    ");
            }
        }
        memset(&disk, 0, sizeof disk);
    }
    p_name = new SgAsmBasicString(name);
    p_name->set_parent(this);
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

    fprintf(f, "%s%-*s = \"%s\"\n",                    p, w, "name", p_name->get_string(true).c_str());
    fprintf(f, "%s%-*s = 0x%08x (%u)\n",               p, w, "res1", p_res1, p_res1);
    fprintf(f, "%s%-*s = %lu %s",                      p, w, "timestamp", (unsigned long)p_timestamp, ctime(&p_timestamp));
    fprintf(f, "%s%-*s = %u\n",                        p, w, "vmajor", p_vmajor);
    fprintf(f, "%s%-*s = %u\n",                        p, w, "vminor", p_vminor);
    fprintf(f, "%s%-*s = %s\n",                        p, w, "name_rva", p_name_rva.to_string().c_str());
    fprintf(f, "%s%-*s = %u\n",                        p, w, "ord_base", p_ord_base);
    fprintf(f, "%s%-*s = %" PRIuPTR "\n",                       p, w, "expaddr_n", p_expaddr_n);
    fprintf(f, "%s%-*s = %" PRIuPTR "\n",                       p, w, "nameptr_n", p_nameptr_n);
    fprintf(f, "%s%-*s = %s\n",                        p, w, "expaddr_rva", p_expaddr_rva.to_string().c_str());
    fprintf(f, "%s%-*s = %s\n",                        p, w, "nameptr_rva", p_nameptr_rva.to_string().c_str());
    fprintf(f, "%s%-*s = %s\n",                        p, w, "ordinals_rva", p_ordinals_rva.to_string().c_str());
}

/* Constructor */
void
SgAsmPEExportEntry::ctor(SgAsmGenericString *fname, unsigned ordinal, rose_rva_t expaddr, SgAsmGenericString *forwarder)
{
    set_name(fname);
    if (fname)
        fname->set_parent(this);

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

    fprintf(f, "%s%-*s = [ord %u] rva=%s \"%s\"", p, w, "info",
            p_ordinal, p_export_rva.to_string().c_str(), p_name->get_string(true).c_str());
    if (p_forwarder)
        fprintf(f, " -> \"%s\"", p_forwarder->get_string(true).c_str());
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

    // Check that the p_export_dir.p_nameptr_n is not out of range.
    rose_addr_t availBytes = fhdr->get_loader_map()->at(p_export_dir->get_nameptr_rva().get_va()).available().size();
    size_t availElmts = availBytes / sizeof(ExportNamePtr_disk);
    if (p_export_dir->get_nameptr_n() > availElmts) {
        mlog[ERROR] <<"SgAsmPEExportSection::parse: number of entries indicated (" <<p_export_dir->get_nameptr_n() <<")"
                    <<" exceeds available mapped memory (room for " <<StringUtility::plural(availElmts, "entries") <<")\n";
    }
    static const size_t maxNamePtrN = 10000;
    if (p_export_dir->get_nameptr_n() > maxNamePtrN) {
        availElmts = std::min(availElmts, maxNamePtrN);
        mlog[WARN] <<"SgAsmPEExportSection::parse: number of entries indicated (" <<p_export_dir->get_nameptr_n() <<")"
                   <<" is large; resetting to " <<availElmts <<"\n";
    }

    /* The export directory points to three parallel arrays:
     *   1. An array of RVAs that point to NUL-terminated names.
     *   2. An array of "ordinals" which serve as indices into the Export Address Table.
     *   3. An array of export addresses (see note below). */
    for (size_t i=0; i<std::min(p_export_dir->get_nameptr_n(), availElmts); i++) {
        /* Function name RVA (nameptr)*/
        ExportNamePtr_disk nameptr_disk = 0;
        rose_addr_t nameptr_va = p_export_dir->get_nameptr_rva().get_va() + i*sizeof(nameptr_disk);
        bool badFunctionNameVa = false;
        try {
            read_content(fhdr->get_loader_map(), nameptr_va, &nameptr_disk, sizeof nameptr_disk);
        } catch (const MemoryMap::NotMapped &e) {
            badFunctionNameVa = true;
            if (mlog[ERROR]) {
                mlog[ERROR] <<"SgAsmPEExportSection::parse: export name #" <<i
                            <<" at va " <<StringUtility::addrToString(nameptr_va)
                            <<" contains unmapped va " <<StringUtility::addrToString(e.va) <<"\n";
                if (e.map) {
                    mlog[ERROR] <<"Memory map in effect at time of error:\n";
                    e.map->dump(mlog[ERROR], "    ");
                }
            }
            nameptr_disk = 0;
        }
        rose_addr_t fname_rva = ByteOrder::le_to_host(nameptr_disk);
        rose_addr_t fname_va = fname_rva + fhdr->get_base_va();

        /* Function name (fname) */
        std::string s;
        try {
            s = read_content_str(fhdr->get_loader_map(), fname_va);
        } catch (const MemoryMap::NotMapped &e) {
            if (mlog[ERROR]) {
                mlog[ERROR] <<"SgAsmPEExportSection::parse: export name #" <<i
                            <<" at va " <<StringUtility::addrToString(fname_va)
                            <<" contains unmapped va " <<StringUtility::addrToString(e.va) <<"\n";
                if (e.map) {
                    mlog[ERROR] <<"Memory map in effect at time of error:\n";
                    e.map->dump(mlog[ERROR], "    ");
                }
            }
        }
        SgAsmGenericString *fname = new SgAsmBasicString(s);

        /* Ordinal (sort of an index into the Export Address Table) */
        ExportOrdinal_disk ordinal_disk = 0;
        rose_addr_t ordinal_va = p_export_dir->get_ordinals_rva().get_va() + i*sizeof(ordinal_disk);
        bool badOrdinalVa = false;
        try {
            read_content(fhdr->get_loader_map(), ordinal_va, &ordinal_disk, sizeof ordinal_disk);
        } catch (const MemoryMap::NotMapped &e) {
            badOrdinalVa = true;
            if (mlog[ERROR]) {
                mlog[ERROR] <<"SgAsmPEExportSection::parse: ordinal #" <<i
                            <<" at va " <<StringUtility::addrToString(ordinal_va)
                            <<" contains unmapped va " <<StringUtility::addrToString(e.va) <<"\n";
                if (e.map) {
                    mlog[ERROR] <<"Memory map in effect at time of error:\n";
                    e.map->dump(mlog[ERROR], "    ");
                }
            }
            ordinal_disk = 0;
        }
        unsigned ordinal = ByteOrder::le_to_host(ordinal_disk);

        /* If the function name pointer and ordinal pointer are both in unmapped memory then give up. */
        if (badFunctionNameVa && badOrdinalVa) {
            if (mlog[ERROR])
                mlog[ERROR] <<"SgAsmPEExportSection::parse: giving up after trying to read entry #" <<i <<"\n";
            break;
        }

        /* Export address. Convert the symbol's Ordinal into an index into the Export Address Table. The spec says to subtract
         * the ord_base from the Ordinal to get the index, but testing has shown this to be off by one (e.g., Windows-XP file
         * /WINDOWS/system32/msacm32.dll's Export Table's first symbol has the name "XRegThunkEntry" with an Ordinal of zero
         * and the ord_base is one. The index according to spec would be -1 rather than the correct value of zero.) */
        rose_rva_t expaddr = 0;
        unsigned expaddr_idx = 0;
        if (ordinal >= (p_export_dir->get_ord_base()-1) &&
            (expaddr_idx = ordinal - (p_export_dir->get_ord_base()-1)) < p_export_dir->get_expaddr_n()) {
            ExportAddress_disk expaddr_disk = 0;
            rose_addr_t expaddr_va = p_export_dir->get_expaddr_rva().get_va() + expaddr_idx*sizeof(expaddr_disk);
            try {
                read_content(fhdr->get_loader_map(), expaddr_va, &expaddr_disk, sizeof expaddr_disk);
            } catch (const MemoryMap::NotMapped &e) {
                if (mlog[ERROR]) {
                    mlog[ERROR] <<"SgAsmPEExportSection::parse: export address #" <<i
                                <<" at va " <<StringUtility::addrToString(expaddr_va)
                                <<" contains unmapped va " <<StringUtility::addrToString(e.va) <<"\n";
                    if (e.map) {
                        mlog[ERROR] <<"Memory map in effect at time of error:\n";
                        e.map->dump(mlog[ERROR], "    ");
                    }
                }
            }
            expaddr = ByteOrder::le_to_host(expaddr_disk);
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
                s = read_content_str(fhdr->get_loader_map(), expaddr.get_va());
            } catch (const MemoryMap::NotMapped &e) {
                if (mlog[ERROR]) {
                    mlog[ERROR] <<"SgAsmPEExportSection::parse: forwarder " <<i
                                <<" at rva " <<StringUtility::addrToString(expaddr.get_rva())
                                <<" contains unmapped va " <<StringUtility::addrToString(e.va) <<"\n";
                    if (e.map) {
                        mlog[ERROR] <<"Memory map in effect at time of error:\n";
                        e.map->dump(mlog[ERROR], "    ");
                    }
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

#endif
