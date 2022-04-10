/* JVM (Java) class file header (SgAsmJvmClassFile and related classes) */
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include "sage3basic.h"

#include <Rose/Diagnostics.h>
#include "JvmClassFile.h"

using namespace Rose::Diagnostics;
using namespace ByteOrder;

// This function shouldn't be reached (ROSETTA won't allow pure virtual functions)
void SgAsmJvmNode::dump(std::ostream &os) const
{
  os << "SgAsmJvmNode::dump: should be a pure virtual function\n";
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
  // Maybe NOT below
  /* Note the parent class was parsed during construction to set up file parameters */

  std::cout << "SgAsmJvmClassFile::parse()\n";

  // GenericFile needed temporarily until inherit from it?
  auto gf = new SgAsmGenericFile{};
  gf->parse(fileName); /* this loads file into memory, does no reading of file */
  //  std::cout << "SgAsmJvmClassFile::parse() is_JVM: " << SgAsmJvmFileHeader::is_JVM(gf) << std::endl;

  auto header = new SgAsmJvmFileHeader(gf);
  ROSE_ASSERT(header == gf->get_header(SgAsmGenericFile::FAMILY_JVM));

  auto gh = gf->get_header(SgAsmGenericFile::FAMILY_JVM);
  std::cout << "--> header class is " << header->class_name() << ":" << header << std::endl;
  std::cout << "--> generic header class is " << gh->class_name() << ":" << gh << std::endl;

  header->parse();
  std::cout << "SgAsmJvmClassFile::parse(): finished parsing header\n";

  // Not sure we need SgAsmJvmClassFile
  //  gf->add_header(header);
  gh = gf->get_header(SgAsmGenericFile::FAMILY_JVM);
  ROSE_ASSERT(gh);
  std::cout << "--> header class is " << header->class_name() << ":" << header << std::endl;
  std::cout << "--> generic header class is " << gh->class_name() << ":" << gh << std::endl;
  ROSE_ASSERT(header == gf->get_header(SgAsmGenericFile::FAMILY_JVM));

  /* Constant pool */
  p_constant_pool = new SgAsmJvmConstantPool(header);
  p_constant_pool->set_parent(this);

  auto pool = get_constant_pool();
  pool->parse();

  auto offset = header->get_offset();
  std::cout << "SgAsmJvmClassFile::parse() offset is " << header->get_offset() << std::endl;

  /* p_access_flags */
  auto count = gf->read_content(offset, &p_access_flags, sizeof p_access_flags);
  if (2 != count) {
    throw FormatError("Bad Java class file access_flags");
  }
  p_access_flags = be_to_host(p_access_flags);
  offset += count;
  header->set_offset(offset);

  std::cout << "SgAsmJvmClassFile::p_access_flags " << p_access_flags << std::endl;

  /* this_class */
  count = gf->read_content(offset, &p_this_class, sizeof p_this_class);
  if (2 != count) {
    throw FormatError("Bad Java class file this_class");
  }
  p_this_class = be_to_host(p_this_class);
  offset += count;
  header->set_offset(offset);

  std::cout << "SgAsmJvmClassFile::p_this_class " << p_this_class << std::endl;

  /* super_class */
  count = gf->read_content(offset, &p_super_class, sizeof p_super_class);
  if (2 != count) {
    throw FormatError("Bad Java class file super_class");
  }
  p_super_class = be_to_host(p_super_class);
  offset += count;
  header->set_offset(offset);

  std::cout << "SgAsmJvmClassFile::p_super_class " << p_super_class << std::endl;

  std::cout << "SgAsmJvmClassFile::parse() offset is " << offset << std::endl;

  // Interface sections
  //
  uint16_t interfaces_count;
  count = gf->read_content(offset, &interfaces_count, sizeof interfaces_count);
  if (2 != count) {
    throw FormatError("Bad Java class file interfaces_count");
  }
  interfaces_count = be_to_host(interfaces_count);
  offset += count;
  gh->set_offset(offset);

  std::cout << "SgAsmJvmClassFile::interfaces_count " << interfaces_count << std::endl;
  // TeMPorary
  ROSE_ASSERT(interfaces_count == 0);

  // Fields
  //
  uint16_t fields_count;
  count = gf->read_content(offset, &fields_count, sizeof fields_count);
  if (2 != count) {
    throw FormatError("Bad Java class file fields_count");
  }
  fields_count = be_to_host(fields_count);
  offset += count;
  gh->set_offset(offset);

  std::cout << "SgAsmJvmClassFile::fields_count " << fields_count << std::endl;
  // TeMPorary
  ROSE_ASSERT(fields_count == 0);

  // Methods
  //
  uint16_t methods_count;
  count = gf->read_content(offset, &methods_count, sizeof methods_count);
  if (2 != count) {
    throw FormatError("Bad Java class file methods_count");
  }
  methods_count = be_to_host(methods_count);
  offset += count;
  gh->set_offset(offset);

  std::cout << "SgAsmJvmClassFile::methods_count " << methods_count << std::endl;

  // Attributes
  //

//erasmus
#if 0
  rose_addr_t fileOffset = 0; // TODO: use tracking_references?

  std::cout << "SgAsmJavaClass: p_headers->get_headers().size(): " << p_headers->get_headers().size() << "\n";

  if (p_headers->get_headers().size()>0 && isSgAsmGenericHeader(p_headers->get_headers()[0])) {
    auto header = p_headers->get_headers()[0];
    std::cout << "header is " << isSgAsmGenericHeader(header) << std::endl;
    std::cout << "s()[i]->get_exec_format()->get_family(): "
              << p_headers->get_headers()[0]->get_exec_format()->get_family() << "\n";
  }
  else {
    throw FormatError("Bad Java class file header");
  }

  /* Parse the header for magic number... and constant pool section */
  auto header = p_headers->get_headers()[0];
  header->parse();

#if 0
  p_header_section = new SgAsmGenericHeader(this);
  std::cout << "isSgAsmGenericSection is " << isSgAsmGenericSection(p_header_section) << "\n";
  //  delete p_header_section;
  //  p_header_section = nullptr;
  p_header_section->set_parent(this);
#endif

#if 0
  const SgCharList &magic = get_magic();
  std::cout << "SgAsmJvmClassFile::parse(): magic size is " << magic.size() << std::endl;;
#endif

  //erasmus:TODO: JavaClassLoader stuff here I believe (yeah)
  //  1. Read magic, major, minor for now
  //  2. sex = ByteOrder::ORDER_MSB;

  std::cout << "WARNING: SgAsmJvmClassFile::parse():1\n";

  std::cout << "SgAsmJvmClassFile::parse() finished ...\n";

  //erasmus:NOTE: Don't need disk format because it's the only one

#if 0
    /* Read 32-bit header for now. Might need to re-read as 64-bit later. */
    Jvm32FileHeader_disk disk32;
    if (sizeof(disk32)>get_size())
        extend(sizeof(disk32)-get_size());
    read_content_local(0, &disk32, sizeof disk32, false); /*zero pad if we read EOF*/

    /* Check magic number early */
    if (disk32.e_ident_magic[0]!=0xCA || disk32.e_ident_magic[1]!=0xFE ||
        disk32.e_ident_magic[2]!=0xBA  || disk32.e_ident_magic[3]!=0xBE)
        throw FormatError("Bad Java class file magic number");

    /* File byte order should be 1 or 2. However, we've seen at least one example that left the byte order at zero, implying
     * that it was the native order. We don't have the luxury of decoding the file on the native machine, so in that case we
     * try to infer the byte order by looking at one of the other multi-byte fields of the file. */
    ByteOrder::Endianness sex;
    if (1 == disk32.e_ident_data_encoding) {
        sex = ByteOrder::ORDER_LSB;
    } else if (2==disk32.e_ident_data_encoding) {
        sex = ByteOrder::ORDER_MSB;
    } else if ((disk32.e_type & 0xff00)==0xff00) {
        /* One of the 0xffxx processor-specific flags in native order */
        if ((disk32.e_type & 0x00ff)==0xff)
            throw FormatError("invalid ELF header byte order"); /*ambiguous*/
        sex = ByteOrder::host_order();
    } else if ((disk32.e_type & 0x00ff)==0x00ff) {
        /* One of the 0xffxx processor specific orders in reverse native order */
        sex = ByteOrder::host_order()==ByteOrder::ORDER_LSB ? ByteOrder::ORDER_MSB : ByteOrder::ORDER_LSB;
    } else if ((disk32.e_type & 0xff00)==0) {
        /* One of the low-valued file types in native order */
        if ((disk32.e_type & 0x00ff)==0)
            throw FormatError("invalid ELF header byte order"); /*ambiguous*/
        sex = ByteOrder::host_order();
    } else if ((disk32.e_type & 0x00ff)==0) {
        /* One of the low-valued file types in reverse native order */
        sex = ByteOrder::host_order() == ByteOrder::ORDER_LSB ? ByteOrder::ORDER_MSB : ByteOrder::ORDER_LSB;
    } else {
        /* Ambiguous order */
        throw FormatError("invalid ELF header byte order");
    }
    ROSE_ASSERT(p_exec_format != NULL);
    p_exec_format->set_sex(sex);
    p_e_ident_data_encoding = disk32.e_ident_data_encoding; /*save original value*/

    /* Decode header to native format */
    rose_rva_t entry_rva, sectab_rva, segtab_rva;
    if (1 == disk32.e_ident_file_class) {
        p_exec_format->set_word_size(4);

        p_e_ident_padding.clear();
        for (size_t i=0; i<sizeof(disk32.e_ident_padding); i++)
             p_e_ident_padding.push_back(disk32.e_ident_padding[i]);

        p_e_ident_file_class    = ByteOrder::disk_to_host(sex, disk32.e_ident_file_class);
        p_e_ident_file_version  = ByteOrder::disk_to_host(sex, disk32.e_ident_file_version);
        p_e_type                = ByteOrder::disk_to_host(sex, disk32.e_type);
        p_e_machine             = ByteOrder::disk_to_host(sex, disk32.e_machine);
        p_exec_format->set_version(ByteOrder::disk_to_host(sex, disk32.e_version));
        entry_rva               = ByteOrder::disk_to_host(sex, disk32.e_entry);
        segtab_rva              = ByteOrder::disk_to_host(sex, disk32.e_phoff);
        sectab_rva              = ByteOrder::disk_to_host(sex, disk32.e_shoff);
        p_e_flags               = ByteOrder::disk_to_host(sex, disk32.e_flags);
        p_e_ehsize              = ByteOrder::disk_to_host(sex, disk32.e_ehsize);

        p_e_phnum               = ByteOrder::disk_to_host(sex, disk32.e_phnum);
        if (p_e_phnum>0) {
            p_phextrasz         = ByteOrder::disk_to_host(sex, disk32.e_phentsize);
            ROSE_ASSERT(p_phextrasz>=sizeof(SgAsmElfSegmentTableEntry::Elf32SegmentTableEntry_disk));
            p_phextrasz -= sizeof(SgAsmElfSegmentTableEntry::Elf32SegmentTableEntry_disk);
        } else {
            p_phextrasz = 0;
        }

        p_e_shnum               = ByteOrder::disk_to_host(sex, disk32.e_shnum);
        if (p_e_shnum>0) {
            p_shextrasz         = ByteOrder::disk_to_host(sex, disk32.e_shentsize);
            ROSE_ASSERT(p_shextrasz>=sizeof(SgAsmElfSectionTableEntry::Elf32SectionTableEntry_disk));
            p_shextrasz -= sizeof(SgAsmElfSectionTableEntry::Elf32SectionTableEntry_disk);
        } else {
            p_shextrasz = 0;
        }

        p_e_shstrndx            = ByteOrder::disk_to_host(sex, disk32.e_shstrndx);
    } else if (2 == disk32.e_ident_file_class) {
        /* We guessed wrong. This is a 64-bit header, not 32-bit. */
        p_exec_format->set_word_size(8);
        Elf64FileHeader_disk disk64;
        if (sizeof(disk64)>get_size())
            extend(sizeof(disk64)-get_size());
        read_content_local(0, &disk64, sizeof disk64, false); /*zero pad at EOF*/

        p_e_ident_padding.clear();
        for (size_t i=0; i<sizeof(disk64.e_ident_padding); i++)
             p_e_ident_padding.push_back(disk64.e_ident_padding[i]);

        p_e_ident_file_class    = ByteOrder::disk_to_host(sex, disk64.e_ident_file_class);
        p_e_ident_file_version  = ByteOrder::disk_to_host(sex, disk64.e_ident_file_version);
        p_e_type                = ByteOrder::disk_to_host(sex, disk64.e_type);
        p_e_machine             = ByteOrder::disk_to_host(sex, disk64.e_machine);
        p_exec_format->set_version(ByteOrder::disk_to_host(sex, disk64.e_version));
        entry_rva               = ByteOrder::disk_to_host(sex, disk64.e_entry);
        segtab_rva              = ByteOrder::disk_to_host(sex, disk64.e_phoff);
        sectab_rva              = ByteOrder::disk_to_host(sex, disk64.e_shoff);
        p_e_flags               = ByteOrder::disk_to_host(sex, disk64.e_flags);
        p_e_ehsize              = ByteOrder::disk_to_host(sex, disk64.e_ehsize);

        p_e_phnum               = ByteOrder::disk_to_host(sex, disk64.e_phnum);
        if (p_e_phnum>0) {
            p_phextrasz         = ByteOrder::disk_to_host(sex, disk64.e_phentsize);
            ROSE_ASSERT(p_phextrasz>=sizeof(SgAsmElfSegmentTableEntry::Elf64SegmentTableEntry_disk));
            p_phextrasz -= sizeof(SgAsmElfSegmentTableEntry::Elf64SegmentTableEntry_disk);
        } else {
            p_phextrasz = 0;
        }

        p_e_shnum               = ByteOrder::disk_to_host(sex, disk64.e_shnum);
        if (p_e_shnum>0) {
            p_shextrasz         = ByteOrder::disk_to_host(sex, disk64.e_shentsize);
            ROSE_ASSERT(p_shextrasz>=sizeof(SgAsmElfSectionTableEntry::Elf64SectionTableEntry_disk));
            p_shextrasz -= sizeof(SgAsmElfSectionTableEntry::Elf64SectionTableEntry_disk);
        } else {
            p_shextrasz = 0;
        }

        p_e_shstrndx            = ByteOrder::disk_to_host(sex, disk64.e_shstrndx);
    } else {
        throw FormatError("invalid JVM header file class");
    }
    
    /* Magic number. disk32 and disk64 have header bytes at same offset */
    p_magic.clear();
    for (size_t i=0; i<sizeof(disk32.e_ident_magic); i++)
        p_magic.push_back(disk32.e_ident_magic[i]);
    
    /* File format */
    p_exec_format->set_family(FAMILY_ELF);
    switch (p_e_type) {
      case 0:
        p_exec_format->set_purpose(PURPOSE_UNSPECIFIED);
        break;
      case 1:
      case 3:
        p_exec_format->set_purpose(PURPOSE_LIBRARY);
        break;
      case 2:
        p_exec_format->set_purpose(PURPOSE_EXECUTABLE);
        break;
      case 4:
        p_exec_format->set_purpose(PURPOSE_CORE_DUMP);
        break;
      default:
        if (p_e_type >= 0xff00 && p_e_type <= 0xffff) {
            p_exec_format->set_purpose(PURPOSE_PROC_SPECIFIC);
        } else {
            p_exec_format->set_purpose(PURPOSE_OTHER);
        }
        break;
    }
    p_exec_format->set_is_current_version(1 == p_exec_format->get_version());
    p_exec_format->set_abi(ABI_UNSPECIFIED);                 /* ELF specifies a target architecture rather than an ABI */
    p_exec_format->set_abi_version(0);

    /* Target architecture */
    set_isa(machine_to_isa(p_e_machine));

    /* Read the optional section and segment tables and the sections to which they point. An empty section or segment table is
     * treated as if it doesn't exist. This seems to be compatible with the loader since the 45-bit "tiny" ELF executable
     * stores a zero in the e_shnum member and a completely invalid value in the e_shoff member. */
    if (sectab_rva>0 && get_e_shnum()>0) {
        SgAsmElfSectionTable *tab = new SgAsmElfSectionTable(this);
        tab->set_offset(sectab_rva.get_rva());
        tab->parse();
    }
    if (segtab_rva>0 && get_e_phnum()>0) {
        SgAsmElfSegmentTable *tab = new SgAsmElfSegmentTable(this);
        tab->set_offset(segtab_rva.get_rva());
        tab->parse();
    }
    
    /* Associate the entry point with a particular section. */
    entry_rva.bind(this);
    add_entry_rva(entry_rva);
#endif
    

#endif //erasmus

  return this;
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

void
SgAsmJvmClassFile::dump(FILE *f, const char *prefix, ssize_t idx) const
{
  char p[4096];
  if (idx>=0) {
    sprintf(p, "%sPvmFileHeader[%zd].", prefix, idx);
  } else {
    sprintf(p, "%sPvmFileHeader.", prefix);
  }
  int w = std::max(1, DUMP_FIELD_WIDTH-(int)strlen(p));

  SgAsmGenericHeader::dump(f, p, -1);
  //  fprintf(f, "%s%-*s = %x\n",       p, w, "magic",          p_magic); // 0x0a00 is for "JVM_FAMILY"
  fprintf(f, "%s%-*s = %u\n",       p, w, "major_version",  p_major_version);
  fprintf(f, "%s%-*s = %u\n",       p, w, "minor_version",  p_minor_version);
}
#endif // SgAsmGenericHeader old stuff

#endif // ROSE_ENABLE_BINARY_ANALYSIS
