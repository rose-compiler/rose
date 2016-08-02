#include "sage3basic.h"
#include "BinaryLoader.h"

void BinaryLoader::initDiagnostics() {}
rose_addr_t
SgAsmGenericSection::get_offset() const
{
    return p_offset;
}

size_t
SgAsmGenericSection::read_content(const MemoryMap *map, rose_addr_t start_va, void *dst_buf, rose_addr_t size, bool strict)
{
  return 0;
}

rose_addr_t
SgAsmGenericSection::get_mapped_preferred_rva() const
{
    ROSE_ASSERT(this != NULL);
    return p_mapped_preferred_rva;
}

size_t
SgAsmGenericFile::read_content(rose_addr_t offset, void *dst_buf, rose_addr_t size, bool strict)
{
    size_t retval;
    return retval;
}
size_t
SgAsmGenericFile::read_content(const MemoryMap *map, rose_addr_t start_va, void *dst_buf, rose_addr_t size, bool strict)
{
    size_t retval;
    return retval;
}
