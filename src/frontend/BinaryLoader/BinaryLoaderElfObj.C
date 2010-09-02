#include "sage3basic.h"
#include "BinaryLoaderElfObj.h"

/* This binary loader can handle ELF object files. */
bool
BinaryLoaderElfObj::can_load(SgAsmGenericHeader *hdr) const
{
    return isSgAsmElfFileHeader(hdr) && hdr->get_exec_format()->get_purpose()==SgAsmExecutableFileFormat::PURPOSE_LIBRARY;
}

/* Identical to parent class but with this minor difference:  Any section that has no mapping information but is known to
 * contain code (SgAsmGenericSection::get_contains_code() is true) is mapped to an otherwise unused area of the virtual
 * memory. */
rose_addr_t
BinaryLoaderElfObj::align_values(SgAsmGenericSection *section, Contribution contrib,
                                 rose_addr_t *va_p/*out*/, rose_addr_t *mem_size_p/*out*/,
                                 rose_addr_t *offset_p/*out*/, rose_addr_t *file_size_p/*out*/,
                                 const MemoryMap *current)
{
    if (section->is_mapped())
        return BinaryLoaderElf::align_values(section, contrib, va_p, mem_size_p, offset_p, file_size_p, current);

    if (section->get_contains_code()) {
        /* We don't need to worry about file alignment because the Unix loader isn't going to ever be mapping this object file
         * anyway.  We align memory on our best guess of a page boundary, 512 bytes. */
        rose_addr_t mem_alignment = 512;
        rose_addr_t size = section->get_size();
        *va_p = current->find_free(0, size, mem_alignment);
        *mem_size_p = *file_size_p = size;
        *offset_p = section->get_offset();
        return *va_p;
    }

    /* Not mapped and not known to contain code, so it should not contribute to the final map. */
    *va_p = *mem_size_p = *offset_p = *file_size_p = 0;
    return 0;
}
