#include "sage3basic.h"
#include "BinaryLoader.h"

void BinaryLoader::initDiagnostics() {}
rose_addr_t SgAsmGenericSection::get_offset() const { return 0; }
size_t SgAsmGenericSection::read_content(const MemoryMap*, rose_addr_t, void*, rose_addr_t, bool) { return 0; }
rose_addr_t SgAsmGenericSection::get_mapped_preferred_rva() const { return 0; }
size_t SgAsmGenericFile::read_content(rose_addr_t, void*, rose_addr_t, bool) { return 0; }
size_t SgAsmGenericFile::read_content(const MemoryMap*, rose_addr_t, void*, rose_addr_t, bool) { return 0; }
