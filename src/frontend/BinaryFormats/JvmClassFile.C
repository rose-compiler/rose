/* JVM (Java) class file header (SgAsmJvmClassFile and related classes) */
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include "sage3basic.h"

#include <Rose/Diagnostics.h>
#include "JvmClassFile.h"

using namespace Rose::Diagnostics;
using namespace ByteOrder;

// This function shouldn't be reached (ROSETTA won't allow pure virtual functions)
void SgAsmJvmNode::dump(FILE* f, const char* prefix, ssize_t idx) const
{
  fprintf(f, "SgAsmJvmNode::dump: should be a pure virtual function\n");
}

#if 0
SgAsmJvmClassFile::SgAsmJvmClassFile(const std::string &fileName)
  : SgAsmGenericFile{}
{
  std::cout << "SgAsmJvmClassFile::SgAsmJvmClassFile ...\n";

  /* Parse the generic file to load the file */
  SgAsmGenericFile::parse(fileName);  

  // What am I???
  //  set_sex(ByteOrder::ORDER_MSB);
  //
  std::cout << "isSgAsmJvmClassFile is " << isSgAsmJvmClassFile(this) << "\n";
  std::cout << "isSgAsmGenericFile is " << isSgAsmGenericFile(this) << "\n";
  std::cout << "isSgAsmGenericHeader is " << isSgAsmGenericHeader(this) << "\n";
  std::cout << "isSgAsmGenericSection is " << isSgAsmGenericSection(this) << "\n";

  std::cout << "SgAsmJavaClass: p_headers->get_headers().size(): " << p_headers->get_headers().size() << "\n";
  auto header = new SgAsmJvmFileHeader(this);
  std::cout << "SgAsmJavaClass: p_headers->get_headers().size(): " << p_headers->get_headers().size() << "\n";

  std::cout << "  header: " << header << "\n";

  auto header2 = p_headers->get_headers()[0];
  std::cout << "isSgAsmGenericHeader is " << isSgAsmGenericHeader(header) << "\n";
  std::cout << "isSgAsmJvmFileHeader is " << isSgAsmJvmFileHeader(header) << "\n";
  std::cout << "isSgAsmGenericHeader is " << isSgAsmGenericHeader(header2) << "\n";

  int same = header == header2;

  std::cout << "  same " << same << std::endl;

  std::cout << "SgAsmJvmClassFile::ctor() finished ...\n";
}
#endif

#if 0
SgAsmJvmClassFile::~SgAsmJvmClassFile()
{
  std::cout << "~SgAsmJvmClassFile:: ...\n";
  if (p_header_section) delete p_header_section;
}
#endif

#if 0
bool
SgAsmJvmClassFile::is_JVM(SgAsmGenericFile* file)
{
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
#endif

SgAsmJvmClassFile*
SgAsmJvmClassFile::parse(std::string fileName)
{
  std::cout << "SgAsmJvmClassFile::parse()\n";

  /* Allow parent to load file into memory (does no reading of file) */
#ifdef IN_PROGRESS
  SgAsmGenericFile::parse(fileName);

  std::cout << "\n--- file header ---\n";
  /* File header */
  auto header = new SgAsmJvmFileHeader(this);
  ROSE_ASSERT(header == get_header(SgAsmGenericFile::FAMILY_JVM));
  header->parse();

  /* Constant pool */
  p_constant_pool = new SgAsmJvmConstantPool(header);
  p_constant_pool->set_parent(this);

  auto pool = get_constant_pool();
  pool->parse();

  Jvm::read_value(pool, p_access_flags);
  Jvm::read_value(pool, p_this_class);
  Jvm::read_value(pool, p_super_class);

  std::cout << "\n--- interfaces ---\n";
  /* Interfaces */
  auto interfaces = new SgAsmJvmInterfaceTable(this);
  ROSE_ASSERT(interfaces->get_interfaces());
  interfaces->parse();
  interfaces->dump(stdout, "Interfaces\n", 0);

  std::cout << "\n--- fields ---\n";
  /* Fields */
  auto fields = new SgAsmJvmFieldTable(this);
  fields->parse();

  std::cout << "\n--- methods ---\n";
  /* Methods */
  auto methods = new SgAsmJvmMethodTable(this);
  methods->parse();

  std::cout << "\n--- attributes ---\n";
  /* Attributes */
  auto attributes = new SgAsmJvmAttributeTable(this);
  ROSE_ASSERT(attributes->get_parent());
  attributes->parse(pool);

  auto offset = header->get_offset();
  if (1 != (header->get_end_offset() - offset)) {
    ROSE_ASSERT(false && "Error reading file, end of file not reached");
  }

  std::cout << "-----------\n";
  std::cout << "Finished reading class file, # bytes read is " << header->get_offset() << std::endl;
#endif

  return this;
}

void
SgAsmJvmClassFile::dump(FILE* f, const char* prefix, ssize_t idx) const
{
  std::cout << "############### HELP CLASSFILE DUMP ###########\n\n";

  auto pool{get_constant_pool()};
  ROSE_ASSERT(pool);

#if 1
  std::cout << "\n--- JVM class file ---\n";
  std::cout << "SgAsmJvmClassFile::p_access_flags " << p_access_flags << std::endl;
  std::cout << "SgAsmJvmClassFile::p_this_class " << p_this_class << std::endl;
  std::cout << "SgAsmJvmClassFile::p_super_class " << p_super_class << std::endl;
#endif

#if 0
  std::string class_name{pool->get_utf8_string(p_this_class)};
  std::string super_name{pool->get_utf8_string(p_super_class)};
  fprintf(f, "%s:Class %s: Super: %s access %d\n", prefix, class_name.c_str(), super_name.c_str(), p_access_flags);
#endif

  get_constant_pool()->dump(stdout, "\nConstant Pool\n-------------\n", 1);

#if 0
  // No interfaces???
  interfaces->dump(stdout, "", 0);
#endif
#ifdef TRIALS
  fields->dump(stdout, "\nMethods\n_____________\n", 1);
  methods->dump(stdout, "", 0);
  attributes->dump(stdout, "", 0);
#endif
}

 // SgAsmGenericHeader old stuff
#if 0
bool
SgAsmJvmClassFile::reallocate()
{
  /* Do not reallocate this file header. */
  return false;
}

void
SgAsmJvmClassFile::unparse(std::ostream &f) const
{
  /* Do not unparse to this file. */
}

#endif // SgAsmGenericHeader old stuff

SgAsmJvmClass::SgAsmJvmClass(SgAsmJvmInterfaceTable* table)
{
  std::cout << "SgAsmJvmClass::ctor() ...\n";
  set_parent(table);
}

SgAsmJvmClass* SgAsmJvmClass::parse(SgAsmJvmConstantPool* pool)
{
  std::cout << "SgAsmJvmClass::parse() ...\n";

  return this;
}

void SgAsmJvmClass::dump(FILE* f, const char* prefix, ssize_t idx) const
{
  fprintf(f, "SgAsmJvmClass::dump() ...\n");
}

SgAsmJvmInterfaceTable::SgAsmJvmInterfaceTable(SgAsmJvmClassFile* jcf)
{
  std::cout << "SgAsmJvmInterfaceTable::ctor() ...\n";

  set_parent(jcf);
  p_interfaces = new SgAsmJvmClassList;
  p_interfaces->set_parent(this);
}

SgAsmJvmInterfaceTable* SgAsmJvmInterfaceTable::parse()
{
  std::cout << "SgAsmJvmInterfaceTable::parse() ...\n";
  uint16_t interfaces_count;

  auto jcf = dynamic_cast<SgAsmJvmClassFile*>(get_parent());
  ROSE_ASSERT(jcf && "JVM class_file is a nullptr");
  auto pool = jcf->get_constant_pool();
  ROSE_ASSERT(pool && "JVM constant_pool is a nullptr");

  Jvm::read_value(pool, interfaces_count);
  std::cout << "SgAsmJvmInterfaceTable::parse(): interfaces_count " << interfaces_count << std::endl;

  std::cout << "SgAsmJvmInterfaceTable::parse() exit ... \n";
  return this;
}

void SgAsmJvmInterfaceTable::dump(FILE* f, const char* prefix, ssize_t idx) const
{
  fprintf(f, "SgAsmJvmInterfaceTable::dump() ...\n");
}

#endif // ROSE_ENABLE_BINARY_ANALYSIS
