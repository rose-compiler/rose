#include "rose.h"

#define __STDC_FORMAT_MACROS
#include <inttypes.h>

/* Main documentation is in the class declaration. */

std::vector<Loader*> Loader::p_registered;

/* Class method */
void
Loader::initclass() 
{
    static bool initialized=false;
    if (!initialized) {
        initialized = true;
        register_subclass(new Loader);
        register_subclass(new LoaderELF);
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
Loader::map_all_sections(const SgAsmGenericSectionPtrList &sections, bool allow_overmap)
{
    struct: public Selector {
        Contribution contributes(SgAsmGenericSection *section) {
            return section->is_mapped() ? CONTRIBUTE_ADD : CONTRIBUTE_NONE;
        }
    } s1;
    return create_map(sections, &s1, allow_overmap);
}

/* Returns a map of code-containing sections. A code-containing section is any section that is mapped with execute permission or
 * any section explicitly marked as containing code. Any section that is not code-containing is subtracted from the mapping. */
MemoryMap *
Loader::map_code_sections(const SgAsmGenericSectionPtrList &sections, bool allow_overmap)
{
    struct: public Selector {
        Contribution contributes(SgAsmGenericSection *section) {
            if (section->is_mapped() && (section->get_contains_code() || section->get_mapped_xperm())) {
                return CONTRIBUTE_ADD;
            } else if (!section->is_mapped()) {
                return CONTRIBUTE_NONE;
            } else {
                return CONTRIBUTE_SUB;
            }
        }
    } s1;
    return create_map(sections, &s1, allow_overmap);
}

/* Returns a map of executable sections. Any mapped section that isn't executable is subtracted from the mapping. */
MemoryMap *
Loader::map_executable_sections(const SgAsmGenericSectionPtrList &sections, bool allow_overmap)
{
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
    return create_map(sections, &s1, allow_overmap);
}

/* Returns a map of executable sections. Any section that is not executable is removed from the mapping. */
MemoryMap *
Loader::map_writable_sections(const SgAsmGenericSectionPtrList &sections, bool allow_overmap)
{
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
    return create_map(sections, &s1, allow_overmap);
}

/* Align section addresses and sizes */
void
Loader::align_values(SgAsmGenericSection *section,
                     rose_addr_t *va_p/*out*/, rose_addr_t *mem_size_p/*out*/,
                     rose_addr_t *offset_p/*out*/, rose_addr_t *file_size_p/*out*/)
{
    ROSE_ASSERT(section->is_mapped());
    rose_addr_t va = section->get_header()->get_base_va() + section->get_mapped_rva();
    if (section->get_mapped_rva()==0 && section->get_rose_mapped_rva()>0)
        va += section->get_rose_mapped_rva();
    rose_addr_t mem_size = section->get_mapped_size();
    rose_addr_t offset = section->get_offset();
    rose_addr_t file_size = section->get_size();

    /* Align the file offset downward, adjusting file size as necessary. Then align the file size upward so the mapped file
     * extent ends on a boundary. */
    rose_addr_t fa = section->get_file_alignment();
    if (fa>0) {
        rose_addr_t n = ALIGN_DN(offset, fa);
        ROSE_ASSERT(n<=offset);
        file_size += offset-n;
        offset = n;
        file_size = ALIGN_UP(file_size, fa);
    }

    /* Align memory address downward, adjusting file size as necessary. Then align memory size upward so the mapped region
     * ends on a boundary. */
    rose_addr_t ma = section->get_mapped_alignment();
    if (ma>0) {
        rose_addr_t n = ALIGN_DN(va, ma);
        ROSE_ASSERT(n<=va);
        mem_size += va-n;
        va = n;
        mem_size = ALIGN_UP(mem_size, ma);
    }

    *va_p = va;
    *mem_size_p = mem_size;
    *offset_p = offset;
    *file_size_p = file_size;
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
Loader::create_map(const SgAsmGenericSectionPtrList &unordered_sections, Selector *selector, bool allow_overmap)
{
    MemoryMap *map = new MemoryMap();
    SgAsmGenericSectionPtrList sections = order_sections(unordered_sections);
    if (p_debug)
        fprintf(p_debug, "Loader: creating memory map...\n");
    for (size_t i=0; i<sections.size(); i++) {
        SgAsmGenericSection *section = sections[i];
        Contribution contrib = selector->contributes(section);
        if (CONTRIBUTE_NONE==contrib)
            continue;
        rose_addr_t va=0, mem_size=0, offset=0, file_size=0;
        align_values(section, &va, &mem_size, &offset, &file_size);

        if (p_debug) {
            fprintf(p_debug, "  %smapping section [%d] \"%s\" with base va 0x%08"PRIx64"\n",
                    CONTRIBUTE_SUB==contrib ? "un" : "",
                    section->get_id(), section->get_name()->c_str(),
                    section->get_header()->get_base_va());
            fprintf(p_debug, "    Section specified: RVA 0x%08"PRIx64" + 0x%08"PRIx64" bytes = 0x%08"PRIx64"\n",
                    section->get_mapped_rva(), section->get_mapped_size(), section->get_mapped_rva()+section->get_mapped_size());
            fprintf(p_debug, "    Section specified:  VA 0x%08"PRIx64" + 0x%08"PRIx64" bytes = 0x%08"PRIx64"\n",
                    section->get_header()->get_base_va() + section->get_mapped_rva(),
                    section->get_mapped_size(),
                    section->get_header()->get_base_va() + section->get_mapped_rva() + section->get_mapped_size());
            fprintf(p_debug, "    Aligned values:     VA 0x%08"PRIx64" + 0x%08"PRIx64" bytes = 0x%08"PRIx64"%s\n",
                    va, mem_size, va+mem_size,
                    (section->get_header()->get_base_va()+section->get_mapped_rva()==va && section->get_mapped_size()==mem_size ?
                     " (no change)":""));
        }
        if (p_debug && CONTRIBUTE_ADD==contrib) {
            fprintf(p_debug, "    File location:         0x%08"PRIx64" + 0x%08"PRIx64" bytes = 0x%08"PRIx64"\n",
                    section->get_offset(), section->get_size(), section->get_offset()+section->get_size());
            fprintf(p_debug, "    File aligned:          0x%08"PRIx64" + 0x%08"PRIx64" bytes = 0x%08"PRIx64"%s\n",
                    offset, file_size, offset+file_size,
                    section->get_offset()==offset && section->get_size()==file_size ? " (no change)" : "");
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

            /* Map the left part to the file; right part is anonymous. */
            if (p_debug)
                fprintf(p_debug, "    Mapping   va 0x%08"PRIx64" + 0x%08"PRIx64" bytes = 0x%08"PRIx64
                        " at file offset 0x%08"PRIx64"\n", va, ltsz, va+ltsz, offset);
            if (p_debug && rtsz>0)
                fprintf(p_debug, "    Anonymous va 0x%08"PRIx64" + 0x%08"PRIx64" bytes = 0x%08"PRIx64" zero filled\n",
                        va+ltsz, rtsz, va+ltsz+rtsz);
            map->insert(MemoryMap::MapElement(va, ltsz, offset));
            if (rtsz>0)
                map->insert(MemoryMap::MapElement(va+ltsz, rtsz));
        }
    }
    if (p_debug) {
        fprintf(p_debug, "Loader: created map:\n");
        map->dump(p_debug, "    ");
    }

    return map;
}
