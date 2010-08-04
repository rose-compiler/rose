// tps (01/14/2010) : Switching from rose.h to sage3.
#include "sage3basic.h"
#include "Loader.h"
#include "LoaderELF.h"
#include "LoaderELFObj.h"
#include "LoaderPE.h"
#define __STDC_FORMAT_MACROS
#include <inttypes.h>

/* Main documentation is in the class declaration. */

std::vector<Loader*> Loader::p_registered;

/* Body of constructor, so we can make changes without having to recompile all of rose, while still keeping ':'
 * initializations near the declarations for the variables they initialize. */
void
Loader::ctor()
{
    //p_debug = stderr;
}

/* Class method */
void
Loader::initclass() 
{
    static bool initialized=false;
    if (!initialized) {
        initialized = true;
        /* Register from most general to most specific */
        register_subclass(new Loader);
        register_subclass(new LoaderELF);
        register_subclass(new LoaderELFObj);
        register_subclass(new LoaderPE);
    }
}

/* Class method to register a loader. */
void
Loader::register_subclass(Loader *loader)
{
    ROSE_ASSERT(loader!=NULL);
    initclass();
    p_registered.push_back(loader);
}

/* Class method to find a loader that can handle a particular file. */
Loader *
Loader::find_loader(SgAsmGenericHeader *header)
{
    for (std::vector<Loader*>::reverse_iterator i=p_registered.rbegin(); i!=p_registered.rend(); i++) {
        if ((*i)->can_handle(header))
            return *i;
    }
    return NULL;
}

/* Returns a map containing the specified sections (if they are mapped). */
MemoryMap *
Loader::map_all_sections(MemoryMap *map, const SgAsmGenericSectionPtrList &sections, bool allow_overmap)
{
#ifdef USE_ROSE
   return NULL;
#else
 // DQ (1/27/2010): This is a problem for ROSE.
    struct: public Selector {
        Contribution contributes(SgAsmGenericSection *section) {
            return CONTRIBUTE_ADD; /*alignment might weed out non-mapped sections*/
        }
    } s1;
    return create_map(map, sections, &s1, allow_overmap);
#endif
}

/* Returns a map of code-containing sections. A code-containing section is any section that is mapped with execute permission or
 * any section explicitly marked as containing code. Any section that is not code-containing is subtracted from the mapping. */
MemoryMap *
Loader::map_code_sections(MemoryMap *map, const SgAsmGenericSectionPtrList &sections, bool allow_overmap)
{
#ifdef USE_ROSE
   return NULL;
#else
 // DQ (1/27/2010): This is a problem for ROSE.
    struct: public Selector {
        Contribution contributes(SgAsmGenericSection *section) {
            if (section->get_contains_code()) {
                return CONTRIBUTE_ADD; /*even if it has no preferred mapping*/
            } else if (!section->is_mapped()) {
                return CONTRIBUTE_NONE;
            } else if (section->get_mapped_xperm()) {
                return CONTRIBUTE_ADD;
            } else {
                return CONTRIBUTE_SUB;
            }
        }
    } s1;
    return create_map(map, sections, &s1, allow_overmap);
#endif
}

/* Returns a map of executable sections. Any mapped section that isn't executable is subtracted from the mapping.
 *
 * Note that another way to do this is to use call map_all_sections() and then prune away, using MemoryMap::prune(), those
 * parts of the map that are not executable. */
MemoryMap *
Loader::map_executable_sections(MemoryMap *map, const SgAsmGenericSectionPtrList &sections, bool allow_overmap)
{
#ifdef USE_ROSE
   return NULL;
#else
 // DQ (1/27/2010): This is a problem for ROSE.
    struct: public Selector {
        Contribution contributes(SgAsmGenericSection *section) {
            if (!section->is_mapped()) {
                return CONTRIBUTE_NONE;
            } else if (section->get_mapped_xperm()) {
                return CONTRIBUTE_ADD;
            } else {
                return CONTRIBUTE_SUB;
            }
        }
    } s1;
    return create_map(map, sections, &s1, allow_overmap);
#endif
}

/* Returns a map of executable sections. Any section that is not writable is removed from the mapping.
 *
 * Note that another way to do this is to call map_all_sections() and then prune away, using MemoryMap::prune(), those parts
 * of the map that are not writable. */
MemoryMap *
Loader::map_writable_sections(MemoryMap *map, const SgAsmGenericSectionPtrList &sections, bool allow_overmap)
{
#ifdef USE_ROSE
   return NULL;
#else
 // DQ (1/27/2010): This is a problem for ROSE.
    struct: public Selector {
        Contribution contributes(SgAsmGenericSection *section) {
            if (!section->is_mapped()) {
                return CONTRIBUTE_NONE;
            } else if (section->get_mapped_wperm()) {
                return CONTRIBUTE_ADD;
            } else {
                return CONTRIBUTE_SUB;
            }
        }
    } s1;
    return create_map(map, sections, &s1, allow_overmap);
#endif
}

/* Align section addresses and sizes */
rose_addr_t
Loader::align_values(SgAsmGenericSection *section, Contribution contrib,
                     rose_addr_t *va_p/*out*/, rose_addr_t *mem_size_p/*out*/,
                     rose_addr_t *offset_p/*out*/, rose_addr_t *file_size_p/*out*/, 
                     const MemoryMap*)
{
    if (!section->is_mapped()) {
        *va_p = *mem_size_p = *offset_p = *file_size_p = 0;
        return 0;
    }
    
    rose_addr_t va = section->get_header()->get_base_va() + section->get_mapped_preferred_rva();
    rose_addr_t mem_size = section->get_mapped_size();
    rose_addr_t offset = section->get_offset();
    rose_addr_t file_size = section->get_size();
    rose_addr_t nleading = 0; /*number of bytes prepended to the mapped part of the file for alignment*/

    /* Align the file offset downward, adjusting file size as necessary. Then align the file size upward so the mapped file
     * extent ends on a boundary. */
    rose_addr_t fa = section->get_file_alignment();
    if (fa>0) {
        rose_addr_t n = ALIGN_DN(offset, fa);
        ROSE_ASSERT(n<=offset);
        nleading = offset-n;
        file_size += nleading;
        offset = n;
        /* file_size = ALIGN_UP(file_size, fa); -- a partial ending page shall be zero filled */
    }

    /* Align memory address downward, adjusting memory size as necessary. Then align memory size upward so the mapped region
     * ends on a boundary. */
    rose_addr_t ma = section->get_mapped_alignment();
    if (ma>0) {
        rose_addr_t n = ALIGN_DN(va, ma);
        ROSE_ASSERT(n<=va);
        mem_size += va-n;
        va = n;
        mem_size = ALIGN_UP(mem_size+nleading, ma);
    }

    *va_p = va;
    *mem_size_p = mem_size;
    *offset_p = offset;
    *file_size_p = file_size;

    return va + nleading;
}

/* Returns sections in mapping order. */
SgAsmGenericSectionPtrList
Loader::order_sections(const SgAsmGenericSectionPtrList &sections)
{
    SgAsmGenericSectionPtrList retval;
    for (size_t i=0; i<sections.size(); i++) {
        if (!sections[i]->get_synthesized()) {
            retval.push_back(sections[i]);
        }
    }
    return sections;
}

/* Uses a selector to determine what sections should be mapped. */
MemoryMap *
Loader::create_map(MemoryMap *map, const SgAsmGenericSectionPtrList &unordered_sections, Selector *selector, bool allow_overmap)
{
    if (!map)
        map = new MemoryMap();
    SgAsmGenericSectionPtrList sections = order_sections(unordered_sections);
    if (p_debug)
        fprintf(p_debug, "Loader: creating memory map...\n");
    for (size_t i=0; i<sections.size(); i++) {
        SgAsmGenericSection *section = sections[i];
        section->set_mapped_actual_rva(0); /*assigned below if mapped*/

        /* Does this section contribute anything to the mapping? */
        Contribution contrib = selector->contributes(section);
        if (CONTRIBUTE_NONE==contrib)
            continue;
        if (p_debug) {
            fprintf(p_debug, "  %smapping section [%d] \"%s\" with base va 0x%08"PRIx64"\n",
                    CONTRIBUTE_SUB==contrib ? "un" : "",
                    section->get_id(), section->get_name()->c_str(),
                    section->get_header()->get_base_va());
            fprintf(p_debug, "    Section specified: RVA 0x%08"PRIx64" + 0x%08"PRIx64" bytes = 0x%08"PRIx64"\n",
                    section->get_mapped_preferred_rva(), section->get_mapped_size(),
                    section->get_mapped_preferred_rva()+section->get_mapped_size());
            fprintf(p_debug, "    Section specified:  VA 0x%08"PRIx64" + 0x%08"PRIx64" bytes = 0x%08"PRIx64"\n",
                    section->get_header()->get_base_va() + section->get_mapped_preferred_rva(),
                    section->get_mapped_size(),
                    section->get_header()->get_base_va() + section->get_mapped_preferred_rva() + section->get_mapped_size());
        }
        
        rose_addr_t va=0, mem_size=0, offset=0, file_size=0;
        rose_addr_t section_va = align_values(section, contrib, &va, &mem_size, &offset, &file_size, map);
        if (p_debug) {
            fprintf(p_debug, "    Aligned values:     VA 0x%08"PRIx64" + 0x%08"PRIx64" bytes = 0x%08"PRIx64,
                    va, mem_size, va+mem_size);
            if (section->get_header()->get_base_va()+section->get_mapped_preferred_rva()==va &&
                section->get_mapped_size()==mem_size) {
                fputs(" (no change)\n", p_debug);
            } else {
                fprintf(p_debug, " (fa=%"PRIu64", ma=%"PRIu64")\n",
                        section->get_file_alignment(), section->get_mapped_alignment());
            }
            
        }

        if (0==mem_size) {
            if (p_debug) fprintf(p_debug, "    Nothing left to do.\n");
            continue;
        }

        if (p_debug && CONTRIBUTE_ADD==contrib) {
            fprintf(p_debug, "    Section begins at va   0x%08"PRIx64"\n", section_va);
            fprintf(p_debug, "    File location:         0x%08"PRIx64" + 0x%08"PRIx64" bytes = 0x%08"PRIx64"\n",
                    section->get_offset(), section->get_size(), section->get_offset()+section->get_size());
            fprintf(p_debug, "    File aligned:          0x%08"PRIx64" + 0x%08"PRIx64" bytes = 0x%08"PRIx64"%s\n",
                    offset, file_size, offset+file_size,
                    section->get_offset()==offset && section->get_size()==file_size ? " (no change)" : "");
            fprintf(p_debug, "    Permissions: %c%c%c\n", 
                    section->get_mapped_rperm()?'r':'-', section->get_mapped_wperm()?'w':'-', section->get_mapped_xperm()?'x':'-');
        }
        
        if (allow_overmap || CONTRIBUTE_SUB==contrib) {
            if (p_debug) {
                fprintf(p_debug, "    Erasing   va 0x%08"PRIx64" + 0x%08"PRIx64" bytes = 0x%08"PRIx64"\n",
                        va, mem_size, va+mem_size);
            }
            map->erase(MemoryMap::MapElement(va, mem_size));
        }

        if (CONTRIBUTE_ADD==contrib) {
            /* Split into a left and right part where the right part, extending beyond EOF, is mapped anonymously. */
            SgAsmGenericFile *file = section->get_file();
            rose_addr_t total = file->get_data().size(); /*total size of file*/
            rose_addr_t ltsz = 0;
            if (offset>total) {
                if (p_debug)
                    fprintf(p_debug, "    Map starts beyond end-of-file.\n");
                ltsz = 0;
            } else if (offset+file_size>total) {
                if (p_debug)
                    fprintf(p_debug, "    Map crosses end-of-file at 0x%08"PRIx64"\n", total);
                ltsz = std::min(mem_size, total-offset);
            } else {
                /* Map falls entirely within the file, but mem size might be larger than file size */
                ltsz = std::min(mem_size, file_size);
            }
            rose_addr_t rtsz = mem_size - ltsz;

            /* Permissions */
            unsigned mapperms=MemoryMap::MM_PROT_NONE;
            if (section->get_mapped_rperm())
                mapperms |= MemoryMap::MM_PROT_READ;
            if (section->get_mapped_wperm())
                mapperms |= MemoryMap::MM_PROT_WRITE;
            if (section->get_mapped_xperm())
                mapperms |= MemoryMap::MM_PROT_EXEC;

            /* Map the left part to the file; right part is anonymous. */
            if (p_debug)
                fprintf(p_debug, "    Mapping   va 0x%08"PRIx64" + 0x%08"PRIx64" bytes = 0x%08"PRIx64
                        " at file offset 0x%08"PRIx64"\n", va, ltsz, va+ltsz, offset);
            if (p_debug && rtsz>0)
                fprintf(p_debug, "    Anonymous va 0x%08"PRIx64" + 0x%08"PRIx64" bytes = 0x%08"PRIx64" zero filled\n",
                        va+ltsz, rtsz, va+ltsz+rtsz);
            if (ltsz>0) {
                MemoryMap::MapElement melmt(va, ltsz, &(file->get_data()[0]), offset, mapperms);
                melmt.set_name(section->get_name()->get_string());
                map->insert(melmt);
            }
            if (rtsz>0) {
                MemoryMap::MapElement melmt(va+ltsz, rtsz, mapperms);
                melmt.set_name(section->get_name()->get_string());
                map->insert(melmt);
            }

            /* Remember virtual address of first byte of section. */
            ROSE_ASSERT(section_va >= section->get_header()->get_base_va());
            section->set_mapped_actual_rva(section_va - section->get_header()->get_base_va());
        }
    }
    if (p_debug) {
        fprintf(p_debug, "Loader: created map:\n");
        map->dump(p_debug, "    ");
    }

    return map;
}
