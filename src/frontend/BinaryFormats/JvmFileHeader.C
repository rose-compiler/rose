/* JVM File Header (SgAsmJvmFileHeader and related classes) */
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include "sage3basic.h"

#include <Rose/Diagnostics.h>

using namespace Rose::Diagnostics;
using namespace ByteOrder;

SgAsmJvmFileHeader::SgAsmJvmFileHeader(SgAsmGenericFile* f)
  : SgAsmGenericHeader{f}, p_minor_version{0}, p_major_version{0},
    p_access_flags{0}, p_this_class{0}, p_super_class{0}
{
  std::cout << "SgAsmJvmFileHeader::ctor() ...\n";

  ROSE_ASSERT(get_file() != nullptr);
  ROSE_ASSERT(get_size() > 0);

  set_name(new SgAsmBasicString("JVM File Header"));
  set_synthesized(true);
  set_purpose(SP_HEADER); // SectionPurpose

  /* Magic number */
  p_magic.clear();
  p_magic.push_back(0xCA);
  p_magic.push_back(0xFE);
  p_magic.push_back(0xBA);
  p_magic.push_back(0xBE);

  /* Executable Format */
  ROSE_ASSERT(p_exec_format != nullptr);
  p_exec_format->set_family(FAMILY_JVM);
  p_exec_format->set_purpose(PURPOSE_EXECUTABLE);
  p_exec_format->set_sex(ByteOrder::ORDER_MSB);
  p_exec_format->set_word_size(4);
  p_exec_format->set_version(0);
  p_exec_format->set_is_current_version(false);
  p_exec_format->set_abi(ABI_JVM);
  p_exec_format->set_abi_version(0);

  p_isa = ISA_JVM;

  /* Constant pool */
  auto pool = new SgAsmJvmConstantPool(this);
  set_constant_pool(pool);
  pool->set_parent(this);

  std::cout << "SgAsmJvmFileHeader::ctor() finished ...\n\n";
}

SgAsmJvmFileHeader*
SgAsmJvmFileHeader::parse()
{
  ROSE_ASSERT(get_file() != nullptr);
  ROSE_ASSERT(get_size() > 0);

  std::cout << "SgAsmJvmFileHeader::parse()...\n";

  // TODO:
  /* The parent class will have already been parsed in constructing the Java class file */
  SgAsmGenericHeader::parse();

  std::cout << "WARNING: SgAsmJvmFileHeader::parse():0\n"; // TODO: move to constructor
  rose_addr_t offset{get_offset()};

  std::cout << "SgAsmJvmFileHeader::parse() offset is " << offset << std::endl;

// NOTE: refactor this once SgJavaClassFile exists (maybe)
#if 1
  /* Ensure magic number in file is correct */
  unsigned char magic[4];
  auto count = read_content(offset, magic, sizeof magic);
  if (4!=count || p_magic.size()!=count || p_magic[0]!=magic[0]
               || p_magic[1]!=magic[1] || p_magic[2]!=magic[2] || p_magic[3]!=magic[3]) {
    throw FormatError("Bad Java class file magic number");
  }
  offset += count;
  set_offset(offset);
#endif

  /* Minor version */
  count = read_content(offset, &p_minor_version, sizeof p_minor_version);
  if (2 != count) {
    throw FormatError("Bad Java class file minor_version");
  }
  p_minor_version = be_to_host(p_minor_version);
  offset += count;
  set_offset(offset);

  std::cout << "SgAsmJvmFileHeader::parse() offset is " << get_offset() << std::endl;

  /* Major version */
  count = read_content(offset, &p_major_version, sizeof p_major_version);
  if (2 != count) {
    throw FormatError("Bad Java class file major_version");
  }
  p_major_version = be_to_host(p_major_version);
  offset += count;
  set_offset(offset);

  std::cout << "SgAsmJvmFileHeader::parse() offset is " << get_offset() << std::endl;
  std::cout << "SgAsmJvmFileHeader::parse() major, minor: " << p_major_version << "," << p_minor_version << std::endl;

  p_exec_format->set_version(p_major_version);
  p_exec_format->set_is_current_version(true);
  p_exec_format->set_abi_version(p_major_version);

  get_constant_pool()->parse();
  offset = get_offset();
  std::cout << "SgAsmJvmFileHeader::parse() offset is " << get_offset() << std::endl;

  /* p_access_flags */
  count = read_content(offset, &p_access_flags, sizeof p_access_flags);
  if (2 != count) {
    throw FormatError("Bad Java class file access_flags");
  }
  p_access_flags = be_to_host(p_access_flags);
  offset += count;
  set_offset(offset);

  std::cout << "SgAsmJvmFileHeader::p_access_flags " << p_access_flags << std::endl;

  /* this_class */
  count = read_content(offset, &p_this_class, sizeof p_this_class);
  if (2 != count) {
    throw FormatError("Bad Java class file this_class");
  }
  p_this_class = be_to_host(p_this_class);
  offset += count;
  set_offset(offset);

  std::cout << "SgAsmJvmFileHeader::p_this_class " << p_this_class << std::endl;

  /* super_class */
  count = read_content(offset, &p_super_class, sizeof p_super_class);
  if (2 != count) {
    throw FormatError("Bad Java class file super_class");
  }
  p_super_class = be_to_host(p_super_class);
  offset += count;
  set_offset(offset);

  std::cout << "SgAsmJvmFileHeader::p_super_class " << p_super_class << std::endl;

  return this;
}

#if 0
bool
SgAsmJvmFileHeader::reallocate()
{
  /* Do not reallocate this file header. */
  return false;
}
#endif

#if 0
void
SgAsmJvmFileHeader::unparse(std::ostream &f) const
{
  /* Do not unparse to this file. */
}

void
SgAsmJvmFileHeader::dump(FILE *f, const char *prefix, ssize_t idx) const
{
  char p[4096];
  if (idx>=0) {
    sprintf(p, "%sJvmFileHeader[%zd].", prefix, idx);
  } else {
    sprintf(p, "%sJvmFileHeader.", prefix);
  }
  int w = std::max(1, DUMP_FIELD_WIDTH-(int)strlen(p));

  SgAsmGenericHeader::dump(f, p, -1);
  fprintf(f, "%s%-*s = 0x%08x\n",   p, w, "magic_number",   p_magic_number); // 0x0a00 is for "JVM_FAMILY"
  fprintf(f, "%s%-*s = %u\n",       p, w, "major_version",  p_major_version);
  fprintf(f, "%s%-*s = %u\n",       p, w, "minor_version",  p_minor_version);
}

bool
SgAsmJvmFileHeader::is_JVM(SgAsmGenericFile* file)
{
  ROSE_ASSERT(file != nullptr);

  /* Turn off byte reference tracking for the duration of this function. We don't want our testing the file contents to
   * affect the list of bytes that we've already referenced or which we might reference later. */
  bool was_tracking = file->get_tracking_references();
  file->set_tracking_references(false);
  try {
    unsigned char magic[4];
    auto count = file->read_content(0, magic, sizeof magic);
    if (4 != count || 0xCA!=magic[0] || 0xFE!=magic[1] || 0xBA!=magic[2] || 0xBE!=magic[3]) {
      throw 1;
    }
  } catch (...) {
    file->set_tracking_references(was_tracking);
    return false;
  }
  file->set_tracking_references(was_tracking);
  return true;
}

const char *
SgAsmJvmFileHeader::format_name() const {
  return "JVM";
}
#endif // add to ROSETTA

#endif
