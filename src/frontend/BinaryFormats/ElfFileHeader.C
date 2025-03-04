/* ELF File Header (SgAsmElfFileHeader and related classes) */
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include "sage3basic.h"

#include <Rose/Diagnostics.h>
#include <Rose/BinaryAnalysis/Hexdump.h>
#include <Rose/BinaryAnalysis/RelativeVirtualAddress.h>
#include <ROSE_NELMTS.h>

// In order to efficiently (in terms of amount of code) parse a file format that's defined for a different architecture, we
// need to occassionally take addresses of structs that don't follow alignment rules for this architecture.
#if defined(__GNUC__) && __GNUC__ >= 9
#pragma GCC diagnostic ignored "-Waddress-of-packed-member"
#endif

using namespace Rose::Diagnostics;

SgAsmElfFileHeader::SgAsmElfFileHeader(SgAsmGenericFile *f)
    : SgAsmGenericHeader(f) {
    initializeProperties();
    ASSERT_not_null(get_file());
    ASSERT_require(get_size() > 0);

    set_name(new SgAsmBasicString("ELF File Header"));
    set_synthesized(true);
    set_purpose(SP_HEADER);

    /* Magic number */
    get_magic().clear();
    get_magic().push_back(0x7f);
    get_magic().push_back('E');
    get_magic().push_back('L');
    get_magic().push_back('F');

    /* Executable Format */
    ASSERT_not_null(get_executableFormat());
    get_executableFormat()->set_family(FAMILY_ELF);
    get_executableFormat()->set_purpose(PURPOSE_EXECUTABLE);
    get_executableFormat()->set_sex(Rose::BinaryAnalysis::ByteOrder::ORDER_LSB);
    get_executableFormat()->set_wordSize(4);
    get_executableFormat()->set_version(1);
    get_executableFormat()->set_isCurrentVersion(true);
    get_executableFormat()->set_abi(ABI_UNSPECIFIED);
    get_executableFormat()->set_abiVersion(0);

    set_isa(ISA_IA32_386);
    set_e_ident_data_encoding(1);  /*LSB*/
    set_e_ident_padding(SgUnsignedCharList(9, '\0'));
}

bool
SgAsmElfFileHeader::is_ELF(SgAsmGenericFile *file) {
    return isElf(file);
}

bool
SgAsmElfFileHeader::isElf(SgAsmGenericFile *file)
{
    /* Turn off byte reference tracking for the duration of this function. We don't want our testing the file contents to
     * affect the list of bytes that we've already referenced or which we might reference later. */
    bool was_tracking = file->get_trackingReferences();
    file->set_trackingReferences(false);

    try {
        unsigned char magic[4];
        file->readContent(0, magic, sizeof magic);
        if (0x7f!=magic[0] || 'E'!=magic[1] || 'L'!=magic[2] || 'F'!=magic[3])
            throw 1;
    } catch (...) {
        file->set_trackingReferences(was_tracking);
        return false;
    }
    file->set_trackingReferences(was_tracking);
    return true;
}

SgAsmExecutableFileFormat::InsSetArchitecture
SgAsmElfFileHeader::machine_to_isa(unsigned machine) {
    return machineToIsa(machine);
}

// class method
SgAsmExecutableFileFormat::InsSetArchitecture
SgAsmElfFileHeader::machineToIsa(unsigned machine) {
    switch (machine) {                                /* These come from the Portable Formats Specification v1.1 */
        case 0:        return ISA_UNSPECIFIED;
        case 1:        return ISA_ATT_WE_32100;
        case 2:        return ISA_SPARC_Family;
        case 3:        return ISA_IA32_386;
        case 4:        return ISA_M68K_Family;
        case 5:        return ISA_M88K_Family;
        case 7:        return ISA_I860_Family;
        case 8:        return ISA_MIPS_Family;
        case 0x14:     return ISA_PowerPC;
        case 0x15:     return ISA_PowerPC_64bit;
        case 0x28:     return ISA_ARM_Family;
        case 0x3e:     return ISA_X8664_Family;
        case 0xb7:     return ISA_ARM_A64;
      default:
        /*FIXME: There's a whole lot more. See Dan's Elf reader. */
        // DQ (10/12/2008): Need more information to address PowerPC support.
        mlog[WARN] <<"SgAsmElfFileHeader::parse::p_e_machine = " <<machine <<"\n";
        return ISA_OTHER;
    }
}

unsigned
SgAsmElfFileHeader::isa_to_machine(SgAsmExecutableFileFormat::InsSetArchitecture isa) const {
    return isaToMachine(isa);
}

unsigned
SgAsmElfFileHeader::isaToMachine(SgAsmExecutableFileFormat::InsSetArchitecture isa) const {
    switch (isa) {
        case ISA_UNSPECIFIED:
        case ISA_OTHER:        return p_e_machine;
        case ISA_ATT_WE_32100: return 1;
        case ISA_IA32_386:     return 3;
        case ISA_PowerPC:      return 0x14;  /*see note in machine_to_isa()*/
        case ISA_ARM_A64:      return 0xb7;
        default:
            switch (isa & ISA_FAMILY_MASK) {
                case ISA_SPARC_Family: return 2;
                case ISA_M68K_Family:  return 4;
                case ISA_M88K_Family:  return 5;
                case ISA_I860_Family:  return 7;
                case ISA_MIPS_Family:  return 8;
                case ISA_ARM_Family:   return 40;
                case ISA_X8664_Family: return 62;
                default:
                    return p_e_machine;
            }
    }
}

SgAsmElfFileHeader*
SgAsmElfFileHeader::parse()
{
    // The ELF File Header should have been constructed such that SgAsmElfFileHeader::ctor() was called.
    SgAsmGenericHeader::parse();

    /* Read 32-bit header for now. Might need to re-read as 64-bit later. */
    Elf32FileHeader_disk disk32;
    if (sizeof(disk32)>get_size())
        extend(sizeof(disk32)-get_size());
    readContentLocal(0, &disk32, sizeof disk32, false); /*zero pad if we read EOF*/

    /* Check magic number early */
    if (disk32.e_ident_magic[0]!=0x7f || disk32.e_ident_magic[1]!='E' ||
        disk32.e_ident_magic[2]!='L'  || disk32.e_ident_magic[3]!='F')
        throw FormatError("Bad ELF magic number");

    /* File byte order should be 1 or 2. However, we've seen at least one example that left the byte order at zero, implying
     * that it was the native order. We don't have the luxury of decoding the file on the native machine, so in that case we
     * try to infer the byte order by looking at one of the other multi-byte fields of the file. */
    Rose::BinaryAnalysis::ByteOrder::Endianness sex;
    if (1 == disk32.e_ident_data_encoding) {
        sex = Rose::BinaryAnalysis::ByteOrder::ORDER_LSB;
    } else if (2==disk32.e_ident_data_encoding) {
        sex = Rose::BinaryAnalysis::ByteOrder::ORDER_MSB;
    } else if ((disk32.e_type & 0xff00)==0xff00) {
        /* One of the 0xffxx processor-specific flags in native order */
        if ((disk32.e_type & 0x00ff)==0xff)
            throw FormatError("invalid ELF header byte order"); /*ambiguous*/
        sex = Rose::BinaryAnalysis::ByteOrder::hostOrder();
    } else if ((disk32.e_type & 0x00ff)==0x00ff) {
        /* One of the 0xffxx processor specific orders in reverse native order */
        sex = Rose::BinaryAnalysis::ByteOrder::hostOrder()==Rose::BinaryAnalysis::ByteOrder::ORDER_LSB ? Rose::BinaryAnalysis::ByteOrder::ORDER_MSB : Rose::BinaryAnalysis::ByteOrder::ORDER_LSB;
    } else if ((disk32.e_type & 0xff00)==0) {
        /* One of the low-valued file types in native order */
        if ((disk32.e_type & 0x00ff)==0)
            throw FormatError("invalid ELF header byte order"); /*ambiguous*/
        sex = Rose::BinaryAnalysis::ByteOrder::hostOrder();
    } else if ((disk32.e_type & 0x00ff)==0) {
        /* One of the low-valued file types in reverse native order */
        sex = Rose::BinaryAnalysis::ByteOrder::hostOrder() == Rose::BinaryAnalysis::ByteOrder::ORDER_LSB ? Rose::BinaryAnalysis::ByteOrder::ORDER_MSB : Rose::BinaryAnalysis::ByteOrder::ORDER_LSB;
    } else {
        /* Ambiguous order */
        throw FormatError("invalid ELF header byte order");
    }
    ROSE_ASSERT(get_executableFormat() != NULL);
    get_executableFormat()->set_sex(sex);
    p_e_ident_data_encoding = disk32.e_ident_data_encoding; /*save original value*/

    /* Decode header to native format */
    Rose::BinaryAnalysis::RelativeVirtualAddress entry_rva, sectab_rva, segtab_rva;
    if (1 == disk32.e_ident_file_class) {
        get_executableFormat()->set_wordSize(4);

        p_e_ident_padding.clear();
        for (size_t i=0; i<sizeof(disk32.e_ident_padding); i++)
             p_e_ident_padding.push_back(disk32.e_ident_padding[i]);

        p_e_ident_file_class    = Rose::BinaryAnalysis::ByteOrder::diskToHost(sex, disk32.e_ident_file_class);
        p_e_ident_file_version  = Rose::BinaryAnalysis::ByteOrder::diskToHost(sex, disk32.e_ident_file_version);
        p_e_type                = Rose::BinaryAnalysis::ByteOrder::diskToHost(sex, disk32.e_type);
        p_e_machine             = Rose::BinaryAnalysis::ByteOrder::diskToHost(sex, disk32.e_machine);
        get_executableFormat()->set_version(Rose::BinaryAnalysis::ByteOrder::diskToHost(sex, disk32.e_version));
        entry_rva               = Rose::BinaryAnalysis::ByteOrder::diskToHost(sex, disk32.e_entry);
        segtab_rva              = Rose::BinaryAnalysis::ByteOrder::diskToHost(sex, disk32.e_phoff);
        sectab_rva              = Rose::BinaryAnalysis::ByteOrder::diskToHost(sex, disk32.e_shoff);
        p_e_flags               = Rose::BinaryAnalysis::ByteOrder::diskToHost(sex, disk32.e_flags);
        p_e_ehsize              = Rose::BinaryAnalysis::ByteOrder::diskToHost(sex, disk32.e_ehsize);

        p_e_phnum               = Rose::BinaryAnalysis::ByteOrder::diskToHost(sex, disk32.e_phnum);
        if (p_e_phnum>0) {
            p_phextrasz         = Rose::BinaryAnalysis::ByteOrder::diskToHost(sex, disk32.e_phentsize);
            ROSE_ASSERT(p_phextrasz>=sizeof(SgAsmElfSegmentTableEntry::Elf32SegmentTableEntry_disk));
            p_phextrasz -= sizeof(SgAsmElfSegmentTableEntry::Elf32SegmentTableEntry_disk);
        } else {
            p_phextrasz = 0;
        }

        p_e_shnum               = Rose::BinaryAnalysis::ByteOrder::diskToHost(sex, disk32.e_shnum);
        if (p_e_shnum>0) {
            p_shextrasz         = Rose::BinaryAnalysis::ByteOrder::diskToHost(sex, disk32.e_shentsize);
            ROSE_ASSERT(p_shextrasz>=sizeof(SgAsmElfSectionTableEntry::Elf32SectionTableEntry_disk));
            p_shextrasz -= sizeof(SgAsmElfSectionTableEntry::Elf32SectionTableEntry_disk);
        } else {
            p_shextrasz = 0;
        }

        p_e_shstrndx            = Rose::BinaryAnalysis::ByteOrder::diskToHost(sex, disk32.e_shstrndx);
    } else if (2 == disk32.e_ident_file_class) {
        /* We guessed wrong. This is a 64-bit header, not 32-bit. */
        get_executableFormat()->set_wordSize(8);
        Elf64FileHeader_disk disk64;
        if (sizeof(disk64)>get_size())
            extend(sizeof(disk64)-get_size());
        readContentLocal(0, &disk64, sizeof disk64, false); /*zero pad at EOF*/

        p_e_ident_padding.clear();
        for (size_t i=0; i<sizeof(disk64.e_ident_padding); i++)
             p_e_ident_padding.push_back(disk64.e_ident_padding[i]);

        p_e_ident_file_class    = Rose::BinaryAnalysis::ByteOrder::diskToHost(sex, disk64.e_ident_file_class);
        p_e_ident_file_version  = Rose::BinaryAnalysis::ByteOrder::diskToHost(sex, disk64.e_ident_file_version);
        p_e_type                = Rose::BinaryAnalysis::ByteOrder::diskToHost(sex, disk64.e_type);
        p_e_machine             = Rose::BinaryAnalysis::ByteOrder::diskToHost(sex, disk64.e_machine);
        get_executableFormat()->set_version(Rose::BinaryAnalysis::ByteOrder::diskToHost(sex, disk64.e_version));
        entry_rva               = Rose::BinaryAnalysis::ByteOrder::diskToHost(sex, disk64.e_entry);
        segtab_rva              = Rose::BinaryAnalysis::ByteOrder::diskToHost(sex, disk64.e_phoff);
        sectab_rva              = Rose::BinaryAnalysis::ByteOrder::diskToHost(sex, disk64.e_shoff);
        p_e_flags               = Rose::BinaryAnalysis::ByteOrder::diskToHost(sex, disk64.e_flags);
        p_e_ehsize              = Rose::BinaryAnalysis::ByteOrder::diskToHost(sex, disk64.e_ehsize);

        p_e_phnum               = Rose::BinaryAnalysis::ByteOrder::diskToHost(sex, disk64.e_phnum);
        if (p_e_phnum>0) {
            p_phextrasz         = Rose::BinaryAnalysis::ByteOrder::diskToHost(sex, disk64.e_phentsize);
            ROSE_ASSERT(p_phextrasz>=sizeof(SgAsmElfSegmentTableEntry::Elf64SegmentTableEntry_disk));
            p_phextrasz -= sizeof(SgAsmElfSegmentTableEntry::Elf64SegmentTableEntry_disk);
        } else {
            p_phextrasz = 0;
        }

        p_e_shnum               = Rose::BinaryAnalysis::ByteOrder::diskToHost(sex, disk64.e_shnum);
        if (p_e_shnum>0) {
            p_shextrasz         = Rose::BinaryAnalysis::ByteOrder::diskToHost(sex, disk64.e_shentsize);
            ROSE_ASSERT(p_shextrasz>=sizeof(SgAsmElfSectionTableEntry::Elf64SectionTableEntry_disk));
            p_shextrasz -= sizeof(SgAsmElfSectionTableEntry::Elf64SectionTableEntry_disk);
        } else {
            p_shextrasz = 0;
        }

        p_e_shstrndx            = Rose::BinaryAnalysis::ByteOrder::diskToHost(sex, disk64.e_shstrndx);
    } else {
        throw FormatError("invalid ELF header file class");
    }
    
    /* Magic number. disk32 and disk64 have header bytes at same offset */
    p_magic.clear();
    for (size_t i=0; i<sizeof(disk32.e_ident_magic); i++)
        p_magic.push_back(disk32.e_ident_magic[i]);
    
    /* File format */
    get_executableFormat()->set_family(FAMILY_ELF);
    switch (p_e_type) {
      case 0:
        get_executableFormat()->set_purpose(PURPOSE_UNSPECIFIED);
        break;
      case 1:
      case 3:
        get_executableFormat()->set_purpose(PURPOSE_LIBRARY);
        break;
      case 2:
        get_executableFormat()->set_purpose(PURPOSE_EXECUTABLE);
        break;
      case 4:
        get_executableFormat()->set_purpose(PURPOSE_CORE_DUMP);
        break;
      default:
        if (p_e_type >= 0xff00 && p_e_type <= 0xffff) {
            get_executableFormat()->set_purpose(PURPOSE_PROC_SPECIFIC);
        } else {
            get_executableFormat()->set_purpose(PURPOSE_OTHER);
        }
        break;
    }
    get_executableFormat()->set_isCurrentVersion(1 == get_executableFormat()->get_version());
    get_executableFormat()->set_abi(ABI_UNSPECIFIED);                 /* ELF specifies a target architecture rather than an ABI */
    get_executableFormat()->set_abiVersion(0);

    /* Target architecture */
    set_isa(machineToIsa(p_e_machine));

    /* Read the optional section and segment tables and the sections to which they point. An empty section or segment table is
     * treated as if it doesn't exist. This seems to be compatible with the loader since the 45-bit "tiny" ELF executable
     * stores a zero in the e_shnum member and a completely invalid value in the e_shoff member. */
    if (sectab_rva>0 && get_e_shnum()>0) {
        SgAsmElfSectionTable *tab = new SgAsmElfSectionTable(this);
        tab->set_offset(sectab_rva.rva());
        tab->parse();
    }
    if (segtab_rva>0 && get_e_phnum()>0) {
        SgAsmElfSegmentTable *tab = new SgAsmElfSegmentTable(this);
        tab->set_offset(segtab_rva.rva());
        tab->parse();
    }
    
    /* Associate the entry point with a particular section. */
    entry_rva.bindBestSection(this);
    addEntryRva(entry_rva);
    
    return this;
}

uint64_t
SgAsmElfFileHeader::max_page_size() {
    return maximumPageSize();
}

uint64_t
SgAsmElfFileHeader::maximumPageSize()
{
    /* FIXME:
     *    System V max page size is 4k.
     *    IA32 is 4k
     *    x86_64 is 2MB
     * Other systems may vary! */
    return 4*1024;
}

SgAsmGenericSectionPtrList
SgAsmElfFileHeader::get_sectab_sections()
{
    return get_sectionTableSections();
}

SgAsmGenericSectionPtrList
SgAsmElfFileHeader::get_sectionTableSections() {
    SgAsmGenericSectionPtrList retval;
    SgAsmGenericSectionPtrList sections = get_sections()->get_sections();
    for (size_t i=0; i<sections.size(); i++) {
        SgAsmElfSection *elfsec = dynamic_cast<SgAsmElfSection*>(sections[i]);
        if (elfsec && elfsec->get_sectionEntry()!=NULL)
            retval.push_back(elfsec);
    }
    return retval;
}

SgAsmGenericSectionPtrList
SgAsmElfFileHeader::get_segtab_sections() {
    return get_segmentTableSections();
}

SgAsmGenericSectionPtrList
SgAsmElfFileHeader::get_segmentTableSections()
{
    SgAsmGenericSectionPtrList retval;
    SgAsmGenericSectionPtrList sections = get_sections()->get_sections();
    for (size_t i=0; i<sections.size(); i++) {
        SgAsmElfSection *elfsec = dynamic_cast<SgAsmElfSection*>(sections[i]);
        if (elfsec && elfsec->get_segmentEntry()!=NULL)
            retval.push_back(elfsec);
    }
    return retval;
}

void *
SgAsmElfFileHeader::encode(Rose::BinaryAnalysis::ByteOrder::Endianness sex, Elf32FileHeader_disk *disk) const
{
    ROSE_ASSERT(p_magic.size() == ROSE_NELMTS(disk->e_ident_magic));
    for (size_t i=0; i<ROSE_NELMTS(disk->e_ident_magic); i++)
        disk->e_ident_magic[i] = p_magic[i];
    Rose::BinaryAnalysis::ByteOrder::hostToDisk(sex, p_e_ident_file_class, &(disk->e_ident_file_class));
    Rose::BinaryAnalysis::ByteOrder::hostToDisk(sex, p_e_ident_data_encoding, &(disk->e_ident_data_encoding));
    Rose::BinaryAnalysis::ByteOrder::hostToDisk(sex, p_e_ident_file_version, &(disk->e_ident_file_version));
    ROSE_ASSERT(p_e_ident_padding.size() == ROSE_NELMTS(disk->e_ident_padding));
    for (size_t i=0; i<ROSE_NELMTS(disk->e_ident_padding); i++)
        disk->e_ident_padding[i] = p_e_ident_padding[i];
    Rose::BinaryAnalysis::ByteOrder::hostToDisk(sex, p_e_type, &(disk->e_type));
    Rose::BinaryAnalysis::ByteOrder::hostToDisk(sex, p_e_machine, &(disk->e_machine));
    Rose::BinaryAnalysis::ByteOrder::hostToDisk(sex, get_executableFormat()->get_version(), &(disk->e_version));
    Rose::BinaryAnalysis::ByteOrder::hostToDisk(sex, get_entryRva(), &(disk->e_entry));
    if (get_segmentTable()) {
        Rose::BinaryAnalysis::ByteOrder::hostToDisk(sex, get_segmentTable()->get_offset(), &(disk->e_phoff));
    } else {
        Rose::BinaryAnalysis::ByteOrder::hostToDisk(sex, 0, &(disk->e_phoff));
    }
    if (get_sectionTable()) {
        Rose::BinaryAnalysis::ByteOrder::hostToDisk(sex, get_sectionTable()->get_offset(), &(disk->e_shoff));
    } else {
        Rose::BinaryAnalysis::ByteOrder::hostToDisk(sex, 0, &(disk->e_shoff));
    }
    Rose::BinaryAnalysis::ByteOrder::hostToDisk(sex, p_e_flags,               &(disk->e_flags));
    Rose::BinaryAnalysis::ByteOrder::hostToDisk(sex, p_e_ehsize,              &(disk->e_ehsize));

    if (p_e_phnum>0) {
        Rose::BinaryAnalysis::ByteOrder::hostToDisk(sex, p_phextrasz+sizeof(SgAsmElfSegmentTableEntry::Elf32SegmentTableEntry_disk),
                                &(disk->e_phentsize));
    } else {
        Rose::BinaryAnalysis::ByteOrder::hostToDisk(sex, 0, &(disk->e_phentsize));
    }
    if (p_e_shnum>0) {
        Rose::BinaryAnalysis::ByteOrder::hostToDisk(sex, p_shextrasz+sizeof(SgAsmElfSectionTableEntry::Elf32SectionTableEntry_disk),
                                &(disk->e_shentsize));
    } else {
        Rose::BinaryAnalysis::ByteOrder::hostToDisk(sex, 0, &(disk->e_shentsize));
    }
    Rose::BinaryAnalysis::ByteOrder::hostToDisk(sex, p_e_phnum,               &(disk->e_phnum));
    Rose::BinaryAnalysis::ByteOrder::hostToDisk(sex, p_e_shnum,               &(disk->e_shnum));
    Rose::BinaryAnalysis::ByteOrder::hostToDisk(sex, p_e_shstrndx,            &(disk->e_shstrndx));

    return disk;
}
void *
SgAsmElfFileHeader::encode(Rose::BinaryAnalysis::ByteOrder::Endianness sex, Elf64FileHeader_disk *disk) const
{
    ROSE_ASSERT(p_magic.size() == ROSE_NELMTS(disk->e_ident_magic));
    for (size_t i=0; i < ROSE_NELMTS(disk->e_ident_magic); i++)
        disk->e_ident_magic[i] = p_magic[i];
    Rose::BinaryAnalysis::ByteOrder::hostToDisk(sex, p_e_ident_file_class, &(disk->e_ident_file_class));
    Rose::BinaryAnalysis::ByteOrder::hostToDisk(sex, p_e_ident_data_encoding, &(disk->e_ident_data_encoding));
    Rose::BinaryAnalysis::ByteOrder::hostToDisk(sex, p_e_ident_file_version,&(disk->e_ident_file_version));
    ROSE_ASSERT(p_e_ident_padding.size() == ROSE_NELMTS(disk->e_ident_padding));
    for (size_t i=0; i<ROSE_NELMTS(disk->e_ident_padding); i++)
        disk->e_ident_padding[i] = p_e_ident_padding[i];
    Rose::BinaryAnalysis::ByteOrder::hostToDisk(sex, p_e_type, &(disk->e_type));
    Rose::BinaryAnalysis::ByteOrder::hostToDisk(sex, p_e_machine, &(disk->e_machine));
    Rose::BinaryAnalysis::ByteOrder::hostToDisk(sex, get_executableFormat()->get_version(), &(disk->e_version));
    Rose::BinaryAnalysis::ByteOrder::hostToDisk(sex, get_entryRva(),         &(disk->e_entry));
    if (get_segmentTable()) {
        Rose::BinaryAnalysis::ByteOrder::hostToDisk(sex, get_segmentTable()->get_offset(), &(disk->e_phoff));
    } else {
        Rose::BinaryAnalysis::ByteOrder::hostToDisk(sex, 0, &(disk->e_phoff));
    }
    if (get_sectionTable()) {
        Rose::BinaryAnalysis::ByteOrder::hostToDisk(sex, get_sectionTable()->get_offset(), &(disk->e_shoff));
    } else {
        Rose::BinaryAnalysis::ByteOrder::hostToDisk(sex, 0, &(disk->e_shoff));
    }
    Rose::BinaryAnalysis::ByteOrder::hostToDisk(sex, p_e_flags,               &(disk->e_flags));
    Rose::BinaryAnalysis::ByteOrder::hostToDisk(sex, p_e_ehsize,              &(disk->e_ehsize));
    Rose::BinaryAnalysis::ByteOrder::hostToDisk(sex, p_phextrasz+sizeof(SgAsmElfSegmentTableEntry::Elf64SegmentTableEntry_disk),
                            &(disk->e_phentsize));
    Rose::BinaryAnalysis::ByteOrder::hostToDisk(sex, p_e_phnum,               &(disk->e_phnum));
    Rose::BinaryAnalysis::ByteOrder::hostToDisk(sex, p_shextrasz+sizeof(SgAsmElfSectionTableEntry::Elf64SectionTableEntry_disk),
                            &(disk->e_shentsize));
    Rose::BinaryAnalysis::ByteOrder::hostToDisk(sex, p_e_shnum,               &(disk->e_shnum));
    Rose::BinaryAnalysis::ByteOrder::hostToDisk(sex, p_e_shstrndx,            &(disk->e_shstrndx));

    return disk;
}

bool
SgAsmElfFileHeader::reallocate()
{
    /* Reallocate superclass. This also calls reallocate() for all the sections associated with this ELF File Header. */
    bool reallocated = SgAsmGenericHeader::reallocate();

    /* Resize header based on current word size */
    Rose::BinaryAnalysis::Address need;
    if (4==get_wordSize()) {
        need = sizeof(Elf32FileHeader_disk);
    } else if (8==get_wordSize()) {
        need = sizeof(Elf64FileHeader_disk);
    } else {
        throw FormatError("unsupported ELF word size");
    }
    if (need < get_size()) {
        if (isMapped()) {
            ROSE_ASSERT(get_mappedSize()==get_size());
            set_mappedSize(need);
        }
        set_size(need);
        reallocated = true;
    } else if (need > get_size()) {
        get_file()->shiftExtend(this, 0, need-get_size(), SgAsmGenericFile::ADDRSP_ALL, SgAsmGenericFile::ELASTIC_HOLE);
        reallocated = true;
    }

    /* Update ELF-specific file class data member from generic data. */
    switch(get_wordSize()) {
      case 4:
        p_e_ident_file_class = 1;
        break;
      case 8:
        p_e_ident_file_class = 2;
        break;
      default:
        ROSE_ASSERT(!"invalid word size");
        break;
    }

    /* Byte order. According to the spec, valid values are 1 (little-endian) and 2 (big-endian). However, we've seen cases
     * where a value of zero is used to indicate "native" order (loader assumes words are in the order of the machine on which
     * the loader is running, and the ROSE ELF parser determines the order by looking at other fields in the header). Any
     * original value other than 1 or 2 will be written to the new output; otherwise we choose 1 or 2 based on the currently
     * defined byte order. */
    if (p_e_ident_data_encoding==1 || p_e_ident_data_encoding==2) {
        p_e_ident_data_encoding = Rose::BinaryAnalysis::ByteOrder::ORDER_LSB==get_sex() ? 1 : 2;
    }

    /* Update ELF-specific file type from generic data. */
    switch (get_executableFormat()->get_purpose()) {
      case PURPOSE_UNSPECIFIED:
      case PURPOSE_PROC_SPECIFIC:
      case PURPOSE_OS_SPECIFIC:
      case PURPOSE_OTHER:
        /* keep as is */
        break;
      case PURPOSE_LIBRARY:
        if (p_e_type==1 || p_e_type==3) {
            /* keep as is */
        } else {
            p_e_type = 1;
        }
        break;
      case PURPOSE_EXECUTABLE:
        p_e_type = 2;
        break;
      case PURPOSE_CORE_DUMP:
        p_e_type = 4;
    }

    /* Update ELF machine type. */
    p_e_machine = isaToMachine(get_isa());

    /* The ELF header stores its own size */
    p_e_ehsize = get_size();

    return reallocated;
}

void
SgAsmElfFileHeader::unparse(std::ostream &f) const
{
    /* Write unreferenced areas back to the file before anything else. */
    unparseHoles(f);

    /* Write the ELF segment table and segments first since they generally overlap with more specific things which may have
     * been modified when walking the AST. (We generally don't modify segments, just the more specific sections.) */
    if (get_segmentTable()) {
        ROSE_ASSERT(get_segmentTable()->get_header()==this);
        get_segmentTable()->unparse(f);
    }

    /* Write the ELF section table and, indirectly, the sections themselves. */
    if (get_sectionTable()) {
        ROSE_ASSERT(get_sectionTable()->get_header()==this);
        get_sectionTable()->unparse(f);
    }
    
    /* Encode and write the ELF file header */
    Elf32FileHeader_disk disk32;
    Elf64FileHeader_disk disk64;
    void *disk = NULL;
    size_t struct_size = 0;
    if (4 == get_wordSize()) {
        disk = encode(get_sex(), &disk32);
        struct_size = sizeof(disk32);
    } else if (8 == get_wordSize()) {
        disk = encode(get_sex(), &disk64);
        struct_size = sizeof(disk64);
    } else {
        ROSE_ASSERT(!"unsupported word size");
    }
    write(f, 0, struct_size, disk);
}

void
SgAsmElfFileHeader::dump(FILE *f, const char *prefix, ssize_t idx) const
{
    char p[4096];
    if (idx>=0) {
        snprintf(p, sizeof(p), "%sElfFileHeader[%zd].", prefix, idx);
    } else {
        snprintf(p, sizeof(p), "%sElfFileHeader.", prefix);
    }
    int w = std::max(size_t{1}, Rose::DUMP_FIELD_WIDTH - strlen(p));

    SgAsmGenericHeader::dump(f, p, -1);
    const char *class_s = 1==p_e_ident_file_class ? " (32-bit)" :
                          2==p_e_ident_file_class ? " (64-bit)" : "";
    fprintf(f, "%s%-*s = %u%s\n",                           p, w, "e_ident_file_class",     p_e_ident_file_class, class_s);
    fprintf(f, "%s%-*s = %u\n",                             p, w, "e_ident_file_version",   p_e_ident_file_version);
    for (size_t i=0; i < p_e_ident_padding.size(); i++)
        fprintf(f, "%s%-*s = [%" PRIuPTR "] %u\n",                   p, w, "e_ident_padding",     i, p_e_ident_padding[i]);
    fprintf(f, "%s%-*s = %lu\n",                            p, w, "e_type",                 p_e_type);
    fprintf(f, "%s%-*s = %lu\n",                            p, w, "e_machine",              p_e_machine);
    fprintf(f, "%s%-*s = 0x%08lx\n",                        p, w, "e_flags",                p_e_flags);
    fprintf(f, "%s%-*s = 0x%08lx (%lu) bytes\n",            p, w, "e_ehsize",               p_e_ehsize, p_e_ehsize);
    fprintf(f, "%s%-*s = 0x%08lx (%lu) bytes\n",            p, w, "phextrasz",              p_phextrasz, p_phextrasz);
    fprintf(f, "%s%-*s = %lu\n",                            p, w, "e_phnum",                p_e_phnum);
    fprintf(f, "%s%-*s = 0x%08lx (%lu) bytes\n",            p, w, "shextrasz",              p_shextrasz, p_shextrasz);
    fprintf(f, "%s%-*s = %lu\n",                            p, w, "e_shnum",                p_e_shnum);
    fprintf(f, "%s%-*s = %lu\n",                            p, w, "e_shstrndx",             p_e_shstrndx);
    if (get_sectionTable()) {
        fprintf(f, "%s%-*s = [%d] \"%s\"\n",                p, w, "section_table",
                get_sectionTable()->get_id(), get_sectionTable()->get_name()->get_string(true).c_str());
    } else {
        fprintf(f, "%s%-*s = none\n",                       p, w, "section_table");
    }
    if (get_segmentTable()) {
        fprintf(f, "%s%-*s = [%d] \"%s\"\n",                p, w, "segment_table",
                get_segmentTable()->get_id(), get_segmentTable()->get_name()->get_string(true).c_str());
    } else {
        fprintf(f, "%s%-*s = none\n",                       p, w, "segment_table");
    }

    if (variantT() == V_SgAsmElfFileHeader) //unless a base class
        Rose::BinaryAnalysis::hexdump(f, 0, std::string(p)+"data at ", p_data);
}

const char*
SgAsmElfFileHeader::format_name() const {
    return formatName();
}

const char *
SgAsmElfFileHeader::formatName() const {
    return "ELF";
}

SgAsmElfSectionTable*
SgAsmElfFileHeader::get_section_table() const {
    return get_sectionTable();
}

void
SgAsmElfFileHeader::set_section_table(SgAsmElfSectionTable *x) {
    set_sectionTable(x);
}

SgAsmElfSegmentTable*
SgAsmElfFileHeader::get_segment_table() const {
    return get_segmentTable();
}

void
SgAsmElfFileHeader::set_segment_table(SgAsmElfSegmentTable *x) {
    set_segmentTable(x);
}

#endif
