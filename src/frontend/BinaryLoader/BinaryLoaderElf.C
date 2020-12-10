#include <rosePublicConfig.h>
#ifdef ROSE_BUILD_BINARY_ANALYSIS_SUPPORT
#include "sage3basic.h"

#include "BinaryLoaderElf.h"
#include "Diagnostics.h"
#include "integerOps.h"                 /* needed for signExtend() */
#include "MemoryMap.h"

#include <fstream>
#include <boost/regex.hpp>
#include <boost/filesystem.hpp>

using namespace Sawyer::Message;

namespace Rose {
namespace BinaryAnalysis {

std::ostream& operator<<(std::ostream &o, const BinaryLoaderElf::VersionedSymbol &x) { x.print(o); return o; }

/* This binary loader can handle all ELF files. */
bool
BinaryLoaderElf::canLoad(SgAsmGenericHeader *hdr) const {
    return isSgAsmElfFileHeader(hdr)!=NULL;
}

/* Sorter for sections returned by get_remap_sections() */
static bool
sectionCmp(SgAsmGenericSection *_a, SgAsmGenericSection *_b) {
    SgAsmElfSection *a = isSgAsmElfSection(_a);
    SgAsmElfSection *b = isSgAsmElfSection(_b);
    ASSERT_not_null(a);
    ASSERT_not_null(b);
    SgAsmElfSegmentTableEntry *a_seg = a->get_segment_entry();
    SgAsmElfSegmentTableEntry *b_seg = b->get_segment_entry();
    if (a_seg && b_seg) {
#if 0
        /* Sort ELF Segments by position in ELF Segment Table */
        return a_seg->get_index() < b_seg->get_index();
#else
        /* Sort ELF Segments by preferred virtual address */
        return a->get_mapped_preferred_rva() < b->get_mapped_preferred_rva();
#endif
    } else if (a_seg) {
        /* ELF Segments come before ELF Sections */
        return true;
    } else {
        /* Sort ELF Sections by position in ELF Section Table */
        return a->get_id() < b->get_id();
    }
}

SgAsmGenericSectionPtrList
BinaryLoaderElf::getRemapSections(SgAsmGenericHeader *header) {
    SgAsmGenericSectionPtrList retval = header->get_mapped_sections();
    std::sort(retval.begin(), retval.end(), sectionCmp);
    return retval;
}

/* For any given file header, start mapping at a particular location in the address space. */
rose_addr_t
BinaryLoaderElf::rebase(const MemoryMap::Ptr &map, SgAsmGenericHeader *header, const SgAsmGenericSectionPtrList &sections) {
    static const size_t maximum_alignment = 8192;
    AddressInterval mappableArea = AddressInterval::whole();

#if 0 // [Robb P. Matzke 2014-10-09]
    // If this is a library then restrict where it can be mapped.  This is to try to make BinaryLoaderElf behave more like the
    // native loader on Linux (at least when run with "i386 --addr-compat-layout --addr-no-randomize), but it's a failing
    // proposition because the headers in ROSE are apparently not processed in the same order as in Linux.
    SgAsmGenericFormat *fmt = header->get_exec_format();
    if (fmt->get_purpose() == SgAsmGenericFormat::PURPOSE_LIBRARY)
        mappableArea = AddressInterval::hull(0x55555000, mappableArea.greatest());
#endif

    // Find the minimum address desired by the sections to be mapped.
    rose_addr_t min_preferred_rva = (uint64_t)(-1);
    for (SgAsmGenericSectionPtrList::const_iterator si=sections.begin(); si!=sections.end(); ++si)
        min_preferred_rva = std::min(min_preferred_rva, (*si)->get_mapped_preferred_rva());
    rose_addr_t min_preferred_va = header->get_base_va() + min_preferred_rva;

    // Minimum address at which to map
    AddressInterval freeSpace = map->unmapped(mappableArea.greatest(), Sawyer::Container::MATCH_BACKWARD);
    freeSpace = freeSpace & mappableArea;
    if (freeSpace.isEmpty())
        throw MemoryMap::NoFreeSpace("no free specimen memory", map, 0);
    rose_addr_t map_base_va = alignUp(freeSpace.least(), (rose_addr_t)maximum_alignment);

    // If the minimum preferred virtual address is less than the floor of the page-aligned mapping area, then
    // return a base address which moves the min_preferred_va to somewhere in the page pointed to by map_base_va.
    if (min_preferred_va < map_base_va) {
        size_t min_preferred_page = min_preferred_va / maximum_alignment;
        if (map_base_va < min_preferred_page * maximum_alignment)
            return 0;
        return map_base_va - min_preferred_page*maximum_alignment;
    }

    // No need to rebase
    return header->get_base_va();
}

BinaryLoader::MappingContribution
BinaryLoaderElf::alignValues(SgAsmGenericSection *_section, const MemoryMap::Ptr &map,
                             rose_addr_t *malign_lo_p, rose_addr_t *malign_hi_p,
                             rose_addr_t *va_p, rose_addr_t *mem_size_p,
                             rose_addr_t *offset_p, rose_addr_t *file_size_p, bool *map_private_p,
                             rose_addr_t *va_offset_p, bool *anon_lo_p, bool *anon_hi_p,
                             ConflictResolution *resolve_p) {
    ASSERT_not_null(_section);
    ASSERT_not_null(map);
    ASSERT_not_null(malign_lo_p);
    ASSERT_not_null(malign_hi_p);
    ASSERT_not_null(va_p);
    ASSERT_not_null(mem_size_p);
    ASSERT_not_null(offset_p);
    ASSERT_not_null(file_size_p);
    ASSERT_not_null(map_private_p);
    ASSERT_not_null(va_offset_p);
    ASSERT_not_null(anon_lo_p);
    ASSERT_not_null(anon_hi_p);
    ASSERT_not_null(resolve_p);

    SgAsmElfSection *section = isSgAsmElfSection(_section);
    ASSERT_not_null(section); /* This method is only for ELF files. */

    // Maximum alignment seems to be 4k regardless of what the ELF file says.  You can see this by running:
    //   $ x86_64 -R /bin/cat /proc/self/maps
    // and noticing that the /bin/cat segments are mapped at '4000, 'b000, and 'c000 even though the ELF file says the memory
    // alignment constraint is 0x200000.
    if (*malign_lo_p > 4096)
        *malign_lo_p = 4096;
    if (*malign_hi_p > 4096)
        *malign_hi_p = 4096;

    /* ELF Segments are aligned using the superclass, but when the section has a low- or high-padding area we'll use file
     * contents for the low area and zeros for the high area. Due to our rebase() method, there should be no conflicts between
     * this header's sections and sections previously mapped from other headers.  Any conflicts are within a single header and
     * are resolved by moving the segment to a free area. */
    if (section->get_segment_entry()) {
        MappingContribution retval = BinaryLoader::alignValues(section, map, malign_lo_p, malign_hi_p, va_p,
                                                               mem_size_p, offset_p, file_size_p, map_private_p, va_offset_p,
                                                               anon_lo_p, anon_hi_p, resolve_p);

        *anon_lo_p = false;
        *map_private_p = true;
        *resolve_p = RESOLVE_OVERMAP;

        if (section->get_mapped_size()==section->get_size()) {
            *anon_hi_p = false;
        } else {
            *anon_hi_p = true;
        }

        return retval;
    }

    /* ELF Sections are often used to refine areas of a segment.  A real loader ignores ELF Sections, but the refinement can
     * be useful for analyses.  Therefore, we'll allow ELF Sections to contribute to the mapping.  Alignment of ELF Sections
     * is temporarily set to none (because they live at arbitrary offsets in ELF Segments). If the section lives in a segment
     * that has been mapped to an address other than its preferred address, then we must make sure the ELF Section is also
     * similarly moved along with the ELF Segment. */
    SgAsmElfSection *part_of = NULL; /*what segment is this section a part of (if any)? */
    SgAsmGenericSectionPtrList sections = section->get_header()->get_mapped_sections();
    for (SgAsmGenericSectionPtrList::iterator si=sections.begin(); si!=sections.end(); ++si) {
        SgAsmElfSection *segment = isSgAsmElfSection(*si);
        if (segment && segment->get_segment_entry() &&
            section->get_mapped_preferred_rva() >= segment->get_mapped_preferred_rva() &&
            section->get_mapped_preferred_rva() < segment->get_mapped_preferred_rva() + segment->get_mapped_size()) {
            part_of = segment;
            break;
        }
    }
    rose_addr_t diff = part_of ? part_of->get_mapped_actual_va() - part_of->get_mapped_preferred_va() : 0;
    *malign_hi_p = *malign_lo_p = 1; /*no alignment constraint*/
    *va_p = section->get_header()->get_base_va() + section->get_mapped_preferred_rva() + diff;
    *mem_size_p = section->get_mapped_size();
    *offset_p = section->get_offset();
    *file_size_p = section->get_size();
    *va_offset_p = 0;
    *anon_lo_p = *anon_hi_p = true;
    *map_private_p = false;
    *resolve_p = RESOLVE_OVERMAP;       /*erase (part of) the previous ELF Segment's memory */
    return CONTRIBUTE_ADD;
}

/* class method */
void
BinaryLoaderElf::getDynamicVars(SgAsmGenericHeader *hdr, std::string &rpath/*out*/, std::string &runpath/*out*/) {
    rpath = runpath = "";

    SgAsmElfDynamicSection *dynamic = isSgAsmElfDynamicSection(hdr->get_section_by_name(".dynamic"));
    if (dynamic) {
        SgAsmElfDynamicEntryPtrList& entries = dynamic->get_entries()->get_entries();
        for (size_t i=0; i<entries.size(); ++i) {
            if (SgAsmElfDynamicEntry::DT_RPATH == entries[i]->get_d_tag()) {
                rpath = entries[i]->get_d_val().to_string();
            } else if (SgAsmElfDynamicEntry::DT_RUNPATH == entries[i]->get_d_tag()) {
                runpath = entries[i]->get_d_val().to_string();
            }
        }
    }
}

void
BinaryLoaderElf::addLibDefaults(SgAsmGenericHeader *hdr/*=NULL*/) {
    /* The LD_PRELOAD environment variable may contain space-separated library names */
    const char* ld_preload_env = getenv("LD_PRELOAD");
    if (ld_preload_env) {
        std::string s = ld_preload_env;
        boost::regex re;
        re.assign("\\s+");
        boost::sregex_token_iterator iter(s.begin(), s.end(), re, -1);
        boost::sregex_token_iterator iterEnd;
        for (; iter!=iterEnd; ++iter)
            preloads().push_back(*iter);
    }

    /* Add the DT_RPATH directories to the search path (only if no DT_RUNPATH). Use of DT_RPATH is deprecated. */
    std::string rpath, runpath;
    if (hdr)
        getDynamicVars(hdr, rpath, runpath);
    if (!rpath.empty() && runpath.empty()) {
        boost::regex re;
        re.assign("[:;]");
        boost::sregex_token_iterator iter(rpath.begin(), rpath.end(), re, -1);
        boost::sregex_token_iterator iterEnd;
        for (; iter!=iterEnd; ++iter)
            directories().push_back(*iter);
    }

    /* Add the paths from the LD_LIBRARY_PATH environment variable */
    const char *ld_library_path_env = getenv("LD_LIBRARY_PATH");
    if (ld_library_path_env) {
        std::string s = ld_library_path_env;
        boost::regex re;
        re.assign("[:;]");
        boost::sregex_token_iterator iter(s.begin(), s.end(), re, -1);
        boost::sregex_token_iterator iterEnd;
        for (; iter!=iterEnd; ++iter)
            directories().push_back(*iter);
    }

    /* Add paths from the .dynamic DT_RUNPATH variable */
    if (!runpath.empty()) {
        boost::regex re;
        re.assign("[:;]");
        boost::sregex_token_iterator iter(runpath.begin(), runpath.end(), re, -1);
        boost::sregex_token_iterator iterEnd;
        for (; iter!=iterEnd; ++iter)
            directories().push_back(*iter);
    }

    /* Add architecture-specific libraries */
    if (hdr) {
        switch (hdr->get_isa() & SgAsmGenericHeader::ISA_FAMILY_MASK) {
            case SgAsmGenericHeader::ISA_X8664_Family:
                directories().push_back("/lib64");
                directories().push_back("/usr/lib64");
                break;
            case SgAsmGenericHeader::ISA_IA32_Family:
                directories().push_back("/lib32");
                directories().push_back("/usr/lib32");
                break;
            default:
                /*none*/
                break;
        }
    }

    /* Add system library locations */
    directories().push_back("/lib");
    directories().push_back("/usr/lib");
}

void
BinaryLoaderElf::buildMasterSymbolTable(SgAsmInterpretation *interp) {
    typedef std::vector<std::pair<SgAsmGenericSection*, SymbolMap*> > SymbolMapList;
    Stream trace(mlog[TRACE]);
    trace.facilityName("BinaryLoaderElf");

    /* Build a mapping (symbolMaps) from each ".dynsym" section (one per file header) to a SymbolMap. The SymbolMap
     * holds the symbols defined (i.e., not local, hidden, or only referenced) by the section. */
    SgAsmGenericHeaderPtrList& headers = interp->get_headers()->get_headers();
    SymbolMapList symbolMaps; /*maps each .dynsym section to a SymbolMap::BaseMap object. */
    for (size_t h=0; h < headers.size(); ++h) {
        SgAsmGenericHeader* header = headers[h];
        trace <<"building symbol map for " <<header->get_file()->get_name() <<"...\n";
        SymverResolver versionResolver(header);

        /* There must be zero or one ELF Section named ".dynsym" under this header.  If the section does not exist then the
         * header is not dynamically linked and neither provides nor requires dynamic symbol resolution.
         *
         * Manual 1-10 under SHT_SYMTAB and SHT_DYNSYM: ...SHT_DYNSYM section holds a minimal set of dynamic linking symbols.
         * FIXME: Technically, the dynsym may be omitted, and we should truly use the .dynamic section's DT_SYMTAB entry(ies) */
        SgAsmElfSymbolSection *dynsym = isSgAsmElfSymbolSection(header->get_section_by_name(".dynsym"));
        if (!dynsym)
            continue;
        ASSERT_not_null(dynsym->get_section_entry());
        ASSERT_require(SgAsmElfSectionTableEntry::SHT_DYNSYM == dynsym->get_section_entry()->get_sh_type());

        /* Add each symbol definition to the SymbolMap and then add the SymbolMap to the symbolMaps */
        SymbolMap* symbolMap = new SymbolMap;
        for (size_t symbol_idx=0; symbol_idx<dynsym->get_symbols()->get_symbols().size(); symbol_idx++) {
            if (0==symbol_idx) continue; /*this must be undefined, so we can skip it*/

            SgAsmElfSymbol* symbol = dynsym->get_symbols()->get_symbols()[symbol_idx];
            trace <<"  symbol [" <<symbol_idx <<"] \"" <<symbol->get_name()->get_string(true) <<"\" ";

            VersionedSymbol symver = versionResolver.getVersionedSymbol(symbol);
// I think we need all symbols in the symbol table for relocations. [RPM 2010-09-16]
//            if (symver.is_local()) {
//                /* Symbols in the dynsym should never be local (if this is isn't the case, we can ignore them). Symbol
//                 * versioning also may make symbols "local". */
//                trace <<" local (ignoring)\n";
//            } else if (symver.is_hidden()) {
//                /* Symbol versioning can result in 'hidden' symbols that should be ignored. */
//                trace <<" hidden (ignoring)\n";
//            } else
            if (symver.isReference()) {
                /* Symbol versioning lets us determine which symbols are 'references' i.e.  that are only used to help
                 * relocating */
                trace <<" reference (ignoring)\n";
            } else {
                std::string symName = symver.getName();
                ASSERT_require(symName==symbol->get_name()->get_string());
                SymbolMapEntry &symbolEntry = (*symbolMap)[symName];
                symbolEntry.addVersion(symver);
                trace <<" added.\n";
            }
        }
        symbolMaps.push_back(std::make_pair(dynsym, symbolMap));
    }

    /* Initialize the p_symbols master symbol table by merging all the definitions from the symbolMaps we created above. */
    trace <<"building master symbol table...\n";
    symbols_.clear();
    for (SymbolMapList::const_iterator smi=symbolMaps.begin(); smi!=symbolMaps.end(); ++smi) {
        const SymbolMap *symbolMap = smi->second;
        for(SymbolMap::const_iterator newSymbolIter=symbolMap->begin(); newSymbolIter!=symbolMap->end(); ++newSymbolIter) {
            const SymbolMapEntry &newEntry = newSymbolIter->second;
            const std::string symName = newSymbolIter->first;
            SymbolMap::iterator oldSymbolIter=symbols_.find(symName);

            if (oldSymbolIter == symbols_.end()) {
                /* no symbol yet, set it (this is the most common case) */
                symbols_[symName] = newSymbolIter->second;
            } else {
                /* We have to go through version by version to 'merge' a complete map */
                oldSymbolIter->second.merge(newEntry);
            }
        }
    }
    if (trace) {
        trace <<"master symbol table:\n";
        symbols_.dump(stderr, "  ");                   // FIXME[Robb Matzke 2014-01-27]: should be able to pass "trace"
    }
}

/* Reference Elf TIS Portal Formats Specification, Version 1.1 */
// DQ (10/15/2015): C++11 "override" keyword is not allowed in function definitions (gnu 4.8.3 using -std=c++11 mode).
// void BinaryLoaderElf::fixup(SgAsmInterpretation *interp, FixupErrors *errors) ROSE_OVERRIDE
void
BinaryLoaderElf::fixup(SgAsmInterpretation *interp, FixupErrors *errors) {
    SgAsmGenericHeaderPtrList& headers = interp->get_headers()->get_headers();
    buildMasterSymbolTable(interp);

    mlog[TRACE] <<"BinaryLoaderElf: performing relocation fixups...\n";
    for (size_t h=0; h<headers.size(); ++h) {
        SgAsmElfFileHeader* elfHeader = isSgAsmElfFileHeader(headers[h]);
        ASSERT_not_null(elfHeader);
        if (errors!=NULL) {
            try {
                performRelocations(elfHeader, interp->get_map());
            } catch (const Exception &e) {
                errors->push_back(e);
            }
        } else {
            performRelocations(elfHeader, interp->get_map());
        }
    }
}

SgAsmGenericSection *
BinaryLoaderElf::findSectionByPreferredVa(SgAsmGenericHeader* header, rose_addr_t va) {
    SgAsmGenericSection *retval = NULL;
    SgAsmGenericSectionPtrList sections = header->get_sections_by_va(va, true);
    for (SgAsmGenericSectionPtrList::iterator si=sections.begin(); si!=sections.end(); ++si) {
        SgAsmElfSection *elf_section = isSgAsmElfSection(*si);
        if (elf_section && elf_section->get_section_entry()) {
            if ((elf_section->get_section_entry()->get_sh_flags() & SgAsmElfSectionTableEntry::SHF_TLS) &&
                elf_section->get_section_entry()->get_sh_type() == SgAsmElfSectionTableEntry::SHT_NOBITS) {
                /* TODO: handle .tbss correctly */
            } else if (retval != NULL) {
                using namespace Rose::StringUtility;
                mlog[ERROR] <<"find_section_by_preferred_va: multiple sections match " <<addrToString(va) <<"\n";
                mlog[ERROR] <<"  section at " <<addrToString(retval->get_mapped_actual_va())
                            <<" + " <<addrToString(retval->get_mapped_size())
                            <<" = " <<addrToString(retval->get_mapped_actual_va() + retval->get_mapped_size())
                            <<" " <<cEscape(retval->get_name()->get_string()) <<"\n";
                mlog[ERROR] <<"  section at " <<addrToString(elf_section->get_mapped_actual_va())
                            <<" + " <<addrToString(elf_section->get_mapped_size())
                            <<" = " <<addrToString(elf_section->get_mapped_actual_va() + elf_section->get_mapped_size())
                            <<" " <<cEscape(elf_section->get_name()->get_string()) <<"\n";
            } else {
                ASSERT_require2(retval==NULL, "there should be only one matching section");
                retval = elf_section;
            }
        }
    }
    return retval;
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// BinaryLoaderElf::VersionedSymbol
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool
BinaryLoaderElf::VersionedSymbol::isLocal() const {
    if (versionEntry_ && versionEntry_->get_value() == 0) {
        return true;
    } else if (symbol_->SgAsmElfSymbol::STB_LOCAL == symbol_->get_elf_binding()) {
        return true;
    }
    return false;
}

bool
BinaryLoaderElf::VersionedSymbol::isHidden() const {
    if (versionEntry_ && versionEntry_->get_value() & VERSYM_HIDDEN)
        return true;
    return false;
}

bool
BinaryLoaderElf::VersionedSymbol::isReference() const {
    return NULL!=versionNeed_ || NULL==symbol()->get_bound();
}

bool
BinaryLoaderElf::VersionedSymbol::isBaseDefinition() const {
    if (NULL == versionEntry_)
         return true; /* unversioned entries are always considered "base" */
    if (NULL == versionDef_)
         return false; /* if it's not a definition, then it's clearly not a base definition */
    if (versionDef_->get_flags() & VER_FLG_BASE)
        return true;
    return false;
}

std::string
BinaryLoaderElf::VersionedSymbol::getVersion() const {
    if (versionDef_)
        return versionDef_->get_entries()->get_entries().front()->get_name()->get_string();
    if (versionNeed_)
        return versionNeed_->get_name()->get_string();
    return std::string();
}

std::string
BinaryLoaderElf::VersionedSymbol::getVersionedName() const {
    std::string name = getName();
    if (isHidden())
        name += "[HIDDEN]";
    if (versionDef_) {
        if (versionDef_->get_flags() & VER_FLG_BASE)
            name += "[BASE]";

        SgAsmElfSymverDefinedAuxPtrList& entries=versionDef_->get_entries()->get_entries();
        name += entries.front()->get_name()->get_string();
        if (entries.size() > 1) {
            name += ";";
            for (size_t i=1;i < entries.size(); ++i) {
                name += " ";
                name += entries[i]->get_name()->get_string();
            }
        }
    }
    if (versionNeed_) {
        if (versionNeed_->get_flags() & VER_FLG_WEAK)
            name += "[WEAK]";
        name += versionNeed_->get_name()->get_string();
    }
    return name;
}

void
BinaryLoaderElf::VersionedSymbol::dump(FILE *f, const char *prefix, ssize_t idx) const {
    char p[4096];
    if (idx>=0) {
        sprintf(p, "%sVersionedSymbol[%zd].", prefix, idx);
    } else {
        sprintf(p, "%sVersionedSymbol.", prefix);
    }
    const int w = std::max(1, DUMP_FIELD_WIDTH-(int)strlen(p));
    fprintf(f, "%s%-*s = \"%s\"\n", p, w, "name", getName().c_str());
    fprintf(f, "%s%-*s = \"%s\"\n", p, w, "version", getVersion().c_str());
    fprintf(f, "%s%-*s =%s%s%s%s\n", p, w, "attributes",
            isLocal()?" local":"",
            isHidden()?" hidden":"",
            isReference()?" reference":"",
            isBaseDefinition()?" basedef":"");
    if (symbol_)
        symbol_->dump(f, p, -1);
    if (versionEntry_)
        versionEntry_->dump(f, p, -1);
    if (versionDef_)
        versionDef_->dump(f, p, -1);
    if (versionNeed_)
        versionNeed_->dump(f, p, -1);
}

void
BinaryLoaderElf::VersionedSymbol::print(std::ostream &o) const {
    o <<"<" <<getVersionedName() <<"> "
      <<(isLocal() ? " local" : "")
      <<(isHidden() ? " hidden" : "")
      <<(isReference() ? " reference" : "")
      <<(isBaseDefinition() ? " base" : "");
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// BinaryLoaderElf::SymbolMapEntry
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

BinaryLoaderElf::VersionedSymbol
BinaryLoaderElf::SymbolMapEntry::getVSymbol(const VersionedSymbol &version) const {
    if (NULL == version.versionNeed())
        return getBaseVersion();

    // Look for a return value that has the correct version.
    std::string neededVersion = version.versionNeed()->get_name()->get_string();
    for (size_t i=0; i<versions_.size(); ++i) {
        if (SgAsmElfSymverDefinedEntry *def = versions_[i].versionDef()) {
            const SgAsmElfSymverDefinedAuxPtrList &defaux_list = def->get_entries()->get_entries();
            ASSERT_require(!defaux_list.empty() && defaux_list[0]!=NULL);
            if (0==defaux_list[0]->get_name()->get_string().compare(neededVersion))
                return versions_[i];
        }
    }

    // If the defined symbols don't have versions, then return them instead.
    return getBaseVersion();
}

void
BinaryLoaderElf::SymbolMapEntry::addVersion(const VersionedSymbol& vsymbol) {
#ifndef _MSC_VER
    if (vsymbol.isBaseDefinition()) {
        // There can be only one "base" version, so if one already exists, move it to the end of the list (vector) and insert
        // our new base version at the beginning.
        if (!versions_.empty()) {
            ASSERT_require(versions_.empty() || false == getVSymbol().isBaseDefinition());
            versions_.push_back(versions_.front());
            versions_[0] = vsymbol;
        } else {
            versions_.push_back(vsymbol);
        }
    } else {
        versions_.push_back(vsymbol);
    }
#endif
}

void
BinaryLoaderElf::SymbolMapEntry::merge(const SymbolMapEntry& newEntry) {
    VersionedSymbol oldSymbol = versions_.front();
    VersionedSymbol newSymbol = newEntry.versions_.front();
    int start=0;

    /* Merge base definitions. */
    if (oldSymbol.isBaseDefinition() && newSymbol.isBaseDefinition()) {
        if (oldSymbol.symbol()->get_elf_binding() == SgAsmElfSymbol::STB_WEAK &&
            newSymbol.symbol()->get_elf_binding() == SgAsmElfSymbol::STB_GLOBAL) {
            /* the new symbol becomes the new base */
            versions_[0] = newSymbol;
            start=1;
            if (!oldSymbol.getVersion().empty() && oldSymbol.getVersion() != newSymbol.getVersion()) {
                /* The old symbol was overridden, but it still has a unique version - so we need to keep it. */
                versions_.push_back(oldSymbol);
            }
        }
    }

    /* Merge remaining versions. */
    for (size_t i=start; i<newEntry.versions_.size(); ++i) {
        newSymbol = newEntry.versions_[i];
        std::string newVersion=newSymbol.getVersion();
        if (newVersion.empty()) {
            /* If the version has no version then it should have been the base entry, in which case it would have been handled
             * above.  Since it isn't, this is some 'weaker' non-versioned entry, and it can be dropped. */
            continue;
        }
        size_t found=false;
        for (size_t j=0; j<versions_.size(); ++j) {
            oldSymbol = versions_[j];
            if (oldSymbol.getVersion() == newSymbol.getVersion() &&                     /* matching version string */
                oldSymbol.symbol()->get_elf_binding() == SgAsmElfSymbol::STB_WEAK &&  /* old is weak */
                newSymbol.symbol()->get_elf_binding() == SgAsmElfSymbol::STB_GLOBAL) {/* new is strong */
                versions_[j] = newSymbol; /* override the old symbol */
                found=true;
                break;
            }
        }
        if (!found) {
            /* This version doesn't exist in this entry, so append it to the list. */
            versions_.push_back(newSymbol);
        }
    }
}

void
BinaryLoaderElf::SymbolMapEntry::dump(FILE *f, const char *prefix) const {
    for (size_t i=0; i<versions_.size(); ++i) {
        const VersionedSymbol &vsym = versions_[i];
        vsym.dump(f, prefix, i);
    }
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// BinaryLoaderElf::SymbolMap
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

const BinaryLoaderElf::SymbolMapEntry *
BinaryLoaderElf::SymbolMap::lookup(std::string name) const {
    const_iterator iter = find(name);
    if (end() ==iter)
        return NULL;
    return &(iter->second);
}

const BinaryLoaderElf::SymbolMapEntry *
BinaryLoaderElf::SymbolMap::lookup(std::string name, std::string version) const {
    if (version.empty()) {
        return lookup(name);
    } else {
        return lookup(name + "("  + version + ")");
    }
}

void
BinaryLoaderElf::SymbolMap::dump(FILE *f, const char *prefix) const {
    for (const_iterator i=begin(); i!=end(); ++i) {
        std::string p = std::string(prefix) + "SymbolMap[" + i->first + "].";
        i->second.dump(f, p.c_str());
    }
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// BinaryLoaderElf::SymverResolver
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
BinaryLoaderElf::SymverResolver::ctor(SgAsmGenericHeader* header) {
    /* Locate the .dynsym, .gnu.version, .gnu.version_d, and/or .gnu_version_r sections. We could have done this with
     * header->get_section_by_name(), but this is possibly more reliable. */
    SgAsmElfSymbolSection* dynsym=NULL;
    SgAsmElfSymverSection* symver=NULL;
    SgAsmElfSymverDefinedSection* symver_def=NULL;
    SgAsmElfSymverNeededSection* symver_need=NULL;
    SgAsmGenericSectionPtrList& sections = header->get_sections()->get_sections();
    for (size_t sec=0; sec < sections.size(); ++sec) {
        SgAsmGenericSection* section = sections[sec];
        if (isSgAsmElfSymbolSection(section) && isSgAsmElfSymbolSection(section)->get_is_dynamic()) {
            dynsym = isSgAsmElfSymbolSection(section);
        } else if(isSgAsmElfSymverSection(section)) {
            symver = isSgAsmElfSymverSection(section);
        } else if(isSgAsmElfSymverDefinedSection(section)) {
            symver_def = isSgAsmElfSymverDefinedSection(section);
        } else if(isSgAsmElfSymverNeededSection(section)) {
            symver_need=isSgAsmElfSymverNeededSection(section);
        }
    }

    /* Build maps */
    if (symver_def)
        makeSymbolVersionDefMap(symver_def);    /* for p_symbolVersionDefMap */
    if (symver_need)
        makeSymbolVersionNeedMap(symver_need);  /* for p_symbolVersionNeedMap */
    makeVersionedSymbolMap(dynsym, symver);     /* for p_versionedSymbolMap */
}

void
BinaryLoaderElf::SymverResolver::makeSymbolVersionDefMap(SgAsmElfSymverDefinedSection* section) {
    ASSERT_not_null(section);
    SgAsmElfSymverDefinedEntryPtrList& defs = section->get_entries()->get_entries();
    for (size_t def=0; def < defs.size(); ++def) {
        SgAsmElfSymverDefinedEntry* defEntry =  defs[def];
        symbolVersionDefMap_[defEntry->get_index()]=defEntry;
    }
}

void
BinaryLoaderElf::SymverResolver::makeSymbolVersionNeedMap(SgAsmElfSymverNeededSection* section) {
    ASSERT_not_null(section);
    SgAsmElfSymverNeededEntryPtrList& needs = section->get_entries()->get_entries();
    for (size_t need=0; need < needs.size(); ++need) {
        SgAsmElfSymverNeededEntry* needEntry = needs[need];
        SgAsmElfSymverNeededAuxPtrList& auxes = needEntry->get_entries()->get_entries();
        for (size_t aux=0; aux < auxes.size(); ++aux) {
            SgAsmElfSymverNeededAux* auxEntry = auxes[aux];
            symbolVersionNeedMap_[auxEntry->get_other()] = auxEntry;
        }
    }
}

void
BinaryLoaderElf::SymverResolver::makeVersionedSymbolMap(SgAsmElfSymbolSection* dynsym, SgAsmElfSymverSection* symver/*=NULL*/) {
    ASSERT_require(dynsym && dynsym->get_is_dynamic());
    SgAsmElfSymbolPtrList& symbols = dynsym->get_symbols()->get_symbols();

    /* symverSection may be NULL, but if it isn't, it must have the same number of entries as dynsym */
    ASSERT_require(NULL == symver || symbols.size() == symver->get_entries()->get_entries().size());

    for (size_t symbol_idx=0; symbol_idx<symbols.size(); symbol_idx++) {
        SgAsmElfSymbol *symbol = symbols[symbol_idx];

        ASSERT_require(versionedSymbolMap_.end() == versionedSymbolMap_.find(symbol));
        VersionedSymbol* versionedSymbol = new VersionedSymbol(symbol);
        versionedSymbolMap_.insert(std::make_pair(symbol,versionedSymbol));

        if (symver) {
            SgAsmElfSymverEntry *symverEntry = symver->get_entries()->get_entries()[symbol_idx];
            uint16_t value = symverEntry->get_value();

            /* From the Sun Microsystem "Linker and Libraries Guide" April 2008, page 254:
             *
             *     STV_HIDDEN: A symbol that is defined in the current component is hidden if its name is not visible in other
             *                 components. Such a symbol is necessarily protected. This attribute is used to control the
             *                 external interface of a component. An object named by such a symbol can still be referenced
             *                 from another component if its address is passed outside.  A hidden symbol contained in a
             *                 relocatable object is either removed or converted to STB_LOCAL binding when the object is
             *                 included in an executable file or shared object.
             *
             * Therefore, we will also treat this symbol as local. */
            if (value & VERSYM_HIDDEN)
                value = 0;

            versionedSymbol->versionEntry(symverEntry);

            /* 0 and 1 are special (local and global respectively) they DO NOT correspond to entries in the ELF Symbol
             * Version Definition Table or the ELF Symbol Version Requirements Table. Otherwise the value should exist in
             * exactly one of those tables. */
            if (0 != value && 1 != value) {
                SymbolVersionDefinitionMap::const_iterator defIter = symbolVersionDefMap_.find(value);
                SymbolVersionNeededMap::const_iterator needIter = symbolVersionNeedMap_.find(value);
                /* We must have a match from defs or needs, not both, not neither. */
                ASSERT_require((symbolVersionDefMap_.end() == defIter) != (symbolVersionNeedMap_.end() == needIter));
                if (symbolVersionDefMap_.end() != defIter) {
                    versionedSymbol->versionDef(defIter->second);
                } else if (symbolVersionNeedMap_.end() != needIter) {
                    versionedSymbol->versionNeed(needIter->second);
                }
            }
        }
    }
}

BinaryLoaderElf::VersionedSymbol
BinaryLoaderElf::SymverResolver::getVersionedSymbol(SgAsmElfSymbol* symbol) const {
    /* If we don't actually have versioned symbols, return the identity version. */
    if (versionedSymbolMap_.empty())
        return VersionedSymbol(symbol);

    /* We should have every symbol that might get looked up in here */
    VersionedSymbolMap::const_iterator iter = versionedSymbolMap_.find(symbol);
    ASSERT_require(versionedSymbolMap_.end() != iter);
    return *(iter->second);
}

void
BinaryLoaderElf::SymverResolver::dump(FILE *f, const char *prefix, ssize_t idx) const {
    char p[4096];
    if (idx>=0) {
        sprintf(p, "%sSymverResolver[%zd].", prefix, idx);
    } else {
        sprintf(p, "%sSymverResolver.", prefix);
    }

    for (SymbolVersionDefinitionMap::const_iterator si=symbolVersionDefMap_.begin(); si!=symbolVersionDefMap_.end(); ++si) {
        uint16_t id = si->first;
        SgAsmElfSymverDefinedEntry *def = si->second;
        def->dump(f, p, id);
    }
    for (SymbolVersionNeededMap::const_iterator si=symbolVersionNeedMap_.begin(); si!=symbolVersionNeedMap_.end(); ++si) {
        uint16_t id = si->first;
        SgAsmElfSymverNeededAux *need = si->second;
        need->dump(f, p, id);
    }
    size_t i=0;
    for (VersionedSymbolMap::const_iterator si=versionedSymbolMap_.begin(); si!=versionedSymbolMap_.end(); ++si, ++i) {
        SgAsmElfSymbol *sym = si->first;
        VersionedSymbol *vsym = si->second;
        std::string pp = std::string(p)+"versionedSymbol["+StringUtility::numberToString(i)+"].";
        ASSERT_always_require(sym == vsym->symbol());
        vsym->dump(f, pp.c_str(), -1); // sym is printed as part of this
    }
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Relocation fixup information methods. Names all begin with "fixupInfo".
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

SgAsmElfSymbol *
BinaryLoaderElf::fixupInfoRelocSymbol(SgAsmElfRelocEntry *reloc, const SymverResolver &resolver) {
    Stream trace(mlog[TRACE]);

    ASSERT_not_null(reloc);
    if (0==reloc->get_sym()) {
        trace <<"    reloc symbol: <none>\n";
        return NULL;
    }

    /* Look up symbol referenced by the relocation entry */
    SgAsmElfSection *reloc_section = SageInterface::getEnclosingNode<SgAsmElfSection>(reloc);
    ASSERT_not_null(reloc_section);
    SgAsmElfSymbolSection* symbol_section = isSgAsmElfSymbolSection(reloc_section->get_linked_section());
    ASSERT_not_null(symbol_section);
    ASSERT_require(reloc->get_sym() < symbol_section->get_symbols()->get_symbols().size());
    SgAsmElfSymbol *reloc_sym = symbol_section->get_symbols()->get_symbols()[reloc->get_sym()];
    ASSERT_not_null(reloc_sym);
    VersionedSymbol reloc_vsym = resolver.getVersionedSymbol(reloc_sym);
    trace <<"    reloc symbol: " <<reloc_vsym.getVersionedName() <<"\n";
    bool is_weak = reloc_sym->get_elf_binding() == SgAsmElfSymbol::STB_WEAK;
    bool is_local = reloc_vsym.isHidden() || reloc_vsym.isLocal();

    /* Find the defining versioned symbol associated with the relocation symbol. */
    SgAsmElfSymbol *retval = NULL;
    std::string symbol_name = reloc_vsym.getName();
    if (const SymbolMapEntry *symbol_entry = symbols_.lookup(symbol_name)) {
        VersionedSymbol source_vsym = symbol_entry->getVSymbol(reloc_vsym);
        if (source_vsym.isReference()) {
            trace <<"    reference symbol is being treated as undefined\n";
        } else if (is_local && reloc_vsym.getSection()->get_header()!=source_vsym.getSection()->get_header()) {
            trace <<"    hidden/local symbol is being treated as undefined\n";
        } else {
            trace <<"    using definition " <<source_vsym.getVersionedName() <<"\n";
            retval = source_vsym.symbol();
            ASSERT_not_null(retval);
            ASSERT_require(0 != retval->get_st_shndx());/* test an assumption [MCB] */
        }
    }

    /* Handle case when defined symbol doesn't exist (or we made it not to exist because it's local). */
    if (!retval) {
        if (!is_weak) {
            trace <<"    could not find symbol in master symbol table\n";
            throw Exception(symbol_name + " not defined in master symbol table");
        }
        return NULL; /*weak symbol with no definition*/
    }

    return retval;
}

rose_addr_t
BinaryLoaderElf::fixupInfoTargetVa(SgAsmElfRelocEntry *reloc, SgAsmGenericSection **section_p, rose_addr_t *adj_p) {
    Stream trace(mlog[TRACE]);
    SgAsmGenericHeader *header = SageInterface::getEnclosingNode<SgAsmGenericHeader>(reloc);
    SgAsmGenericSection *section = findSectionByPreferredVa(header, reloc->get_r_offset());
    if (!section) {
        trace <<"    target: no suitable section at preferred va " <<StringUtility::addrToString(reloc->get_r_offset()) <<"\n";
        throw Exception("reloc target " + StringUtility::addrToString(reloc->get_r_offset()) + " is not mapped");
    }

    rose_addr_t target_adj = section->get_mapped_actual_va() - section->get_mapped_preferred_va();
    rose_addr_t target_va = reloc->get_r_offset() + target_adj;

    if (trace) {
        trace <<"    target: file=\"" <<header->get_file()->get_name() <<"\"\n";
        trace <<"            section=[" <<section->get_id() <<"] \"" <<section->get_name()->get_string(true) <<"\"\n";
        trace <<"            preferred=" <<StringUtility::addrToString(section->get_mapped_preferred_va())
              <<", actual=" <<StringUtility::addrToString(section->get_mapped_actual_va()) <<"\n";
        trace <<"            va " <<StringUtility::addrToString(reloc->get_r_offset()) <<" + adjustment "
              <<StringUtility::addrToString(target_adj) <<" = " <<StringUtility::addrToString(target_va) <<"\n";
    }

    if (section_p)
        *section_p = section;
    if (adj_p)
        *adj_p = target_adj;
    return target_va;
}

rose_addr_t
BinaryLoaderElf::fixupInfoSymbolVa(SgAsmElfSymbol *symbol, SgAsmGenericSection **section_p, rose_addr_t *adj_p) {
    Stream trace(mlog[TRACE]);

    if (!symbol) {
        trace <<"    symbol: no symbol; possibly weak with no definition\n";
        if (section_p)
            *section_p = NULL;
        if (adj_p)
            *adj_p = 0;
        return 0;
    }

    SgAsmGenericHeader *header = SageInterface::getEnclosingNode<SgAsmGenericHeader>(symbol);
    SgAsmGenericSection *section = findSectionByPreferredVa(header, symbol->get_value());
    if (!section) {
        trace <<"    symbol: no suitable section at preferred va " <<StringUtility::addrToString(symbol->get_value()) <<"\n";
        throw Exception("symbol value " + StringUtility::addrToString(symbol->get_value()) + " is not mapped");
    }

    rose_addr_t symbol_adj = section->get_mapped_actual_va() - section->get_mapped_preferred_va();
    rose_addr_t symbol_va = symbol->get_value() + symbol_adj;
    if (trace) {
        trace <<"    symbol: file=\"" <<header->get_file()->get_name() <<"\"\n";
        trace <<"            section=[" <<section->get_id() <<"] \"" <<section->get_name()->get_string(true) <<"\"\n";
        trace <<"            preferred=" <<StringUtility::addrToString(section->get_mapped_preferred_va())
              <<", actual=" <<StringUtility::addrToString(section->get_mapped_actual_va()) <<"\n";
        trace <<"            value " <<StringUtility::addrToString(symbol->get_value())
              <<" + adjustment " <<StringUtility::addrToString(symbol_adj)
              <<" = " <<StringUtility::addrToString(symbol_va) <<"\n";
    }

    if (section_p)
        *section_p = section;
    if (adj_p)
        *adj_p = symbol_adj;
    return symbol_va;
}

rose_addr_t
BinaryLoaderElf::fixupInfoAddend(SgAsmElfRelocEntry *reloc, rose_addr_t target_va, const MemoryMap::Ptr &memmap,
                                 size_t nbytes) {
    Stream trace(mlog[TRACE]);

    SgAsmElfRelocSection *reloc_section = SageInterface::getEnclosingNode<SgAsmElfRelocSection>(reloc);
    ASSERT_not_null(reloc_section);
    if (0==nbytes)
        nbytes = reloc_section->get_header()->get_word_size();

    if (reloc_section->get_uses_addend()) {
        trace <<"    addend from reloc is " <<StringUtility::addrToString(reloc->get_r_addend()) <<"\n";
        return reloc->get_r_addend();
    }

    trace <<"    reading " <<nbytes <<"-byte addend from memory at " <<StringUtility::addrToString(target_va) <<"\n";
    rose_addr_t retval = 0;
    ByteOrder::Endianness sex = reloc_section->get_header()->get_sex();
    ASSERT_not_null(memmap);
    switch (nbytes) {
        case 4: {
            uint32_t guest;
            size_t nread = memmap->readQuick(&guest, target_va, sizeof guest);
            if (nread<sizeof guest) {
                trace <<"    short read of relocation addend at " <<StringUtility::addrToString(target_va) <<"\n";
                throw Exception("short read of relocation addend at " + StringUtility::addrToString(target_va));
            }
            retval = ByteOrder::disk_to_host(sex, guest);
            break;
        }
        case 8: {
            uint64_t guest;
            size_t nread = memmap->readQuick(&guest, target_va, sizeof guest);
            if (nread<sizeof guest) {
                trace <<"    short read of relocation addend at " <<StringUtility::addrToString(target_va) <<"\n";
                throw Exception("short read of relocation addend at " + StringUtility::addrToString(target_va));
            }
            retval = ByteOrder::disk_to_host(sex, guest);
            break;
        }
        default:
            ASSERT_not_implemented("only 4 and 8 byte sizes are handled");
    }

    trace <<"    addend from memory is " <<StringUtility::addrToString(retval) <<"\n";
    return retval;
}

rose_addr_t
BinaryLoaderElf::fixupInfoExpr(const std::string &expression, SgAsmElfRelocEntry *reloc, const SymverResolver &resolver,
                               const MemoryMap::Ptr &memmap, rose_addr_t *target_va_p) {
    std::vector<rose_addr_t> stack;
    SgAsmElfSymbol *symbol = NULL;                      /* Defining symbol for relocation */
    size_t nbytes = 0;                                  /* Size of addend */
    rose_addr_t target_va = fixupInfoTargetVa(reloc);
    if (target_va_p)
        *target_va_p = target_va;

    for (std::string::size_type i=0; i<expression.size(); i++) {
        switch (expression[i]) {
            case '0':
            case '4':
            case '8':
                nbytes = expression[i] - '0';
                break;

            case 'A': {                                 /* Addend of relocation from reloc entry or specimen memory */
                rose_addr_t addend = fixupInfoAddend(reloc, target_va, memmap, nbytes);
                stack.push_back(addend);
                break;
            }
            case 'B': {                                 /* Adjustment for defining symbol's section */
                if (!symbol)
                    symbol = fixupInfoRelocSymbol(reloc, resolver);
                rose_addr_t symbol_adj;
                fixupInfoSymbolVa(symbol, NULL, &symbol_adj);
                stack.push_back(symbol_adj);
                break;
            }
            case 'S': {                                 /* Value of defining symbol for relocation */
                if (!symbol)
                    symbol = fixupInfoRelocSymbol(reloc, resolver);
                rose_addr_t symbol_va = fixupInfoSymbolVa(symbol);
                stack.push_back(symbol_va);
                break;
            }
            case '+': {                                 /* Addition of two top stack items */
                ASSERT_require(stack.size()>=2);
                rose_addr_t opand1 = stack.back(); stack.pop_back();
                rose_addr_t opand2 = stack.back(); stack.pop_back();
                stack.push_back(opand1 + opand2);
                break;
            }
            default:
                ASSERT_not_implemented(std::string("fixup info expression = '") + expression[i] + "'");
        }
    }

    ASSERT_require(1==stack.size());
    return stack[0];
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Methods that apply relocation fixups. Their names all begin with "fixupApply".
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
BinaryLoaderElf::fixupApply(rose_addr_t value, SgAsmElfRelocEntry *reloc, const MemoryMap::Ptr &memmap,
                            rose_addr_t target_va/*=0*/, size_t nbytes/*=0*/) {
    Stream trace(mlog[TRACE]);

    SgAsmGenericHeader *header = SageInterface::getEnclosingNode<SgAsmGenericHeader>(reloc);
    ASSERT_not_null(header);
    ByteOrder::Endianness sex = header->get_sex();

    if (0==target_va)
        target_va = fixupInfoTargetVa(reloc);
    if (0==nbytes)
        nbytes = header->get_word_size();

    trace <<"    writing " <<StringUtility::addrToString(value) <<" (" <<StringUtility::plural(nbytes, "bytes") <<")"
          <<" to address " <<StringUtility::addrToString(target_va) <<"\n";
    switch (nbytes) {
        case 4: {
            uint32_t guest;
            ByteOrder::host_to_disk(sex, value, &guest);
            size_t nwrite = memmap->writeQuick(&guest, target_va, sizeof guest);
            if (nwrite<sizeof guest) {
                trace <<"    short write (only " <<StringUtility::plural(nwrite, "bytes") <<")\n";
                throw Exception("short write at " + StringUtility::addrToString(target_va));
            }
            break;
        }
        case 8: {
            uint64_t guest;
            ByteOrder::host_to_disk(sex, value, &guest);
            size_t nwrite = memmap->writeQuick(&guest, target_va, sizeof guest);
            if (nwrite<sizeof guest) {
                trace <<"    short write (only " <<StringUtility::plural(nwrite, "bytes") <<")\n";
                throw Exception("short write at " + StringUtility::addrToString(target_va));
            }
            break;
        }
        default:
            ASSERT_not_implemented("only 4 and 8 byte sizes are implemented");
    }
}


void
BinaryLoaderElf::fixupApplySymbolCopy(SgAsmElfRelocEntry* reloc, const SymverResolver &resolver,
                                      const MemoryMap::Ptr &memmap) {
    Stream trace(mlog[TRACE]);
    SgAsmElfSymbol *symbol = fixupInfoRelocSymbol(reloc, resolver);
    rose_addr_t target_va = fixupInfoTargetVa(reloc);
    rose_addr_t symbol_va = fixupInfoSymbolVa(symbol);
    size_t symbol_sz = symbol->get_size();

    trace <<"    copying " <<StringUtility::plural(symbol_sz, "bytes")
          <<" from " <<StringUtility::addrToString(symbol_va) <<" to " <<StringUtility::addrToString(target_va) <<"\n";
    while (symbol_sz>0) {
        uint8_t buf[4096];
        size_t nbytes = std::min(symbol_sz, sizeof buf);

        size_t nread = memmap->readQuick(buf, symbol_va, nbytes);
        if (nread<nbytes) {
            trace <<"    short read (only " <<StringUtility::plural(nread, "bytes")
                  <<" but expected " <<nbytes <<") at " <<StringUtility::addrToString(symbol_va) <<"\n";
            throw Exception("short read at " + StringUtility::addrToString(symbol_va));
        }

        size_t nwrite = memmap->writeQuick(buf, target_va, nbytes);
        if (nwrite<nbytes) {
            trace <<"    short write (only " <<StringUtility::plural(nwrite, "bytes")
                  <<" but expected " <<nbytes <<" at " <<StringUtility::addrToString(target_va) <<"\n";
            throw Exception("short write at " + StringUtility::addrToString(target_va));
        }

        symbol_sz -= nbytes;
        symbol_va += nbytes;
        target_va += nbytes;
    }
}



/*************************************************************************************************************************
 * Low-level ELF stuff and lots of commented out code begins here and continues to the end of the file.  I've moved this stuff
 * from the BinaryLoader_ElfSupport name space to class methods in BinaryLoaderElf, but have not had a chance to review these
 * functions yet. [RPM 2010-09-14]
 *************************************************************************************************************************/



/**
    Relocation of Jump Table Entry (e.g. jump slot).
    A call to a library looks like the following :
      .text: [EXECUTABLE READ ONLY]
        ...
A       call 0x000002080 <file relative address in plt>
F       ...

      .plt [EXECUTABLE READ ONLY]
        ...
B 0x2080 jmp *03030 <file relative address in .got.plt>
C 0x2086 push 0x4 <This is the 4th function in the plt>
D 0x208b jmp *0x00002018 <basically a call to dynamic linker>

      .got.plt [NOT EXECUTABLE WRITABLE]
        ...
Z 0x3030 : 0x2086



LIBRARY :
   .text:
     ...
E 0xA018: FOO <do function stuff here>
  ...
  ret


In the above example, in "normal" operation, we would see:
The FIRST time we call the "foo"
A
B <jumps to *Z, which starts pointing at C>
C
D <change Z to the value of E: 0xA018, call Foo directly>
E <execute Foo>
F

The SECOND time, we will have called the dynamic linker, and C and D are no inert
A
B <jumps to *Z which is NOW just E>
E <execute Foo>
F

In this relocation, we need to mimic the dynamic linker, and overwrite the
.got entry (Z) with the address of the function (E i.e. Foo)
Thus, we're performing
*Z = E

*/



void
BinaryLoaderElf::performRelocation(SgAsmElfRelocEntry* reloc, const SymverResolver &resolver,
                                   const MemoryMap::Ptr &memmap) {
    Stream trace(mlog[TRACE]);
    ASSERT_not_null2(reloc, "ELF relocation entry");
    SgAsmElfRelocSection *parentSection = SageInterface::getEnclosingNode<SgAsmElfRelocSection>(reloc);
    ASSERT_not_null2(parentSection, "section containing ELF relocation entry");
    ASSERT_not_null2(memmap, "memory map");
    SgAsmGenericHeader* header = parentSection->get_header();
    ASSERT_not_null2(header, "ELF file header for relocation entry");
    SgAsmGenericHeader::InsSetArchitecture isa = header->get_isa();

    SgAsmElfSymbolSection* linkedSymbolSection = isSgAsmElfSymbolSection(parentSection->get_linked_section());
    ASSERT_not_null2(linkedSymbolSection, "linked ELF section for relocation entry");
    ASSERT_require(reloc->get_sym() < linkedSymbolSection->get_symbols()->get_symbols().size());
    SgAsmElfSymbol* relocSymbol = linkedSymbolSection->get_symbols()->get_symbols()[reloc->get_sym()];
    ASSERT_not_null2(relocSymbol, "relocation symbol");
    trace <<"  " <<StringUtility::addrToString(reloc->get_r_offset()) <<" " <<reloc->reloc_name()
          <<" for \"" <<relocSymbol->get_name()->get_string(true) <<"\"\n";

    rose_addr_t target_va = 0;
    switch (isa & SgAsmGenericHeader::ISA_FAMILY_MASK) {
        case SgAsmGenericHeader::ISA_IA32_Family:
            switch (reloc->get_type()) {
                case SgAsmElfRelocEntry::R_386_JMP_SLOT:
                case SgAsmElfRelocEntry::R_386_GLOB_DAT: {
                    rose_addr_t value = fixupInfoExpr("S", reloc, resolver, memmap, &target_va);
                    fixupApply(value, reloc, memmap, target_va, 4);
                    break;
                }
                case SgAsmElfRelocEntry::R_386_COPY: {
                    fixupApplySymbolCopy(reloc, resolver, memmap);
                    break;
                }
                case SgAsmElfRelocEntry::R_386_RELATIVE: {
                    rose_addr_t value = fixupInfoExpr("4BA+", reloc, resolver, memmap, &target_va);
                    fixupApply(value, reloc, memmap, target_va, 4);
                    break;
                }
                case SgAsmElfRelocEntry::R_386_32: {
                    rose_addr_t value = fixupInfoExpr("4SA+", reloc, resolver, memmap, &target_va);
                    fixupApply(value, reloc, memmap, target_va, 4);
                    break;
                }
                case SgAsmElfRelocEntry::R_386_TLS_TPOFF:
                case SgAsmElfRelocEntry::R_386_TLS_IE:
                case SgAsmElfRelocEntry::R_386_TLS_GOTIE:
                case SgAsmElfRelocEntry::R_386_TLS_LE:
                case SgAsmElfRelocEntry::R_386_TLS_GD:
                case SgAsmElfRelocEntry::R_386_TLS_LDM:
                case SgAsmElfRelocEntry::R_386_TLS_GD_32:
                case SgAsmElfRelocEntry::R_386_TLS_GD_PUSH:
                case SgAsmElfRelocEntry::R_386_TLS_GD_CALL:
                case SgAsmElfRelocEntry::R_386_TLS_GD_POP:
                case SgAsmElfRelocEntry::R_386_TLS_LDM_32:
                case SgAsmElfRelocEntry::R_386_TLS_LDM_PUSH:
                case SgAsmElfRelocEntry::R_386_TLS_LDM_CALL:
                case SgAsmElfRelocEntry::R_386_TLS_LDM_POP:
                case SgAsmElfRelocEntry::R_386_TLS_LDO_32:
                case SgAsmElfRelocEntry::R_386_TLS_IE_32:
                case SgAsmElfRelocEntry::R_386_TLS_LE_32:
                case SgAsmElfRelocEntry::R_386_TLS_DTPMOD32:
                case SgAsmElfRelocEntry::R_386_TLS_DTPOFF32:
                case SgAsmElfRelocEntry::R_386_TLS_TPOFF32: {
                    trace <<"    thread local storage not supported\n";
                    throw Exception("relocation " + reloc->reloc_name() + " not supported");
                    break;
                }
                default: {
                    trace <<"    not implemented\n";
                    throw Exception("relocation " + reloc->reloc_name() + " not implemented");
                    break;
                }
            };
            break;

        case SgAsmGenericHeader::ISA_X8664_Family:
            switch (reloc->get_type()) {
                case SgAsmElfRelocEntry::R_X86_64_JUMP_SLOT:
                case SgAsmElfRelocEntry::R_X86_64_GLOB_DAT: {
                    rose_addr_t value = fixupInfoExpr("S", reloc, resolver, memmap, &target_va);
                    fixupApply(value, reloc, memmap, target_va, 8);
                    break;
                }
                case SgAsmElfRelocEntry::R_X86_64_32: {
                    /* FIXME: Not sure if this is correct.  Are both the addend and result only 32 bits? [RPM 2010-09-16] */
                    rose_addr_t value = fixupInfoExpr("4SA+", reloc, resolver, memmap, &target_va);
                    if (value > 0xffffffff) {
                        trace <<"    value exceeds 32-bit range\n";
                        throw Exception("value exceeds 32-bit range");
                    }
                    fixupApply(value, reloc, memmap, target_va, 4);
                    break;
                }
                case SgAsmElfRelocEntry::R_X86_64_32S: {
                    /* FIXME: Not sure if this is correct. Why would we need to sign extend to 64 bits if we're only
                     *        writing 32 bits back to memory? [RPM 2010-09-16] */
                    rose_addr_t value = fixupInfoExpr("4SA+", reloc, resolver, memmap, &target_va);
                    value = IntegerOps::signExtend<32, 64>(value);
                    fixupApply(value, reloc, memmap, target_va, 4);
                    break;
                }
                case SgAsmElfRelocEntry::R_X86_64_64: {
                    rose_addr_t value = fixupInfoExpr("8SA+", reloc, resolver, memmap, &target_va);
                    fixupApply(value, reloc, memmap, target_va, 8);
                    break;
                }
                case SgAsmElfRelocEntry::R_X86_64_RELATIVE: {
                    rose_addr_t value = fixupInfoExpr("8BA+", reloc, resolver, memmap, &target_va);
                    fixupApply(value, reloc, memmap, target_va, 8);
                    break;
                }
                default:
                    trace <<"    not implemented\n";
                    throw Exception("relocation " + reloc->reloc_name() + " not implemented");
                    break;
            }
            break;

        default:
            trace <<"    not implemented\n";
            throw Exception("relocation " + reloc->reloc_name() + " not implemented");
    }
}

void
BinaryLoaderElf::performRelocations(SgAsmElfFileHeader* elfHeader, const MemoryMap::Ptr &memmap) {
    SymverResolver resolver(elfHeader);
    SgAsmGenericSectionPtrList sections = elfHeader->get_sectab_sections();
    for (size_t sec=0; sec < sections.size(); ++sec) {
        SgAsmElfRelocSection* relocSection = isSgAsmElfRelocSection(sections[sec]);
        if (NULL == relocSection)
            continue;

        SgAsmElfRelocEntryPtrList &relocs = relocSection->get_entries()->get_entries();
        for (size_t r=0; r <  relocs.size(); ++r) {
            SgAsmElfRelocEntry* reloc = relocs[r];
            performRelocation(reloc, resolver, memmap);
        }
    }
}

// #if 0
// VersionedSymbol makeVersionedSymbol(SgAsmElfSymbol* symbol)
// {
//   VersionedSymbol vsymbol(symbol);
//   SgAsmElfSymverSection* symver=NULL;
//   SgAsmElfSymverDefinedSection* symver_def=NULL;
//   SgAsmElfSymverNeededSection* symver_need=NULL;
//
//   SgAsmElfFileHeader* header = SageInterface::getEnclosingNode<SgAsmElfFileHeader>(symbol);
//   if(header == NULL){
//     // somehow free floating - just return it
//     return vsymbol;
//   }
//   SgAsmElfSymbolSection* dynsym = SageInterface::getEnclosingNode<SgAsmElfSymbolSection>(symbol);
//   ROSE_ASSERT(NULL != dynsym);
//   if(false == dynsym->get_is_dynamic()){
//     // only the dynamic symbol table contains versioned symbols
//     return vsymbol;
//   }
//
//   SgAsmGenericSectionPtrList& sections = header->get_sections()->get_sections();
//   for(size_t sec=0; sec < sections.size(); ++sec){
//     SgAsmGenericSection* section = sections[sec];
//     if(isSgAsmElfSymverSection(section)){
//       symver = isSgAsmElfSymverSection(section);
//     }
//     else if(isSgAsmElfSymverDefinedSection(section)){
//       symver_def = isSgAsmElfSymverDefinedSection(section);
//     }
//     else if(isSgAsmElfSymverDefinedSection(section)){
//       symver_need = isSgAsmElfSymverNeededSection(section);
//     }
//   }
//   if(NULL == symver){
//     // symbol versioning not present here, return unversioned
//     return vsymbol;
//   }
//
// }
// #endif

    /*
    // 1. Get section map (name -> list<section*>)
    getSectionNameMap(binaryFile,allSectionsMap,loadableSectionsMap);

    // 2. Create Symbol map from relevant sections (.dynsym)
    SymbolMap symbolMap;
    getSymbolMap(allSectionsMap,symbolMap);

    // 3. Create Extent sorted list of loadable sections
    SgAsmElfSectionPtrList extentSortedSections;
    sortSections(loadableSectionsMap,extentSortedSections);

    // 4. Collect Relocation Entries.
    RelocationEntryList relocations;
    getRelocationEntries(binaryFile, relocations);

    // 5.  for each relocation entry, perform relocation
    processRelocations(relocations,symbolMap,extentSortedSections);
    */



/* This is a big section of commented out code. I'm prefixing all lines with "//" to make it more obvious. [RPM 2010-08-31] */
//
// typedef std::map<string,SgAsmElfSectionPtrList> SectionNameMap;
//
// class ElfSectionCollector : public AstSimpleProcessing
// {
// public:
//   SectionNameMap mAllSections;
//   SectionNameMap mLoadableSections;
//
//   virtual void visit(SgNode* node)
//   {
//     SgAsmElfSection* elfSection = isSgAsmElfSection(node);
//     if(elfSection == NULL)
//       return; // not an elf section
//
//     // TODO we shouldn't really be using 'name' as a designator, rather, we should use sectionEntry flags
//     string name = elfSection->get_name()->c_str();
//     SgAsmElfSectionPtrList& allEntryList = mAllSections[name];
//     if(!allEntryList.empty() && elfSection == allEntryList.back()){
//       // we're getting duplicates in visit
//       return;
//     }
//
//     allEntryList.push_back(elfSection);
//
//     bool isLoadable=true;
//     const SgAsmElfSectionTableEntry* sectionEntry = elfSection->get_section_entry();
//     if(sectionEntry) {
//       const uint64_t flags = sectionEntry->get_sh_flags();
//       if(!(flags & SgAsmElfSectionTableEntry::SHF_ALLOC )){
//      // Elf lets us completely skip loading a section if its not SHF_ALLOC
//      isLoadable=false;
//       }
//       if((flags & SgAsmElfSectionTableEntry::SHF_TLS) &&
//          sectionEntry->get_sh_type() == SgAsmElfSectionTableEntry::SHT_NOBITS){ // .tbss
//         //TODO support Thread Local Storage.  In particular, .tbss gives us
//      //  issues because it occupies zero disk and zero LOGICAL space
//      isLoadable=false;
//       }
//     }
//     if(isLoadable){
//       mLoadableSections[name].push_back(elfSection);
//     }
//   }
// };
//
// void getSectionNameMap(SgBinaryComposite* binaryFile,
//                        SectionNameMap& allSections,
//                     SectionNameMap& loadedSections)
// {
//   ElfSectionCollector visitor;
//   visitor.traverse(binaryFile,preorder);
//   allSections = visitor.mAllSections;
//   loadedSections = visitor.mLoadableSections;
// }
//
//
// struct ElfSymbolMapEntry
// {
//   ElfSymbolMapEntry() : symbol(NULL){};
// public:
//   rose_addr_t get_va() const
//   { return symbol->get_value() + section->get_base_va();}
//
//   SgAsmElfSymbol* symbol;
//   SgAsmGenericSection* section;
// };
//
// /// map from symbol name to a symbol entry
// typedef Rose_STL_Container<ElfSymbolMapEntry> ElfSymbolMapEntryList;
// typedef std::map<string, ElfSymbolMapEntryList> SymbolMap;
//
//
// class SymbolCollector : public AstSimpleProcessing
// {
// public:
//   SymbolMap _symbolMap;
//   virtual void visit(SgNode* node)
//   {
//     if (isSgAsmElfSymbol(node)){
//       SgAsmElfSymbol* symbol = isSgAsmElfSymbol(node);
//       SgAsmGenericHeader* header = SageInterface::getEnclosingNode<SgAsmGenericHeader>(symbol);
//       string symbolName = symbol->get_name()->c_str();
//       if(symbolName.empty()){
//      return;
//       }
//       ElfSymbolMapEntryList& entryList = _symbolMap[symbolName];
//       if(header == NULL){
//      return;
//       }
//       SgAsmGenericSectionPtrList sections = header->get_sections_by_va(symbol->get_value());
//
//       ROSE_ASSERT(!sections.empty());
//       SgAsmGenericSectionPtrList::iterator iter = sections.begin();
//       for(; iter != sections.end(); ++iter){
//      SgAsmGenericSection* section = *iter;
//      SgAsmElfSection* elfSection = isSgAsmElfSection(section);
//
//      // TODO document this
//      if((!elfSection) ||
//            (elfSection->get_section_entry() &&
//             !(elfSection->get_section_entry()->get_sh_flags() & SgAsmElfSectionTableEntry::SHF_ALLOC)))
//      {
//        continue;
//      }
//
//      ElfSymbolMapEntry addr;
//      addr.section = *iter;
//      addr.symbol = symbol;
//      entryList.push_back(addr);
//       }
//     }
//   }
// };
//
//
// void getSymbolMap( const SectionNameMap &allSectionsMap,SymbolMap &symbolMap)
// {
//
//   SectionNameMap::const_iterator findIter=allSectionsMap.find(".dynsym");
//   ROSE_ASSERT(findIter != allSectionsMap.end());// must have at least one dynamic symbol table
//
//   const SgAsmElfSectionPtrList& dynsymSections = findIter->second;
//   SymbolCollector symbolCollector;
//   {
//     SgAsmElfSectionPtrList::const_iterator sectionIter=dynsymSections.begin();
//     for(; sectionIter != dynsymSections.end(); ++sectionIter){
//       symbolCollector.traverse(*sectionIter,preorder);
//     }
//   }
//   symbolMap = symbolCollector._symbolMap;
// }
//
// bool extentSorterActualVA(const SgAsmGenericSection* lhs,const SgAsmGenericSection* rhs)
// {
//   return // sorted by start of extent, then by length
//     (std::make_pair(lhs->get_mapped_actual_va(),
//                  lhs->get_mapped_size()))
//     <
//     (std::make_pair(rhs->get_mapped_actual_va(),
//                  rhs->get_mapped_size()));
// }
//
// bool extentSorterPreferedRVA(const SgAsmGenericSection* lhs,const SgAsmGenericSection* rhs)
// {
//   return // sorted by start of extent, then by length
//     (std::make_pair(lhs->get_mapped_actual_rva(),
//                  lhs->get_mapped_size()))
//     <
//     (std::make_pair(rhs->get_mapped_actual_rva(),
//                  rhs->get_mapped_size()));
// }
//
// void sortSections(const SectionNameMap& sectionMap,SgAsmElfSectionPtrList& sortedSections)
// {
//   SectionNameMap::const_iterator sectionIter=sectionMap.begin();
//
//   for( ;sectionIter!=sectionMap.end();++sectionIter){
//     sortedSections.insert(sortedSections.end(),sectionIter->second.begin(),sectionIter->second.end());
//   }
//   std::sort(sortedSections.begin(),sortedSections.end(),extentSorterActualVA);
//   sortedSections.erase(std::unique(sortedSections.begin(), sortedSections.end()), sortedSections.end());
// }
// struct RelocationEntry
// {
//   SgAsmElfRelocEntry* reloc;
//   SgAsmElfSymbol* symbol;
// };
//
// typedef Rose_STL_Container<RelocationEntry> RelocationEntryList;
// typedef Rose_STL_Container<SgAsmElfRelocSection*> SgAsmElfRelocSectionPtrList;
// void getRelocationEntries(SgBinaryComposite *binaryFile,RelocationEntryList& relocs)
// {
// #if 0
//
//   SgAsmElfRelocSectionPtrList relocSections =
//     SageInterface::querySubTree<SgAsmElfRelocSection>(binaryFile,V_SgAsmElfRelocSection);
//
//   SgAsmElfRelocSectionPtrList::const_iterator sectionIter = relocSections.begin();
//   for(;sectionIter != relocSections.end();++sectionIter){
//     SgAsmElfRelocSection* section = *sectionIter;
//
//     SgAsmElfSymbolSection *symtab=isSgAsmElfSymbolSection(section->get_linked_section());
//     ROSE_ASSERT(symtab != NULL);
//
//     SgAsmElfRelocEntryPtrList& entries = section->get_entries()->get_entries();
//     SgAsmElfRelocEntryPtrList::iterator iter = entries.begin();
//     for(; iter != entries.end(); ++iter)
//     {
//       SgAsmElfRelocEntry* reloc = *iter;
//       RelocationEntry entry;
//       entry.reloc = reloc;
//       entry.symbol = symtab->get_symbols()->get_symbols()[reloc->get_sym()];
//       relocs.push_back(entry);
//     }
//   }
// #endif
// }
//
// string sectionString(SgAsmGenericSection* section)
// {
//   string str;
//   SgAsmGenericFile* file = SageInterface::getEnclosingNode<SgAsmGenericFile>(section);
//   if(file)
//     str += file->get_name();
//   else
//     str += "<none>";
//
//   str += "[";
//   str += section->get_name()->c_str();
//   str += "]";
//   return str;
// }
//
// // TODO use sh_type instead of section name
// ElfSymbolMapEntry
// findSection(const ElfSymbolMapEntryList& symbolEntry,
//          const string& sectionName)
// {
//   ElfSymbolMapEntryList::const_iterator iter = symbolEntry.begin();
//   for(; iter != symbolEntry.end(); ++iter){
//     if(iter->section->get_name()->c_str() == sectionName)
//       return *iter;
//   }
//   return ElfSymbolMapEntry();
// }
//
// SgAsmElfSectionPtrList sectionsOfAddress(rose_addr_t va_addr,
//                                          const SgAsmElfSectionPtrList &extentSortedSections)
// {
//   SgAsmElfSectionPtrList returnSections;
//   for(size_t i=0; i < extentSortedSections.size(); ++i)
//   {
//     SgAsmElfSection* section = extentSortedSections[i];
//     if(va_addr >= section->get_mapped_actual_va() &&
//        va_addr < (section->get_mapped_actual_va() + section->get_mapped_size()))
//     {
//       returnSections.push_back(section);
//     }
//   }
//   return returnSections;
// }
//
// SgAsmElfSection* chooseSectionAtAddress(rose_addr_t va_addr,
//                                      const SgAsmElfSectionPtrList &extentSortedSections)
// {
//   // we always prefer sections to segments (both of which are SgAsmElfSection's
//   SgAsmElfSection* segment=NULL;
//   for(size_t i=0; i < extentSortedSections.size(); ++i)
//   {
//     SgAsmElfSection* section = extentSortedSections[i];
//     if(va_addr >= section->get_mapped_actual_va() &&
//        va_addr < (section->get_mapped_actual_va() + section->get_mapped_size()))
//     {
//       if(section->get_section_entry())
//      return section;
//       else
//      segment = section;
//     }
//   }
//   return segment;
// }
//
// // TODO use sh_type instead of sectionName
// SgAsmElfSection*
// findElfSection(const SgAsmElfSectionPtrList& sections,
//                const string& sectionName)
// {
//   for(size_t i=0; i < sections.size(); ++i){
//     if(sections[i]->get_name()->c_str() == sectionName)
//       return sections[i];
//   }
//   return NULL;
// }
//
// ElfSymbolMapEntryList removeSegments(const ElfSymbolMapEntryList &symbolEntry)
// {
//   ElfSymbolMapEntryList returnList;
//   ElfSymbolMapEntryList::const_iterator iter = symbolEntry.begin();
//   for(; iter != symbolEntry.end(); ++iter){
//     SgAsmElfSection* section = isSgAsmElfSection(iter->section);
//     if(section && section->get_section_entry() != NULL)
//     {
//       // have to be careful here, some section DO have a segment_entry (like .dynamic)
//       returnList.push_back(*iter);
//     }
//   }
//   return returnList;
// }
//
// SgAsmElfSectionPtrList removeSegments(const SgAsmElfSectionPtrList &extentSortedSections)
// {
//   SgAsmElfSectionPtrList returnSections;
//   for(size_t i=0; i < extentSortedSections.size(); ++i)
//   {
//     SgAsmElfSection* section = extentSortedSections[i];
//     if(section->get_section_entry() != NULL)
//     {
//       // have to be careful here, some section DO have a segment_entry (like .dynamic)
//       returnSections.push_back(section);
//     }
//   }
//   return returnSections;
// }
//
//
// string stringOfSectionsOfAddress(rose_addr_t va_addr,
//                                    const SgAsmElfSectionPtrList& extentSortedSections)
// {
//   std::vector<SgAsmElfSection*> sections = sectionsOfAddress(va_addr,extentSortedSections);
//   SgAsmElfSection* bestSection = NULL;
//
//   for(size_t i=0; i < sections.size(); ++i)
//   {
//     SgAsmElfSection* section = sections[i];
//     if(bestSection == NULL || bestSection->get_segment_entry())
//       bestSection = section;
//   }
//   if(bestSection == NULL)
//   {
//     return "<unknown>";
//   }
//   else
//   {
//     return sectionString(bestSection);
//   }
// }
//
// void printSymbolMapEntry(const ElfSymbolMapEntryList& entries,
//                          const SgAsmElfSectionPtrList& extentSortedSections)
// {
//   printf("'%s':\n", entries.front().symbol->get_name()->c_str());
//
//   for(size_t i=0; i < entries.size(); ++i){
//     const ElfSymbolMapEntry& entry = entries[i];
//     rose_addr_t symbol_va = entry.get_va();
//     printf(" %01d 0x%016x %s\n", i, symbol_va, sectionString(entry.section).c_str());
//     printf("   %s\n",stringOfSectionsOfAddress(symbol_va,extentSortedSections).c_str());
//   }
// }
//
// void
// printSymbolMap(const SymbolMap& symbolMap,
//                const SgAsmElfSectionPtrList &extentSortedSections)
// {
//   SymbolMap::const_iterator iter = symbolMap.begin();
//   SymbolMap::const_iterator iterEnd = symbolMap.end();
//   for(; iter != iterEnd; ++iterEnd){
//     const ElfSymbolMapEntryList &entry = iter->second;
//     printSymbolMapEntry(entry,extentSortedSections);
//   }
// }
//
// ElfSymbolMapEntry
// BinaryLoaderElf::chooseSymbol(const ElfSymbolMapEntryList& entries, SgAsmElfSectionTableEntry::SectionType sh_type)
// {
//     ROSE_ASSERT(!entries.empty());
//     ElfSymbolMapEntry best = ElfSymbolMapEntry();
//     bool bestGlobalBinding = false;
//     bool bestIsSection = false;
//     bool checkSectionType = (sh_type != SgAsmElfSectionTableEntry::SHT_NULL); /* is the section type is an important filter? */
//
//     for (size_t i=0; i<entries.size(); ++i) {
//         SgAsmElfSection* section = isSgAsmElfSection(entries[i].section);
//         SgAsmElfSectionTableEntry* sectionEntry = NULL;
//         if (section)
//             sectionEntry = section->get_section_entry();
//
//         if (checkSectionType && (!sectionEntry || sectionEntry->get_sh_type() != sh_type)) {
//             /* We're asked to check the section type, but it is not a section or it's the wrong type, so we just drop it
//              * completely. */
//             continue;
//         }
//
//         bool globalBinding = entries[i].symbol->get_elf_binding() == SgAsmElfSymbol::STB_GLOBAL;
//         bool isSection = (sectionEntry != NULL);/* sections must have a section entry */
//
//         if (bestGlobalBinding == globalBinding && (bestIsSection || !isSection)) {
//             /* If the global binding is the same and we are not a section (and best IS), we lose. */
//             continue;
//         } else if (bestGlobalBinding && !globalBinding) {
//             /* The current best is globally bound, and we are not, we lose */
//             continue;
//         } else {
//             /* we are globally bound and/or we are a section */
//             bestGlobalBinding = globalBinding;
//             bestIsSection = isSection;
//             best = entries[i];
//         }
//     }
//     return best;
// }
//
//
//
//
// /**
//     Relocation of Jump Table Entry (e.g. jump slot).
//     A call to a library looks like the following :
//       .text: [EXECUTABLE READ ONLY]
//         ...
// A       call 0x000002080 <file relative address in plt>
// F       ...
//
//       .plt [EXECUTABLE READ ONLY]
//         ...
// B 0x2080 jmp *03030 <file relative address in .got.plt>
// C 0x2086 push 0x4 <This is the 4th function in the plt>
// D 0x208b jmp *0x00002018 <basically a call to dynamic linker>
//
//       .got.plt [NOT EXECUTABLE WRITABLE]
//         ...
// Z 0x3030 : 0x2086
//
//
//
// LIBRARY :
//    .text:
//      ...
// E 0xA018: FOO <do function stuff here>
//   ...
//   ret
//
//
// In the above example, in "normal" operation, we would see:
// The FIRST time we call the "foo"
// A
// B <jumps to *Z, which starts pointing at C>
// C
// D <change Z to the value of E: 0xA018, call Foo directly>
// E <execute Foo>
// F
//
// The SECOND time, we will have called the dynamic linker, and C and D are no inert
// A
// B <jumps to *Z which is NOW just E>
// E <execute Foo>
// F
//
// In this relocation, we need to mimic the dynamic linker, and overwrite the
// .got entry (Z) with the address of the function (E i.e. Foo)
// Thus, we're performing
// *Z = E
//
// */
//

} // namespace
} // namespace

#endif
