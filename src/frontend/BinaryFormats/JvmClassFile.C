/* JVM (Java) class file header (SgAsmJvmClassFile and related classes) */
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include "sage3basic.h"

#include <Rose/Diagnostics.h>
#include "JvmClassFile.h"

using namespace Rose::Diagnostics;
using namespace ByteOrder;
using std::cout;
using std::endl;

// This function shouldn't be reached (ROSETTA won't allow pure virtual functions)
void SgAsmJvmNode::dump(FILE* f, const char* prefix, ssize_t idx) const
{
  fprintf(f, "SgAsmJvmNode::dump: should be a pure virtual function\n");
}

#if 0
SgAsmJvmClassFile::SgAsmJvmClassFile(const std::string &fileName)
  : SgAsmGenericFile{}
{
  cout << "SgAsmJvmClassFile::SgAsmJvmClassFile ...\n";

  /* Parse the generic file to load the file */
  SgAsmGenericFile::parse(fileName);  

  // What am I???
  //  set_sex(ByteOrder::ORDER_MSB);
  //
  cout << "isSgAsmJvmClassFile is " << isSgAsmJvmClassFile(this) << "\n";
  cout << "isSgAsmGenericFile is " << isSgAsmGenericFile(this) << "\n";
  cout << "isSgAsmGenericHeader is " << isSgAsmGenericHeader(this) << "\n";
  cout << "isSgAsmGenericSection is " << isSgAsmGenericSection(this) << "\n";

  cout << "SgAsmJavaClass: p_headers->get_headers().size(): " << p_headers->get_headers().size() << "\n";
  auto header = new SgAsmJvmFileHeader(this);
  cout << "SgAsmJavaClass: p_headers->get_headers().size(): " << p_headers->get_headers().size() << "\n";

  cout << "  header: " << header << "\n";

  auto header2 = p_headers->get_headers()[0];
  cout << "isSgAsmGenericHeader is " << isSgAsmGenericHeader(header) << "\n";
  cout << "isSgAsmJvmFileHeader is " << isSgAsmJvmFileHeader(header) << "\n";
  cout << "isSgAsmGenericHeader is " << isSgAsmGenericHeader(header2) << "\n";

  int same = header == header2;

  cout << "  same " << same << endl;

  cout << "SgAsmJvmClassFile::ctor() finished ...\n";
}
#endif

#if 0
SgAsmJvmClassFile::~SgAsmJvmClassFile()
{
  cout << "~SgAsmJvmClassFile:: ...\n";
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
  cout << "SgAsmJvmClassFile::parse()\n";

  /* Allow parent to load file into memory (does no reading of file) */
#ifdef NOT_YET
  SgAsmGenericFile::parse(fileName);
#endif

  cout << "\n--- file header ---\n";
  /* File header */
  auto header = new SgAsmJvmFileHeader();
#ifdef NOT_YET
  ROSE_ASSERT(header == get_header(SgAsmGenericFile::FAMILY_JVM));
#endif
  header->parse();

  /* Constant pool */
  p_constant_pool = new SgAsmJvmConstantPool(header);
  p_constant_pool->set_parent(this);

  auto pool = get_constant_pool();
  pool->parse();

  Jvm::read_value(pool, p_access_flags);
  Jvm::read_value(pool, p_this_class);
  Jvm::read_value(pool, p_super_class);

  cout << "\n--- interfaces ---\n";
  /* Interfaces */
#ifdef NOT_YET
  std::list<uint16_t>& interfaces = get_interfaces();
  std::list<uint16_t>* ilist = &p_interfaces;
  cout << "SgAsmJvmClassFile::parse(): &p_interfaces: " << ilist << ": size:" << ilist->size() << endl;;
#endif
  uint16_t interfaces_count;
  Jvm::read_value(pool, interfaces_count);
  cout << "SgAsmJvmClassFile::parse(): interfaces_count " << interfaces_count << endl;

  for (int i = 0; i < interfaces_count; i++) {
    uint16_t index;
    Jvm::read_value(pool, index);
    cout << "     interface class index: " << index << endl;
#ifdef NOT_YET
    interfaces.push_back(index);
    cout << "SgAsmJvmClassFile::parse(): &p_interfaces: " << ilist << ": size:" << ilist->size() << endl;;
    cout << "SgAsmJvmClassFile::parse(): get_interfaces().size(): " << get_interfaces().size() << endl;;
    cout << "SgAsmJvmClassFile::parse(): interfaces.size(): " << interfaces.size() << endl;;
#endif
  }

  cout << "\n--- fields ---\n";
  /* Fields */
  auto fields = new SgAsmJvmFieldTable(this);
  fields->parse();

  cout << "\n--- methods ---\n";
  /* Methods */
  auto methods = new SgAsmJvmMethodTable(this);
  methods->parse();

  cout << "\n--- attributes ---\n";
  /* Attributes */
  auto attributes = new SgAsmJvmAttributeTable(this);
  ROSE_ASSERT(attributes->get_parent());
  attributes->parse(pool);

  auto offset = header->get_offset();
  if (1 != (header->get_end_offset() - offset)) {
    ROSE_ASSERT(false && "Error reading file, end of file not reached");
  }

  cout << "-----------\n";
  cout << "Finished reading class file, # bytes read is " << header->get_offset() << endl;

  return this;
}

void
SgAsmJvmClassFile::dump(FILE* f, const char* prefix, ssize_t idx) const
{
  cout << "############### HELP CLASSFILE DUMP ###########\n\n";

  auto pool{get_constant_pool()};
  ROSE_ASSERT(pool);

#if 1
  cout << "\n--- JVM class file ---\n";
  cout << "SgAsmJvmClassFile::p_access_flags " << p_access_flags << endl;
  cout << "SgAsmJvmClassFile::p_this_class " << p_this_class << endl;
  cout << "SgAsmJvmClassFile::p_super_class " << p_super_class << endl;
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

#endif // ROSE_ENABLE_BINARY_ANALYSIS
