#ifndef Jvm_H
#define Jvm_H

#ifdef ROSE_ENABLE_BINARY_ANALYSIS

namespace Jvm {

/** Helper function to read uint8_t arrays from a Java class file.
 *  
 *  Memory for the array is allocated and the length returned.
 */
template <typename T>
size_t read_bytes(const SgAsmJvmConstantPool* pool, char* &bytes, T &length)
{
  SgAsmGenericHeader* header{pool->get_header()};
  rose_addr_t offset{header->get_offset()};

  /* read length of the array */
  size_t count = header->read_content(offset, &length, sizeof(length));
  if (count != sizeof(length)) {
    //throw FormatError("Error reading JVM bytes array length");
    ROSE_ASSERT(false && "Error reading JVM bytes array length");
  }
  length = ByteOrder::be_to_host(length);
  offset += count;
  header->set_offset(offset);

  /* allocate memory for array */
  bytes = new char[length];

  /* read array */
  count = header->read_content(offset, bytes, length);
  if (count != length) {
    //throw FormatError("Error reading JVM bytes array");
    ROSE_ASSERT(false && "Error reading JVM bytes array");
  }
  offset += count;
  header->set_offset(offset);

  return count;
}

template <typename T>
size_t read_value(const SgAsmJvmConstantPool* pool, T &value, bool advance_offset=true)
{
  SgAsmGenericHeader* header{pool->get_header()};
  rose_addr_t offset{header->get_offset()};

  size_t count = header->read_content(offset, &value, sizeof(T));
  if (count != sizeof(T)) {
    //throw FormatError("Error reading JVM value");
    ROSE_ASSERT(false && "Error reading JVM value");
  }
  value = ByteOrder::be_to_host(value);
  if (advance_offset) header->set_offset(offset + count);
  return count;
}

} // namespace Jvm

#endif
#endif
