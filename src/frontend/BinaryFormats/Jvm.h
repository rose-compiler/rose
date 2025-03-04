#ifndef Jvm_H
#define Jvm_H
#ifdef ROSE_ENABLE_BINARY_ANALYSIS

#include <Rose/BinaryAnalysis/ByteOrder.h>

namespace Jvm {

/** Helper function to read uint8_t arrays from a Java class file.
 *  
 *  Memory for the array is allocated and the length returned.
 */
template <typename T>
size_t read_bytes(const SgAsmJvmConstantPool* pool, char* &bytes, T &length)
{
  SgAsmGenericHeader* header{pool->get_header()};
  Rose::BinaryAnalysis::Address offset{header->get_offset()};

  /* read length of the array */
  size_t count = header->readContent(offset, &length, sizeof(length));
  if (count != sizeof(length)) {
    //throw FormatError("Error reading JVM bytes array length");
    ROSE_ASSERT(false && "Error reading JVM bytes array length");
  }
  length = Rose::BinaryAnalysis::ByteOrder::beToHost(length);
  offset += count;
  header->set_offset(offset);

  /* allocate memory for array */
  bytes = new char[length];

  /* read array */
  count = header->readContent(offset, bytes, length);
  if (count != length) {
    //throw FormatError("Error reading JVM bytes array");
    ROSE_ASSERT(false && "Error reading JVM bytes array");
  }
  offset += count;
  header->set_offset(offset);

  return count;
}

template <typename T>
size_t read_value(const SgAsmJvmConstantPool* pool, T &value, bool advanceOffset=true)
{
  SgAsmGenericHeader* header{pool->get_header()};
  Rose::BinaryAnalysis::Address offset{header->get_offset()};

  size_t count = header->readContent(offset, &value, sizeof(T));
  if (count != sizeof(T)) {
    //throw FormatError("Error reading JVM value");
    ROSE_ASSERT(false && "Error reading JVM value");
  }
  value = Rose::BinaryAnalysis::ByteOrder::beToHost(value);
  if (advanceOffset) header->set_offset(offset + count);
  return count;
}

template <typename T>
void writeValue(std::ostream& os, T value)
{
  Rose::BinaryAnalysis::ByteOrder::hostToBe(value, &value);
  os.write(reinterpret_cast<const char*>(&value), sizeof value);
}

} // namespace Jvm

#endif
#endif
