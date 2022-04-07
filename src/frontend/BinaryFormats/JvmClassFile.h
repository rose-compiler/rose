#ifndef JvmClassFile_H
#define JvmClassFile_H

#ifdef ROSE_ENABLE_BINARY_ANALYSIS

#ifdef JVM
namespace Jvm {
#endif

/** Helper functions to read uint8_t arrays from a Java class file.
 *  
 *  Memory for the array is allocated and the length returned.
 */
size_t read_jvm_bytes(const SgAsmJvmConstantPool* pool, char* &bytes);

template <typename T>
size_t read_jvm_value(const SgAsmJvmConstantPool* pool, T &value, bool advance_offset)
{
  SgAsmGenericHeader* header{pool->get_header()};
  rose_addr_t offset{header->get_offset()};

  std::cout << "read_jvm_value: offset is " << offset << std::endl;

  size_t count = header->read_content(offset, &value, sizeof(T));
  if (count != sizeof(T)) {
    //throw FormatError("Error reading JVM value");
    ROSE_ASSERT(false && "Error reading JVM value");
  }
  value = ByteOrder::be_to_host(value);
  if (advance_offset) header->set_offset(offset + count);
  return count;
}

#ifdef JVM
} // namespace Jvm
#endif

#endif
#endif
