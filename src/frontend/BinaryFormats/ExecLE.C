/* Copyright 2008 Lawrence Livermore National Security, LLC */
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include "sage3basic.h"

#include <ROSE_NELMTS.h>

// In order to efficiently (in terms of amount of code) parse a file format that's defined for a different architecture, we
// need to occassionally take addresses of structs that don't follow alignment rules for this architecture.
#if defined(__GNUC__) && __GNUC__ >= 9
#pragma GCC diagnostic ignored "-Waddress-of-packed-member"
#endif

// DQ (8/21/2008): Now we want to move away from using the older header files (from before we used the IR nodes).
// #include "ExecLE.h"

// namespace Exec {
// namespace LE {

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// LE File Header
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
SgAsmLEFileHeader::SgAsmLEFileHeader(SgAsmGenericFile *f, rose_addr_t offset)
    : SgAsmGenericHeader(f) {
    initializeProperties();

    set_offset(offset);
    set_size(sizeof(LEFileHeader_disk));
    grabContent();

 // DQ (8/16/2008): Added code to set SgAsmPEFileHeader as parent of input SgAsmGenericFile
    f->set_parent(this);

    LEFileHeader_disk fh;
    readContentLocal(0, &fh, sizeof fh);

    /* Check magic number early */
    if (fh.e_magic[0]!='L' ||
        (fh.e_magic[1]!='E' && fh.e_magic[1]!='X'))
        throw FormatError("Bad LE/LX magic number");

    /* Decode file header */
    get_executableFormat()->set_family( fh.e_magic[1]=='E' ? FAMILY_LE : FAMILY_LX );
    const char *section_name = FAMILY_LE == get_executableFormat()->get_family() ? "LE File Header" : "LX File Header";
    set_name(new SgAsmBasicString(section_name));
    set_synthesized(true);
    set_purpose(SP_HEADER);
    p_e_byte_order = Rose::BinaryAnalysis::ByteOrder::leToHost(fh.e_byte_order);
    p_e_word_order = Rose::BinaryAnalysis::ByteOrder::leToHost(fh.e_word_order);
    ROSE_ASSERT(p_e_byte_order == p_e_word_order);
    Rose::BinaryAnalysis::ByteOrder::Endianness sex = 0 == p_e_byte_order ? Rose::BinaryAnalysis::ByteOrder::ORDER_LSB : Rose::BinaryAnalysis::ByteOrder::ORDER_MSB;

    p_e_format_level           = Rose::BinaryAnalysis::ByteOrder::diskToHost(sex, fh.e_format_level);
    p_e_cpu_type               = Rose::BinaryAnalysis::ByteOrder::diskToHost(sex, fh.e_cpu_type);
    p_e_os_type                = Rose::BinaryAnalysis::ByteOrder::diskToHost(sex, fh.e_os_type);
    p_e_module_version         = Rose::BinaryAnalysis::ByteOrder::diskToHost(sex, fh.e_module_version);
    p_e_flags                  = Rose::BinaryAnalysis::ByteOrder::diskToHost(sex, fh.e_flags);
    p_e_npages                 = Rose::BinaryAnalysis::ByteOrder::diskToHost(sex, fh.e_npages);
    p_e_eip_section            = Rose::BinaryAnalysis::ByteOrder::diskToHost(sex, fh.e_eip_section);
    p_e_eip                    = Rose::BinaryAnalysis::ByteOrder::diskToHost(sex, fh.e_eip);
    p_e_esp_section            = Rose::BinaryAnalysis::ByteOrder::diskToHost(sex, fh.e_esp_section);
    p_e_esp                    = Rose::BinaryAnalysis::ByteOrder::diskToHost(sex, fh.e_esp);
    p_e_page_size              = Rose::BinaryAnalysis::ByteOrder::diskToHost(sex, fh.e_page_size);
    if (FAMILY_LE == get_executableFormat()->get_family()) {
        p_e_last_page_size     = Rose::BinaryAnalysis::ByteOrder::diskToHost(sex, fh.e_lps_or_shift);
        p_e_page_offset_shift  = 0;
    } else {
        ROSE_ASSERT(FAMILY_LX == get_executableFormat()->get_family());
        p_e_last_page_size     = 0;
        p_e_page_offset_shift  = Rose::BinaryAnalysis::ByteOrder::diskToHost(sex, fh.e_lps_or_shift);
    }
    p_e_fixup_sect_size        = Rose::BinaryAnalysis::ByteOrder::diskToHost(sex, fh.e_fixup_sect_size);
    p_e_fixup_sect_cksum       = Rose::BinaryAnalysis::ByteOrder::diskToHost(sex, fh.e_fixup_sect_cksum);
    p_e_loader_sect_size       = Rose::BinaryAnalysis::ByteOrder::diskToHost(sex, fh.e_loader_sect_size);
    p_e_loader_sect_cksum      = Rose::BinaryAnalysis::ByteOrder::diskToHost(sex, fh.e_loader_sect_cksum);
    p_e_secttab_rfo            = Rose::BinaryAnalysis::ByteOrder::diskToHost(sex, fh.e_secttab_rfo);
    p_e_secttab_nentries       = Rose::BinaryAnalysis::ByteOrder::diskToHost(sex, fh.e_secttab_nentries);
    p_e_pagetab_rfo            = Rose::BinaryAnalysis::ByteOrder::diskToHost(sex, fh.e_pagetab_rfo);
    p_e_iterpages_offset       = Rose::BinaryAnalysis::ByteOrder::diskToHost(sex, fh.e_iterpages_offset);
    p_e_rsrctab_rfo            = Rose::BinaryAnalysis::ByteOrder::diskToHost(sex, fh.e_rsrctab_rfo);
    p_e_rsrctab_nentries       = Rose::BinaryAnalysis::ByteOrder::diskToHost(sex, fh.e_rsrctab_nentries);
    p_e_resnametab_rfo         = Rose::BinaryAnalysis::ByteOrder::diskToHost(sex, fh.e_resnametab_rfo);
    p_e_entrytab_rfo           = Rose::BinaryAnalysis::ByteOrder::diskToHost(sex, fh.e_entrytab_rfo);
    p_e_fmtdirtab_rfo          = Rose::BinaryAnalysis::ByteOrder::diskToHost(sex, fh.e_fmtdirtab_rfo);
    p_e_fmtdirtab_nentries     = Rose::BinaryAnalysis::ByteOrder::diskToHost(sex, fh.e_fmtdirtab_nentries);
    p_e_fixup_pagetab_rfo      = Rose::BinaryAnalysis::ByteOrder::diskToHost(sex, fh.e_fixup_pagetab_rfo);
    p_e_fixup_rectab_rfo       = Rose::BinaryAnalysis::ByteOrder::diskToHost(sex, fh.e_fixup_rectab_rfo);
    p_e_import_modtab_rfo      = Rose::BinaryAnalysis::ByteOrder::diskToHost(sex, fh.e_import_modtab_rfo);
    p_e_import_modtab_nentries = Rose::BinaryAnalysis::ByteOrder::diskToHost(sex, fh.e_import_modtab_nentries);
    p_e_import_proctab_rfo     = Rose::BinaryAnalysis::ByteOrder::diskToHost(sex, fh.e_import_proctab_rfo);
    p_e_ppcksumtab_rfo         = Rose::BinaryAnalysis::ByteOrder::diskToHost(sex, fh.e_ppcksumtab_rfo);
    p_e_data_pages_offset      = Rose::BinaryAnalysis::ByteOrder::diskToHost(sex, fh.e_data_pages_offset);
    p_e_preload_npages         = Rose::BinaryAnalysis::ByteOrder::diskToHost(sex, fh.e_preload_npages);
    p_e_nonresnametab_offset   = Rose::BinaryAnalysis::ByteOrder::diskToHost(sex, fh.e_nonresnametab_offset);
    p_e_nonresnametab_size     = Rose::BinaryAnalysis::ByteOrder::diskToHost(sex, fh.e_nonresnametab_size);
    p_e_nonresnametab_cksum    = Rose::BinaryAnalysis::ByteOrder::diskToHost(sex, fh.e_nonresnametab_cksum);
    p_e_auto_ds_section        = Rose::BinaryAnalysis::ByteOrder::diskToHost(sex, fh.e_auto_ds_section);
    p_e_debug_info_rfo         = Rose::BinaryAnalysis::ByteOrder::diskToHost(sex, fh.e_debug_info_rfo);
    p_e_debug_info_size        = Rose::BinaryAnalysis::ByteOrder::diskToHost(sex, fh.e_debug_info_size);
    p_e_num_instance_preload   = Rose::BinaryAnalysis::ByteOrder::diskToHost(sex, fh.e_num_instance_preload);
    p_e_num_instance_demand    = Rose::BinaryAnalysis::ByteOrder::diskToHost(sex, fh.e_num_instance_demand);
    p_e_heap_size              = Rose::BinaryAnalysis::ByteOrder::diskToHost(sex, fh.e_heap_size);

    /* Magic number */
    for (size_t i = 0; i < sizeof(fh.e_magic); ++i)
        p_magic.push_back(fh.e_magic[i]);

    /* File format */
    //exec_format.family    = ???; /*set above*/
    get_executableFormat()->set_purpose( HF_MODTYPE_PROG == (p_e_flags & HF_MODTYPE_MASK) ? PURPOSE_EXECUTABLE : PURPOSE_LIBRARY );
    get_executableFormat()->set_sex( sex );
    switch (p_e_os_type) {
      case 0:  get_executableFormat()->set_abi( ABI_UNSPECIFIED ); break;
      case 1:  get_executableFormat()->set_abi( ABI_OS2 );         break;
      case 2:  get_executableFormat()->set_abi( ABI_NT );          break;
      case 3:  get_executableFormat()->set_abi( ABI_MSDOS );       break;
      case 4:  get_executableFormat()->set_abi( ABI_WIN386 );      break;
      default: get_executableFormat()->set_abi( ABI_OTHER );       break;
    }
    get_executableFormat()->set_abiVersion( 0 );
    get_executableFormat()->set_wordSize( 4 );
    get_executableFormat()->set_version( p_e_format_level );
    get_executableFormat()->set_isCurrentVersion( 0 == p_e_format_level );

    /* Target architecture */
    switch (p_e_cpu_type) {
      case 0x01: set_isa(ISA_IA32_286);     break;
      case 0x02: set_isa(ISA_IA32_386);     break;
      case 0x03: set_isa(ISA_IA32_486);     break;
      case 0x04: set_isa(ISA_IA32_Pentium); break;
      case 0x20: set_isa(ISA_I860_860XR);   break; /*N10*/
      case 0x21: set_isa(ISA_I860_860XP);   break; /*N11*/
      case 0x40: set_isa(ISA_MIPS_MarkI);   break; /*R2000, R3000*/
      case 0x41: set_isa(ISA_MIPS_MarkII);  break; /*R6000*/
      case 0x42: set_isa(ISA_MIPS_MarkIII); break; /*R4000*/
      default:   set_isa(ISA_OTHER);        break;
    }

    /* Entry point */
//    entry_rva = ???; /*FIXME: see e_eip and e_eip_section; we must parse section table first */
}

bool
SgAsmLEFileHeader::is_LE(SgAsmGenericFile *file)
{
    return isLe(file);
}

/* Return true if the file looks like it might be an LE or LX file according to the magic number.  The file must contain what
 * appears to be a DOS File Header at address zero, and what appears to be an LE or LX File Header at a file offset specified in
 * part of the DOS File Header (actually, in the bytes that follow the DOS File Header). */
bool
SgAsmLEFileHeader::isLe(SgAsmGenericFile *file)
{
    /* Turn off byte reference tracking for the duration of this function. We don't want our testing the file contents to
     * affect the list of bytes that we've already referenced or which we might reference later. */
    bool was_tracking = file->get_trackingReferences();
    file->set_trackingReferences(false);

    try {
        /* Check DOS File Header magic number at beginning of the file */
        unsigned char dos_magic[2];
        file->readContent(0, dos_magic, sizeof dos_magic);
        if ('M'!=dos_magic[0] || 'Z'!=dos_magic[1])
            throw 1;

        /* Read four-byte offset of potential LE/LX File Header at offset 0x3c */
        uint32_t lfanew_disk;
        file->readContent(0x3c, &lfanew_disk, sizeof lfanew_disk);
        rose_addr_t le_offset = Rose::BinaryAnalysis::ByteOrder::leToHost(lfanew_disk);
        
        /* Look for the LE/LX File Header magic number */
        unsigned char le_magic[4];
        file->readContent(le_offset, le_magic, sizeof le_magic);
        if ('L'!=le_magic[0] || ('E'!=le_magic[1] && 'X'!=le_magic[1]))
            throw 1;
    } catch (...) {
        file->set_trackingReferences(was_tracking);
        return false;
    }
    
    file->set_trackingReferences(was_tracking);
    return true;
}

/* Encode the LE header into disk format */
void *
SgAsmLEFileHeader::encode(Rose::BinaryAnalysis::ByteOrder::Endianness sex, LEFileHeader_disk *disk) const
{
    for (size_t i=0; i<ROSE_NELMTS(disk->e_magic); i++)
        disk->e_magic[i] = get_magic()[i];
    hostToDisk(sex, p_e_byte_order,             &(disk->e_byte_order));
    hostToDisk(sex, p_e_word_order,             &(disk->e_word_order));
    hostToDisk(sex, p_e_format_level,           &(disk->e_format_level));
    hostToDisk(sex, p_e_cpu_type,               &(disk->e_cpu_type));
    hostToDisk(sex, p_e_os_type,                &(disk->e_os_type));
    hostToDisk(sex, p_e_module_version,         &(disk->e_module_version));
    hostToDisk(sex, p_e_flags,                  &(disk->e_flags));
    hostToDisk(sex, p_e_npages,                 &(disk->e_npages));
    hostToDisk(sex, p_e_eip_section,            &(disk->e_eip_section));
    hostToDisk(sex, p_e_eip,                    &(disk->e_eip));
    hostToDisk(sex, p_e_esp_section,            &(disk->e_esp_section));
    hostToDisk(sex, p_e_esp,                    &(disk->e_esp));
    hostToDisk(sex, p_e_page_size,              &(disk->e_page_size));
    if (FAMILY_LE == get_executableFormat()->get_family()) {
        hostToDisk(sex, p_e_last_page_size,     &(disk->e_lps_or_shift));
    } else {
        ROSE_ASSERT(FAMILY_LX == get_executableFormat()->get_family());
        hostToDisk(sex, p_e_page_offset_shift,  &(disk->e_lps_or_shift));
    }
    hostToDisk(sex, p_e_fixup_sect_size,        &(disk->e_fixup_sect_size));
    hostToDisk(sex, p_e_fixup_sect_cksum,       &(disk->e_fixup_sect_cksum));
    hostToDisk(sex, p_e_loader_sect_size,       &(disk->e_loader_sect_size));
    hostToDisk(sex, p_e_loader_sect_cksum,      &(disk->e_loader_sect_cksum));
    hostToDisk(sex, p_e_secttab_rfo,            &(disk->e_secttab_rfo));
    hostToDisk(sex, p_e_secttab_nentries,       &(disk->e_secttab_nentries));
    hostToDisk(sex, p_e_pagetab_rfo,            &(disk->e_pagetab_rfo));
    hostToDisk(sex, p_e_iterpages_offset,       &(disk->e_iterpages_offset));
    hostToDisk(sex, p_e_rsrctab_rfo,            &(disk->e_rsrctab_rfo));
    hostToDisk(sex, p_e_rsrctab_nentries,       &(disk->e_rsrctab_nentries));
    hostToDisk(sex, p_e_resnametab_rfo,         &(disk->e_resnametab_rfo));
    hostToDisk(sex, p_e_entrytab_rfo,           &(disk->e_entrytab_rfo));
    hostToDisk(sex, p_e_fmtdirtab_rfo,          &(disk->e_fmtdirtab_rfo));
    hostToDisk(sex, p_e_fmtdirtab_nentries,     &(disk->e_fmtdirtab_nentries));
    hostToDisk(sex, p_e_fixup_pagetab_rfo,      &(disk->e_fixup_pagetab_rfo));
    hostToDisk(sex, p_e_fixup_rectab_rfo,       &(disk->e_fixup_rectab_rfo));
    hostToDisk(sex, p_e_import_modtab_rfo,      &(disk->e_import_modtab_rfo));
    hostToDisk(sex, p_e_import_modtab_nentries, &(disk->e_import_modtab_nentries));
    hostToDisk(sex, p_e_import_proctab_rfo,     &(disk->e_import_proctab_rfo));
    hostToDisk(sex, p_e_ppcksumtab_rfo,         &(disk->e_ppcksumtab_rfo));
    hostToDisk(sex, p_e_data_pages_offset,      &(disk->e_data_pages_offset));
    hostToDisk(sex, p_e_preload_npages,         &(disk->e_preload_npages));
    hostToDisk(sex, p_e_nonresnametab_offset,   &(disk->e_nonresnametab_offset));
    hostToDisk(sex, p_e_nonresnametab_size,     &(disk->e_nonresnametab_size));
    hostToDisk(sex, p_e_nonresnametab_cksum,    &(disk->e_nonresnametab_cksum));
    hostToDisk(sex, p_e_auto_ds_section,        &(disk->e_auto_ds_section));
    hostToDisk(sex, p_e_debug_info_rfo,         &(disk->e_debug_info_rfo));
    hostToDisk(sex, p_e_debug_info_size,        &(disk->e_debug_info_size));
    hostToDisk(sex, p_e_num_instance_preload,   &(disk->e_num_instance_preload));
    hostToDisk(sex, p_e_num_instance_demand,    &(disk->e_num_instance_demand));
    hostToDisk(sex, p_e_heap_size,              &(disk->e_heap_size));
    return disk;
}

/* Write the LE file header back to disk and all that it references */
void
SgAsmLEFileHeader::unparse(std::ostream &f) const
{
    LEFileHeader_disk fh;
    encode(get_sex(), &fh);
    write(f, 0, sizeof fh, &fh);

    /* The extended DOS header */
    if (get_dos2Header())
        get_dos2Header()->unparse(f);

    /* The section table and all the non-synthesized sections */
    if (get_sectionTable())
        get_sectionTable()->unparse(f);

    /* Sections defined in the file header */
    if (get_pageTable())
        get_pageTable()->unparse(f);
    if (get_residentNameTable())
        get_residentNameTable()->unparse(f);
    if (get_nonresidentNameTable())
        get_nonresidentNameTable()->unparse(f);
    if (get_entryTable())
        get_entryTable()->unparse(f);
    if (get_relocationTable())
        get_relocationTable()->unparse(f);
}

const char *
SgAsmLEFileHeader::format_name() const
{
    return formatName();
}

const char *
SgAsmLEFileHeader::formatName() const
{
    if (FAMILY_LE == get_executableFormat()->get_family()) {
        return "LE";
    } else {
        ROSE_ASSERT(FAMILY_LX == get_executableFormat()->get_family());
        return "LX";
    }
}
    
/* Print some debugging information */
void
SgAsmLEFileHeader::dump(FILE *f, const char *prefix, ssize_t idx) const
{
    char p[4096];
    if (idx>=0) {
        snprintf(p, sizeof(p), "%s%sFileHeader[%zd].", prefix, formatName(), idx);
    } else {
        snprintf(p, sizeof(p), "%s%sFileHeader.", prefix, formatName());
    }

    int w = std::max(size_t{1}, Rose::DUMP_FIELD_WIDTH - strlen(p));

    SgAsmGenericHeader::dump(f, p, -1);
    fprintf(f, "%s%-*s = %u\n",                        p, w, "e_byte_order",             p_e_byte_order);
    fprintf(f, "%s%-*s = %u\n",                        p, w, "e_word_order",             p_e_word_order);
    fprintf(f, "%s%-*s = %u\n",                        p, w, "e_format_level",           p_e_format_level);
    fprintf(f, "%s%-*s = %u\n",                        p, w, "e_cpu_type",               p_e_cpu_type);
    fprintf(f, "%s%-*s = %u\n",                        p, w, "e_os_type",                p_e_os_type);
    fprintf(f, "%s%-*s = %u\n",                        p, w, "e_module_version",         p_e_module_version);
    fprintf(f, "%s%-*s = 0x%08x\n",                    p, w, "e_flags",                  p_e_flags);
    fprintf(f, "%s%-*s = %" PRIu64 "\n",               p, w, "e_npages",                 p_e_npages);
    fprintf(f, "%s%-*s = %u\n",                        p, w, "e_eip_section",            p_e_eip_section);
    fprintf(f, "%s%-*s = 0x%08" PRIx64 "\n",           p, w, "e_eip",                    p_e_eip);
    fprintf(f, "%s%-*s = %u\n",                        p, w, "e_esp_section",            p_e_esp_section);
    fprintf(f, "%s%-*s = 0x%08" PRIx64 "\n",           p, w, "e_esp",                    p_e_esp);
    fprintf(f, "%s%-*s = %" PRIu64 "\n",               p, w, "e_page_size",              p_e_page_size);
    if (FAMILY_LE == get_executableFormat()->get_family()) {
        fprintf(f, "%s%-*s = %u\n",                    p, w, "e_last_page_size",         p_e_last_page_size);
    } else {
        ROSE_ASSERT(FAMILY_LX == get_executableFormat()->get_family());
        fprintf(f, "%s%-*s = %u\n",                    p, w, "e_page_offset_shift",      p_e_page_offset_shift);
    }
    fprintf(f, "%s%-*s = %" PRIu64 "\n",               p, w, "e_fixup_sect_size",        p_e_fixup_sect_size);
    fprintf(f, "%s%-*s = 0x%08x\n",                    p, w, "e_fixup_sect_cksum",       p_e_fixup_sect_cksum);
    fprintf(f, "%s%-*s = %" PRIu64 "\n",               p, w, "e_loader_sect_size",       p_e_loader_sect_size);
    fprintf(f, "%s%-*s = 0x%08x\n",                    p, w, "e_loader_sect_cksum",      p_e_loader_sect_cksum);
    fprintf(f, "%s%-*s = %" PRIu64 " (%" PRIu64 " abs)\n", p, w, "e_secttab_rfo",
                                                       p_e_secttab_rfo, p_e_secttab_rfo+p_offset);
    fprintf(f, "%s%-*s = %u\n",                        p, w, "e_secttab_nentries",       p_e_secttab_nentries);
    fprintf(f, "%s%-*s = %" PRIu64 " (%" PRIu64 " abs)\n", p, w, "e_pagetab_rfo",
                                                       p_e_pagetab_rfo, p_e_pagetab_rfo+p_offset);
    fprintf(f, "%s%-*s = %" PRIu64 "\n",               p, w, "e_iterpages_offset",       p_e_iterpages_offset);
    fprintf(f, "%s%-*s = %" PRIu64 " (%" PRIu64 " abs)\n", p, w, "e_rsrctab_rfo",
                                                       p_e_rsrctab_rfo, p_e_rsrctab_rfo+p_offset);
    fprintf(f, "%s%-*s = %u\n",                        p, w, "e_rsrctab_nentries",       p_e_rsrctab_nentries);
    fprintf(f, "%s%-*s = %" PRIu64 " (%" PRIu64 " abs)\n", p, w, "e_resnametab_rfo",
                                                       p_e_resnametab_rfo, p_e_resnametab_rfo+p_offset);
    fprintf(f, "%s%-*s = %" PRIu64 " (%" PRIu64 " abs)\n", p, w, "e_entrytab_rfo",
                                                       p_e_entrytab_rfo, p_e_entrytab_rfo+p_offset);
    fprintf(f, "%s%-*s = %" PRIu64 " (%" PRIu64 " abs)\n", p, w, "e_fmtdirtab_rfo",
                                                       p_e_fmtdirtab_rfo, p_e_fmtdirtab_rfo+p_offset);
    fprintf(f, "%s%-*s = %u\n",                        p, w, "e_fmtdirtab_nentries",     p_e_fmtdirtab_nentries);
    fprintf(f, "%s%-*s = %" PRIu64 " (%" PRIu64 " abs)\n", p, w, "e_fixup_pagetab_rfo",
                                                       p_e_fixup_pagetab_rfo, p_e_fixup_pagetab_rfo+p_offset);
    fprintf(f, "%s%-*s = %" PRIu64 " (%" PRIu64 " abs)\n", p, w, "e_fixup_rectab_rfo",
                                                       p_e_fixup_rectab_rfo, p_e_fixup_rectab_rfo+p_offset);
    fprintf(f, "%s%-*s = %" PRIu64 " (%" PRIu64 " abs)\n", p, w, "e_import_modtab_rfo",
                                                       p_e_import_modtab_rfo, p_e_import_modtab_rfo+p_offset);
    fprintf(f, "%s%-*s = %u\n",                        p, w, "e_import_modtab_nentries", p_e_import_modtab_nentries);
    fprintf(f, "%s%-*s = %" PRIu64 " (%" PRIu64 " abs)\n", p, w, "e_import_proctab_rfo",
                                                       p_e_import_proctab_rfo, p_e_import_proctab_rfo+p_offset);
    fprintf(f, "%s%-*s = %" PRIu64 " (%" PRIu64 " abs)\n", p, w, "e_ppcksumtab_rfo",
                                                       p_e_ppcksumtab_rfo, p_e_ppcksumtab_rfo+p_offset);
    fprintf(f, "%s%-*s = %" PRIu64 "\n",                 p, w, "e_data_pages_offset",      p_e_data_pages_offset);
    fprintf(f, "%s%-*s = %u\n",                        p, w, "e_preload_npages",         p_e_preload_npages);
    fprintf(f, "%s%-*s = %" PRIu64 "\n",                 p, w, "e_nonresnametab_offset",   p_e_nonresnametab_offset);
    fprintf(f, "%s%-*s = %u\n",                        p, w, "e_nonresnametab_size",     p_e_nonresnametab_size);
    fprintf(f, "%s%-*s = 0x%08x\n",                    p, w, "e_nonresnametab_cksum",    p_e_nonresnametab_cksum);
    fprintf(f, "%s%-*s = %u\n",                        p, w, "e_auto_ds_section",        p_e_auto_ds_section);
    fprintf(f, "%s%-*s = %" PRIu64 " (%" PRIu64 " abs)\n", p, w, "e_debug_info_rfo",
                                                       p_e_debug_info_rfo, p_e_debug_info_rfo+p_offset);
    fprintf(f, "%s%-*s = %u\n",                        p, w, "e_debug_info_size",        p_e_debug_info_size);
    fprintf(f, "%s%-*s = %u\n",                        p, w, "e_num_instance_preload",   p_e_num_instance_preload);
    fprintf(f, "%s%-*s = %u\n",                        p, w, "e_num_instance_demand",    p_e_num_instance_demand);
    fprintf(f, "%s%-*s = %u\n",                        p, w, "e_heap_size",              p_e_heap_size);

    if (get_dos2Header()) {
        fprintf(f, "%s%-*s = [%d] \"%s\"\n", p, w, "dos2_header",
                get_dos2Header()->get_id(), get_dos2Header()->get_name()->get_string(true).c_str());
    } else {
        fprintf(f, "%s%-*s = none\n", p, w, "dos2_header");
    }
    if (get_sectionTable()) {
        fprintf(f, "%s%-*s = [%d] \"%s\"\n", p, w, "section_table",
                get_sectionTable()->get_id(), get_sectionTable()->get_name()->get_string(true).c_str());
    } else {
        fprintf(f, "%s%-*s = none\n", p, w, "section_table");
    }
    if (get_pageTable()) {
        fprintf(f, "%s%-*s = [%d] \"%s\"\n", p, w, "page_table",
                get_pageTable()->get_id(), get_pageTable()->get_name()->get_string(true).c_str());
    } else {
        fprintf(f, "%s%-*s = none\n", p, w, "page_table");
    }
    if (get_residentNameTable()) {
        fprintf(f, "%s%-*s = [%d] \"%s\"\n", p, w, "resname_table",
                get_residentNameTable()->get_id(), get_residentNameTable()->get_name()->get_string(true).c_str());
    } else {
        fprintf(f, "%s%-*s = none\n", p, w, "resname_table");
    }
    if (get_nonresidentNameTable()) {
        fprintf(f, "%s%-*s = [%d] \"%s\"\n", p, w, "nonresname_table",
                get_nonresidentNameTable()->get_id(), get_nonresidentNameTable()->get_name()->get_string(true).c_str());
    } else {
        fprintf(f, "%s%-*s = none\n", p, w, "nonresname_table");
    }
    if (get_entryTable()) {
        fprintf(f, "%s%-*s = [%d] \"%s\"\n", p, w, "entry_table",
                get_entryTable()->get_id(), get_entryTable()->get_name()->get_string(true).c_str());
    } else {
        fprintf(f, "%s%-*s = none\n", p, w, "entry_table");
    }
    if (get_relocationTable()) {
        fprintf(f, "%s%-*s = [%d] \"%s\"\n", p, w, "reloc_table",
                get_relocationTable()->get_id(), get_relocationTable()->get_name()->get_string(true).c_str());
    } else {
        fprintf(f, "%s%-*s = none\n", p, w, "reloc_table");
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// LE/LX Page Table
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* Constructor */
SgAsmLEPageTableEntry::SgAsmLEPageTableEntry(Rose::BinaryAnalysis::ByteOrder::Endianness sex, const SgAsmLEPageTableEntry::LEPageTableEntry_disk *disk) {
    initializeProperties();

    unsigned pageno_lo = Rose::BinaryAnalysis::ByteOrder::diskToHost(sex, disk->pageno_lo);
    unsigned pageno_hi = Rose::BinaryAnalysis::ByteOrder::diskToHost(sex, disk->pageno_hi);
    p_pageno      = (pageno_hi << 8) | pageno_lo;
    p_flags       = Rose::BinaryAnalysis::ByteOrder::diskToHost(sex, disk->flags);
}

/* Encode page table entry to disk format */
void *
SgAsmLEPageTableEntry::encode(Rose::BinaryAnalysis::ByteOrder::Endianness sex, SgAsmLEPageTableEntry::LEPageTableEntry_disk *disk) const
{
    hostToDisk(sex, (p_pageno & 0xff),    &(disk->pageno_lo));
    hostToDisk(sex, (p_pageno>>8)&0xffff, &(disk->pageno_hi));
    hostToDisk(sex, p_flags,              &(disk->flags));
    return disk;
}

/* Print some debugging information */
void
SgAsmLEPageTableEntry::dump(FILE *f, const char *prefix, ssize_t idx) const
{
    char p[4096];
    if (idx>=0) {
        snprintf(p, sizeof(p), "%sPageTableEntry[%zd].", prefix, idx);
    } else {
        snprintf(p, sizeof(p), "%sPageTableEntry.", prefix);
    }

    int w = std::max(size_t{1}, Rose::DUMP_FIELD_WIDTH - strlen(p));

    fprintf(f, "%s%-*s = 0x%04x\n", p, w, "flags",  p_flags);
    fprintf(f, "%s%-*s = %u\n",     p, w, "pageno", p_pageno);
}

/* Constructor */
SgAsmLEPageTable::SgAsmLEPageTable(SgAsmLEFileHeader *fhdr, rose_addr_t offset, rose_addr_t size)
    : SgAsmGenericSection(fhdr->get_file(), fhdr) {
    initializeProperties();

    set_offset(offset);
    set_size(size);
    grabContent();

    char section_name[64];
    snprintf(section_name, sizeof(section_name), "%s Page Table", fhdr->formatName());
    set_synthesized(true);
    set_name(new SgAsmBasicString(section_name));
    set_purpose(SP_HEADER);

    const rose_addr_t entry_size = sizeof(SgAsmLEPageTableEntry::LEPageTableEntry_disk);
    for (rose_addr_t entry_offset=0; entry_offset+entry_size <= get_size(); entry_offset+=entry_size) {
        SgAsmLEPageTableEntry::LEPageTableEntry_disk disk;
        readContentLocal(entry_offset, &disk, entry_size);
        p_entries.push_back(new SgAsmLEPageTableEntry(fhdr->get_sex(), &disk));
    }
}

/* Returns info about a particular page. Indices are 1-origin */
SgAsmLEPageTableEntry *
SgAsmLEPageTable::get_page(size_t idx)
{
    ROSE_ASSERT(idx > 0);
    ROSE_ASSERT(idx <= p_entries.size());
    return p_entries[idx-1];
}

/* Write page table back to disk */
void
SgAsmLEPageTable::unparse(std::ostream &f) const
{
    rose_addr_t spos=0; /*section offset*/
    for (size_t i=0; i < p_entries.size(); i++) {
        SgAsmLEPageTableEntry::LEPageTableEntry_disk disk;
        p_entries[i]->encode(get_header()->get_sex(), &disk);
        spos = write(f, spos, sizeof disk, &disk);
    }
}

/* Print some debugging information */
void
SgAsmLEPageTable::dump(FILE *f, const char *prefix, ssize_t idx) const
{
    char p[4096];
    if (idx>=0) {
        snprintf(p, sizeof(p), "%s%sPageTable[%zd].", prefix, get_header()->formatName(), idx);
    } else {
        snprintf(p, sizeof(p), "%s%sPageTable.", prefix, get_header()->formatName());
    }

    SgAsmGenericSection::dump(f, p, -1);
    for (size_t i = 0; i < p_entries.size(); i++) {
        p_entries[i]->dump(f, p, i);
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// LE/LX Section Table
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* Constructor */
SgAsmLESectionTableEntry::SgAsmLESectionTableEntry(Rose::BinaryAnalysis::ByteOrder::Endianness sex, const LESectionTableEntry_disk *disk) {
    initializeProperties();

    p_mappedSize      = Rose::BinaryAnalysis::ByteOrder::diskToHost(sex, disk->mapped_size);
    p_baseAddr        = Rose::BinaryAnalysis::ByteOrder::diskToHost(sex, disk->base_addr);
    p_flags            = Rose::BinaryAnalysis::ByteOrder::diskToHost(sex, disk->flags);
    p_pageMapIndex    = Rose::BinaryAnalysis::ByteOrder::diskToHost(sex, disk->pagemap_index);
    p_pageMapNEntries = Rose::BinaryAnalysis::ByteOrder::diskToHost(sex, disk->pagemap_nentries);
    p_res1             = Rose::BinaryAnalysis::ByteOrder::diskToHost(sex, disk->res1);
}

/* Encodes a section table entry back into disk format. */
void *
SgAsmLESectionTableEntry::encode(Rose::BinaryAnalysis::ByteOrder::Endianness sex, LESectionTableEntry_disk *disk) const
{
    Rose::BinaryAnalysis::ByteOrder::hostToDisk(sex, p_mappedSize,      &(disk->mapped_size));
    Rose::BinaryAnalysis::ByteOrder::hostToDisk(sex, p_baseAddr,        &(disk->base_addr));
    Rose::BinaryAnalysis::ByteOrder::hostToDisk(sex, p_flags,            &(disk->flags));
    Rose::BinaryAnalysis::ByteOrder::hostToDisk(sex, p_pageMapIndex,    &(disk->pagemap_index));
    Rose::BinaryAnalysis::ByteOrder::hostToDisk(sex, p_pageMapNEntries, &(disk->pagemap_nentries));
    Rose::BinaryAnalysis::ByteOrder::hostToDisk(sex, p_res1,             &(disk->res1));
    return disk;
}

/* Prints some debugging info */
void
SgAsmLESectionTableEntry::dump(FILE *f, const char *prefix, ssize_t idx) const
{
    char p[4096];
    if (idx>=0) {
        snprintf(p, sizeof(p), "%sLESectionTableEntry[%zd].", prefix, idx);
    } else {
        snprintf(p, sizeof(p), "%sLESectionTableEntry.", prefix);
    }

    const int w = std::max(size_t{1}, Rose::DUMP_FIELD_WIDTH - strlen(p));

    fprintf(f, "%s%-*s = %" PRIu64 " bytes\n", p, w, "mapped_size",      p_mappedSize);
    fprintf(f, "%s%-*s = 0x%08" PRIx64 "\n",   p, w, "base_addr",        p_baseAddr);

    fprintf(f, "%s%-*s = 0x%08x",            p, w, "flags",            p_flags);
    switch (p_flags & SF_TYPE_MASK) {
      case SF_TYPE_NORMAL:   fputs(" normal",   f); break;
      case SF_TYPE_ZERO:     fputs(" zero",     f); break;
      case SF_TYPE_RESIDENT: fputs(" resident", f); break;
      case SF_TYPE_RESCONT:  fputs(" res-cont", f); break;
      default: fprintf(f, "type=%u", p_flags & SF_TYPE_MASK); break;
    }
    fputs(" perm=", f);
    fputc(p_flags & SF_READABLE   ? 'r' : '-', f);
    fputc(p_flags & SF_WRITABLE   ? 'w' : '-', f);
    fputc(p_flags & SF_EXECUTABLE ? 'x' : '-', f);
    if (p_flags & SF_RESOURCE)      fputs(" resource",      f);
    if (p_flags & SF_DISCARDABLE)   fputs(" discardable",   f);
    if (p_flags & SF_SHARED)        fputs(" shared",        f);
    if (p_flags & SF_PRELOAD_PAGES) fputs(" preload",       f);
    if (p_flags & SF_INVALID_PAGES) fputs(" invalid",       f);
    if (p_flags & SF_RES_LONG_LOCK) fputs(" res-long-lock", f);
    if (p_flags & SF_1616_ALIAS)    fputs(" 16:16-alias",   f);
    if (p_flags & SF_BIG_BIT)       fputs(" big-bit",       f);
    if (p_flags & SF_CODE_CONFORM)  fputs(" code-conform",  f);
    if (p_flags & SF_IO_PRIV)       fputs(" io-priv",       f);
    fputc('\n', f);
    
    fprintf(f, "%s%-*s = %u\n",              p, w, "pagemap_index",    p_pageMapIndex);
    fprintf(f, "%s%-*s = %u entries\n",      p, w, "pagemap_nentries", p_pageMapNEntries);
    fprintf(f, "%s%-*s = 0x%08x\n",          p, w, "res1",             p_res1);
}
    
SgAsmLESection::SgAsmLESection(SgAsmLEFileHeader *fhdr)
    : SgAsmGenericSection(fhdr->get_file(), fhdr) {
    initializeProperties();
}

/* Print some debugging info. */
void
SgAsmLESection::dump(FILE *f, const char *prefix, ssize_t idx) const
{
    SgAsmLEFileHeader *fhdr = dynamic_cast<SgAsmLEFileHeader*>(get_header());

    char p[4096];
    if (idx>=0) {
        snprintf(p, sizeof(p), "%s%sSection[%zd].", prefix, fhdr->formatName(), idx);
    } else {
        snprintf(p, sizeof(p), "%s%sSection.", prefix, fhdr->formatName());
    }

    SgAsmGenericSection::dump(f, p, -1);
    get_sectionTableEntry()->dump(f, p, -1);
}

/* Constructor */
SgAsmLESectionTable::SgAsmLESectionTable(SgAsmLEFileHeader *fhdr, rose_addr_t offset, rose_addr_t size)
    : SgAsmGenericSection(fhdr->get_file(), fhdr) {
    initializeProperties();

    set_offset(offset);
    set_size(size);
    grabContent();

    set_synthesized(true);
    char section_name[64];
    snprintf(section_name, sizeof(section_name), "%s Section Table", fhdr->formatName());
    set_name(new SgAsmBasicString(section_name));
    set_purpose(SP_HEADER);

    SgAsmLEPageTable *pages = fhdr->get_pageTable();
    
    const size_t entsize = sizeof(SgAsmLESectionTableEntry::LESectionTableEntry_disk);
    for (size_t i = 0; i < fhdr->get_e_secttab_nentries(); i++) {
        /* Parse the section table entry */
        SgAsmLESectionTableEntry::LESectionTableEntry_disk disk;
        readContentLocal(i*entsize, &disk, entsize);
        SgAsmLESectionTableEntry *entry = new SgAsmLESectionTableEntry(fhdr->get_sex(), &disk);

        /* The section pages in the executable file. For now we require that the entries in the page table for the section
         * being defined are contiguous in the executable file, otherwise we'd have to define more than one actual section to
         * represent this section table entry. */
        rose_addr_t section_offset, section_size; /*offset and size of section within file */
        SgAsmLEPageTableEntry *page = pages->get_page(entry->get_pageMapIndex());
#ifndef NDEBUG
        for (size_t j = 1; j < entry->get_pageMapNEntries(); j++) {
            SgAsmLEPageTableEntry *p2 = pages->get_page(entry->get_pageMapIndex()+j);
            ROSE_ASSERT(page->get_pageno()+j == p2->get_pageno());
        }
#endif
        rose_addr_t pageno = page->get_pageno();
        ROSE_ASSERT(pageno>0);
        if (FAMILY_LE==fhdr->get_executableFormat()->get_family()) {
            section_offset = fhdr->get_e_data_pages_offset() + (pageno-1) * fhdr->get_e_page_size();

            section_size = std::min(entry->get_mappedSize(), entry->get_pageMapNEntries() * fhdr->get_e_page_size());

                } else {
            ROSE_ASSERT(FAMILY_LX==fhdr->get_executableFormat()->get_family());
            section_offset = fhdr->get_e_data_pages_offset() + ((pageno-1) << fhdr->get_e_page_offset_shift());

            section_size = std::min(entry->get_mappedSize(),
                                    (rose_addr_t)(entry->get_pageMapNEntries() * (1<<fhdr->get_e_page_offset_shift())));

                }

        SgAsmLESection *section = new SgAsmLESection(fhdr);
        section->set_offset(section_offset);
        section->set_size(section_size);
        section->parse();
        section->set_synthesized(false);
        section->set_id(i+1); /*numbered starting at 1, not zero*/
        section->set_purpose(SP_PROGRAM);
        section->set_sectionTableEntry(entry);

        /* Section permissions */
        section->set_mappedPreferredRva(entry->get_baseAddr());
        section->set_mappedActualVa(0); /*assigned by Loader*/
        section->set_mappedSize(entry->get_mappedSize());
        section->set_mappedReadPermission((entry->get_flags() & SgAsmLESectionTableEntry::SF_READABLE)
                                          == SgAsmLESectionTableEntry::SF_READABLE);
        section->set_mappedWritePermission((entry->get_flags() & SgAsmLESectionTableEntry::SF_WRITABLE)
                                           == SgAsmLESectionTableEntry::SF_WRITABLE);
        section->set_mappedExecutePermission((entry->get_flags() & SgAsmLESectionTableEntry::SF_EXECUTABLE)
                                             == SgAsmLESectionTableEntry::SF_EXECUTABLE);

        unsigned section_type = entry->get_flags() & SgAsmLESectionTableEntry::SF_TYPE_MASK;
        if (SgAsmLESectionTableEntry::SF_TYPE_ZERO==section_type) {
            section->set_name(new SgAsmBasicString(".bss"));
        } else if (entry->get_flags() & SgAsmLESectionTableEntry::SF_EXECUTABLE) {
            section->set_name(new SgAsmBasicString(".text"));
        }
    }
}

/* Writes the section table back to disk along with each of the sections. */
void
SgAsmLESectionTable::unparse(std::ostream &f) const
{
    SgAsmLEFileHeader *fhdr = dynamic_cast<SgAsmLEFileHeader*>(get_header());
    ROSE_ASSERT(fhdr!=NULL);
    SgAsmGenericSectionPtrList sections = fhdr->get_sections()->get_sections();

    for (size_t i = 0; i < sections.size(); i++) {
        if (sections[i]->get_id() >= 0) {
            SgAsmLESection *section = dynamic_cast<SgAsmLESection*>(sections[i]);

            /* Write the table entry */
            ROSE_ASSERT(section->get_id()>0); /*ID's are 1-origin in LE*/
            size_t slot = section->get_id()-1;
            SgAsmLESectionTableEntry *shdr = section->get_sectionTableEntry();
            SgAsmLESectionTableEntry::LESectionTableEntry_disk disk;
            shdr->encode(get_header()->get_sex(), &disk);
            write(f, slot*sizeof(disk), sizeof disk, &disk);

            /* Write the section */
            section->unparse(f);
        }
    }
}

/* Prints some debugging info */
void
SgAsmLESectionTable::dump(FILE *f, const char *prefix, ssize_t idx) const
{
    char p[4096];
    if (idx>=0) {
        snprintf(p, sizeof(p), "%s%sSectionTable[%zd].", prefix, get_header()->formatName(), idx);
    } else {
        snprintf(p, sizeof(p), "%s%sSectionTable.", prefix, get_header()->formatName());
    }
    SgAsmGenericSection::dump(f, p, -1);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// LE/LX Resident and Non-Resident Name Tables
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* Constructor assumes SgAsmGenericSection is zero bytes long so far */
SgAsmLENameTable::SgAsmLENameTable(SgAsmLEFileHeader *fhdr, rose_addr_t offset)
    : SgAsmGenericSection(fhdr->get_file(), fhdr) {
    initializeProperties();

    set_offset(offset);
    set_size(0);
    grabContent();

    set_synthesized(true);
    char section_name[64];
    snprintf(section_name, sizeof(section_name), "%s Name Table", fhdr->formatName());
    set_name(new SgAsmBasicString(section_name));
    set_purpose(SP_HEADER);

    /* Resident exported procedure names, until we hit a zero length name. The first name
     * is for the library itself and the corresponding ordinal has no meaning. */
    rose_addr_t at = 0;
    while (1) {
        extend(1);
        unsigned char byte;
        readContentLocal(at++, &byte, 1);
        size_t length = byte;
        if (0==length) break;

        extend(length);
        char *buf = new char[length];
        readContentLocal(at, buf, length);
        p_names.push_back(std::string(buf, length));
        delete[] buf;
        at += length;

        extend(2);
        uint16_t u16_disk;
        readContentLocal(at, &u16_disk, 2);
        p_ordinals.push_back(Rose::BinaryAnalysis::ByteOrder::leToHost(u16_disk));
        at += 2;
    }
}

/* Writes the section back to disk. */
void
SgAsmLENameTable::unparse(std::ostream &f) const
{
    rose_addr_t spos=0; /*section offset*/
    ROSE_ASSERT(p_names.size() == p_ordinals.size());
    for (size_t i = 0; i < p_names.size(); i++) {
        /* Name length */
        ROSE_ASSERT(p_names[i].size() <= 0xff);
        unsigned char len = p_names[i].size();
        spos = write(f, spos, len);

        /* Name */
        spos = write(f, spos, p_names[i]);

        /* Ordinal */
        ROSE_ASSERT(p_ordinals[i] <= 0xffff);
        uint16_t ordinal_le;
        Rose::BinaryAnalysis::ByteOrder::hostToLe(p_ordinals[i], &ordinal_le);
        spos = write(f, spos, sizeof ordinal_le, &ordinal_le);
    }
    
    /* Zero-terminated */
    write(f, spos, '\0');
}

/* Prints some debugging info */
void
SgAsmLENameTable::dump(FILE *f, const char *prefix, ssize_t idx) const
{
    char p[4096];
    if (idx>=0) {
        snprintf(p, sizeof(p), "%sLENameTable[%zd].", prefix, idx);
    } else {
        snprintf(p, sizeof(p), "%sLENameTable.", prefix);
    }

        const int w = std::max(size_t{1}, Rose::DUMP_FIELD_WIDTH - strlen(p));

    SgAsmGenericSection::dump(f, p, -1);
    ROSE_ASSERT(p_names.size() == p_ordinals.size());
    for (size_t i = 0; i < p_names.size(); i++) {
        fprintf(f, "%s%-*s = [%zd] \"%s\"\n", p, w, "names",    i, escapeString(p_names[i]).c_str());
        fprintf(f, "%s%-*s = [%zd] %u\n",     p, w, "ordinals", i, p_ordinals[i]);
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// LE/LX Entry Table
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* Constructor */
SgAsmLEEntryPoint::SgAsmLEEntryPoint(Rose::BinaryAnalysis::ByteOrder::Endianness sex, const SgAsmLEEntryPoint::LEEntryPoint_disk *disk) {
    initializeProperties();
    ASSERT_not_null(disk);
    p_flags        = Rose::BinaryAnalysis::ByteOrder::diskToHost(sex, disk->flags);
    p_objnum       = Rose::BinaryAnalysis::ByteOrder::diskToHost(sex, disk->objnum);
    p_entry_type   = Rose::BinaryAnalysis::ByteOrder::diskToHost(sex, disk->entry_type);
    p_entry_offset = Rose::BinaryAnalysis::ByteOrder::diskToHost(sex, disk->entry_offset);
    p_res1         = Rose::BinaryAnalysis::ByteOrder::diskToHost(sex, disk->res1);
}

SgAsmLEEntryPoint::SgAsmLEEntryPoint(Rose::BinaryAnalysis::ByteOrder::Endianness, unsigned /*flags*/) {
    initializeProperties();
}

/* Write the entry information back to the disk at the specified section and section offset, returning the new section offset. */
rose_addr_t
SgAsmLEEntryPoint::unparse(std::ostream &f, Rose::BinaryAnalysis::ByteOrder::Endianness sex, const SgAsmGenericSection *section,
                           rose_addr_t spos) const
{
    if (0==(p_flags & 0x01)) {
        /* Empty entry; write only the flag byte */
        uint8_t byte;
        Rose::BinaryAnalysis::ByteOrder::hostToDisk(sex, p_flags, &byte);
        spos = section->write(f, spos, byte);
    } else {
        /* Non-empty entry */
        LEEntryPoint_disk disk;
        Rose::BinaryAnalysis::ByteOrder::hostToDisk(sex, p_flags,        &(disk.flags));
        Rose::BinaryAnalysis::ByteOrder::hostToDisk(sex, p_objnum,       &(disk.objnum));
        Rose::BinaryAnalysis::ByteOrder::hostToDisk(sex, p_entry_type,   &(disk.entry_type));
        Rose::BinaryAnalysis::ByteOrder::hostToDisk(sex, p_entry_offset, &(disk.entry_offset));
        Rose::BinaryAnalysis::ByteOrder::hostToDisk(sex, p_res1,         &(disk.res1));
        spos = section->write(f, spos, sizeof disk, &disk);
    }
    return spos;
}

/* Print some debugging info */
void
SgAsmLEEntryPoint::dump(FILE *f, const char *prefix, ssize_t idx) const
{
    char p[4096];
    if (idx>=0) {
        snprintf(p, sizeof(p), "%sEntryPoint[%zd].", prefix, idx);
    } else {
        snprintf(p, sizeof(p), "%sEntryPoint.", prefix);
    }

    const int w = std::max(size_t{1}, Rose::DUMP_FIELD_WIDTH - strlen(p));

    fprintf(f, "%s%-*s = 0x%02x",    p, w, "flags",        p_flags);
    if (p_flags & 0x01)
        fprintf(f, " 32-bit");
    if (p_flags & 0x01) {
        fprintf(f, " non-empty\n");
        fprintf(f, "%s%-*s = %u\n", p, w, "objnum",       p_objnum);
        fprintf(f, "%s%-*s = 0x%02x", p, w, "entry_type",   p_entry_type);
        if (p_entry_type & 0x01) fputs(" exported", f);
        if (p_entry_type & 0x02) fputs(" shared-data", f);
        fprintf(f, " stack-params=%u\n", (p_entry_type >> 3) & 0x1f);
        fprintf(f, "%s%-*s = %" PRIu64 "\n", p, w, "entry_offset", p_entry_offset);
        fprintf(f, "%s%-*s = 0x%04x\n",    p, w, "res1",         p_res1);
    } else {
        fprintf(f, " empty\n");
    }
}

/* Constructor. We don't know the size of the LE Entry table until after reading the first byte. Therefore the SgAsmGenericSection is
 * created with an initial size of zero. */
SgAsmLEEntryTable::SgAsmLEEntryTable(SgAsmLEFileHeader *fhdr, rose_addr_t offset)
    : SgAsmGenericSection(fhdr->get_file(), fhdr) {
    initializeProperties();

    set_offset(offset);
    set_size(0);
    grabContent();

    set_synthesized(true);
    char section_name[64];
    snprintf(section_name, sizeof(section_name), "%s Entry Table", fhdr->formatName());
    set_name(new SgAsmBasicString(section_name));
    set_purpose(SP_HEADER);

    ROSE_ASSERT(0 == get_size());

    if (FAMILY_LX == fhdr->get_executableFormat()->get_family()) {
        /* FIXME: LX Entry tables have a different format than LE (they are similar to NE Entry Tables). See
         *        http://members.rediff.com/pguptaji/executable.htm (among others) for the format. We don't parse them
         *        at this time since it's not a Windows format and we leave the section size at zero to make this more
         *        obvious. */
        return;
    }

    rose_addr_t at = 0;
    extend(1);
    unsigned char byte;
    readContentLocal(at++, &byte, 1);
    size_t nentries = byte;
    for (size_t i = 0; i < nentries; i++) {
        extend(1);
        uint8_t flags;
        readContentLocal(at, &flags, 1);
        if (flags & 0x01) {
            extend(sizeof(SgAsmLEEntryPoint::LEEntryPoint_disk)-1);
            SgAsmLEEntryPoint::LEEntryPoint_disk disk;
            readContentLocal(at, &disk, sizeof disk);
            p_entries.push_back(new SgAsmLEEntryPoint(fhdr->get_sex(), &disk));
        } else {
            p_entries.push_back(new SgAsmLEEntryPoint(fhdr->get_sex(), flags));
        }
    }
}

/* Write entry table back to file */
void
SgAsmLEEntryTable::unparse(std::ostream &f) const
{
    rose_addr_t spos=0; /*section offset*/
    ROSE_ASSERT(p_entries.size()<=0xff);
    uint8_t byte = p_entries.size();
    spos = write(f, spos, byte);
    
    Rose::BinaryAnalysis::ByteOrder::Endianness sex = get_header()->get_sex();
    for (size_t i = 0; i < p_entries.size(); i++) {
        spos = p_entries[i]->unparse(f, sex, this, spos);
    }
}

/* Print some debugging info */
void
SgAsmLEEntryTable::dump(FILE *f, const char *prefix, ssize_t idx) const
{
    char p[4096];
    if (idx>=0) {
        snprintf(p, sizeof(p), "%s%sEntryTable[%zd].", prefix, get_header()->formatName(), idx);
    } else {
        snprintf(p, sizeof(p), "%s%sEntryTable.", prefix, get_header()->formatName());
    }

    const int w = std::max(size_t{1}, Rose::DUMP_FIELD_WIDTH - strlen(p));

    SgAsmGenericSection::dump(f, p, -1);
    fprintf(f, "%s%-*s = %" PRIuPTR " entry points\n", p, w, "size", p_entries.size());
    for (size_t i = 0; i < p_entries.size(); i++) {
        p_entries[i]->dump(f, p, i);
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// LE/LX Relocation Table
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* Constructor. */
SgAsmLERelocTable::SgAsmLERelocTable(SgAsmLEFileHeader *fhdr, rose_addr_t offset)
    : SgAsmGenericSection(fhdr->get_file(), fhdr) {
    initializeProperties();

    set_offset(offset);
    set_size(0);
    grabContent();

    char name[64];
    snprintf(name, sizeof(name), "%s Relocation Table", fhdr->formatName());
    set_synthesized(true);
    set_name(new SgAsmBasicString(name));
    set_purpose(SP_HEADER);

    ROSE_ASSERT(0 == get_size());

#if 0 /*FIXME: How do we know how many entries are in the relocation table? */
    size_t nrelocs = 0;

 // DQ (12/8/2008): reloc_size was previously not initialized before use in the for loop.
    rose_addr_t at = 0, reloc_size = 0;
    for (size_t i = 0; i < nrelocs; i++, at+=reloc_size) {
        p_entries.push_back(new SgAsmLERelocEntry(this, at, &reloc_size));
    }
#endif
}

#if 0 /*FIXME: not implemented yet*/
/* Write relocation table back to disk */
void
SgAsmLERelocTable::unparse(std::ostream &f)
{
    ROSE_ASSERT(0==reallocate(false)); /*should have been called well before any unparsing started*/
}
#endif
    
/* Print some debugging info */
void
SgAsmLERelocTable::dump(FILE *f, const char *prefix, ssize_t idx) const
{
    char p[4096];
    if (idx>=0) {
        snprintf(p, sizeof(p), "%s%sRelocTable[%zd].", prefix, get_header()->formatName(), idx);
    } else {
        snprintf(p, sizeof(p), "%s%sRelocTable.", prefix, get_header()->formatName());
    }

    const int w = std::max(size_t{1}, Rose::DUMP_FIELD_WIDTH - strlen(p));

    SgAsmGenericSection::dump(f, p, -1);
    fprintf(f, "%s%-*s = %" PRIuPTR " entries\n", p, w, "size", p_entries.size());
    for (size_t i = 0; i < p_entries.size(); i++) {
        p_entries[i]->dump(f, p, i);
    }
}
    
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* Parses the structure of an LE/LX file and adds the information to the SgAsmGenericFile. */
SgAsmLEFileHeader *
SgAsmLEFileHeader::parse(SgAsmDOSFileHeader *dos_header)
{
    ROSE_ASSERT(dos_header);
    SgAsmGenericFile *ef = dos_header->get_file();
    ROSE_ASSERT(ef);

    /* LE files extend the DOS header with some additional info */
    SgAsmDOSExtendedHeader *dos2_header = new SgAsmDOSExtendedHeader(dos_header);
    dos2_header->set_offset(dos_header->get_size());
    dos2_header->parse();
    
    /* The LE header */
    SgAsmLEFileHeader *le_header = new SgAsmLEFileHeader(ef, dos2_header->get_e_lfanew());
    le_header->set_dos2Header(dos2_header);

    /* Page Table */
    if (le_header->get_e_pagetab_rfo() > 0 && le_header->get_e_npages() > 0) {
        rose_addr_t table_offset = le_header->get_offset() + le_header->get_e_pagetab_rfo();
        rose_addr_t table_size = le_header->get_e_npages() * sizeof(SgAsmLEPageTableEntry::LEPageTableEntry_disk);
        SgAsmLEPageTable *table = new SgAsmLEPageTable(le_header, table_offset, table_size);
        le_header->set_pageTable(table);
    }

    /* Section (Object) Table */
    if (le_header->get_e_secttab_rfo() > 0 && le_header->get_e_secttab_nentries() > 0) {
        rose_addr_t table_offset = le_header->get_offset() + le_header->get_e_secttab_rfo();
        rose_addr_t table_size = le_header->get_e_secttab_nentries() * sizeof(SgAsmLESectionTableEntry::LESectionTableEntry_disk);
        SgAsmLESectionTable *table = new SgAsmLESectionTable(le_header, table_offset, table_size);
        le_header->set_sectionTable(table);
    }
    
    /* Resource Table */
    if (le_header->get_e_rsrctab_rfo() > 0 && le_header->get_e_rsrctab_nentries() > 0) {
        /*FIXME*/
    }

    /* Resident Names Table */
    if (le_header->get_e_resnametab_rfo() > 0) {
        rose_addr_t table_offset = le_header->get_offset() + le_header->get_e_resnametab_rfo();
        SgAsmLENameTable *table = new SgAsmLENameTable(le_header, table_offset);
        char section_name[64];
        snprintf(section_name, sizeof(section_name), "%s Resident Name Table", le_header->formatName());
        table->set_name(new SgAsmBasicString(section_name));
        le_header->set_residentNameTable(table);
    }

    /* Non-resident Names Table */
    if (le_header->get_e_nonresnametab_offset() > 0) {
        rose_addr_t table_offset = le_header->get_e_nonresnametab_offset();
        SgAsmLENameTable *table = new SgAsmLENameTable(le_header, table_offset);
        char section_name[64];
        snprintf(section_name, sizeof(section_name), "%s Non-resident Name Table", le_header->formatName());
        table->set_name(new SgAsmBasicString(section_name));
        le_header->set_nonresidentNameTable(table);
    }
    
    /* Entry Table */
    if (le_header->get_e_entrytab_rfo() > 0) {
        rose_addr_t table_offset = le_header->get_offset() + le_header->get_e_entrytab_rfo();
        SgAsmLEEntryTable *table = new SgAsmLEEntryTable(le_header, table_offset);
        le_header->set_entryTable(table);
    }

    /* Fixup (Relocation) Table */
    if (le_header->get_e_fixup_rectab_rfo() > 0) {
        rose_addr_t table_offset = le_header->get_offset() + le_header->get_e_fixup_rectab_rfo();
        SgAsmLERelocTable *table = new SgAsmLERelocTable(le_header, table_offset);
        le_header->set_relocationTable(table);
    }
    
    return le_header;
}

// }; //namespace LE
// }; //namespace Exec

SgAsmDOSExtendedHeader*
SgAsmLEFileHeader::get_dos2_header() const {
    return get_dos2Header();
}

void
SgAsmLEFileHeader::set_dos2_header(SgAsmDOSExtendedHeader *x) {
    set_dos2Header(x);
}

SgAsmLESectionTable*
SgAsmLEFileHeader::get_section_table() const {
    return get_sectionTable();
}

void
SgAsmLEFileHeader::set_section_table(SgAsmLESectionTable *x) {
    set_sectionTable(x);
}

SgAsmLEPageTable*
SgAsmLEFileHeader::get_page_table() const {
    return get_pageTable();
}

void
SgAsmLEFileHeader::set_page_table(SgAsmLEPageTable *x) {
    set_pageTable(x);
}

SgAsmLENameTable*
SgAsmLEFileHeader::get_resname_table() const {
    return get_residentNameTable();
}

void
SgAsmLEFileHeader::set_resname_table(SgAsmLENameTable *x) {
    set_residentNameTable(x);
}

SgAsmLENameTable*
SgAsmLEFileHeader::get_nonresname_table() const {
    return get_nonresidentNameTable();
}

void
SgAsmLEFileHeader::set_nonresname_table(SgAsmLENameTable *x) {
    set_nonresidentNameTable(x);
}

SgAsmLEEntryTable*
SgAsmLEFileHeader::get_entry_table() const {
    return get_entryTable();
}

void
SgAsmLEFileHeader::set_entry_table(SgAsmLEEntryTable *x) {
    set_entryTable(x);
}

SgAsmLERelocTable*
SgAsmLEFileHeader::get_reloc_table() const {
    return get_relocationTable();
}

void
SgAsmLEFileHeader::set_reloc_table(SgAsmLERelocTable *x) {
    set_relocationTable(x);
}

unsigned
SgAsmLESectionTableEntry::get_pagemap_index() const {
    return get_pageMapIndex();
}

void
SgAsmLESectionTableEntry::set_pagemap_index(unsigned x) {
    set_pageMapIndex(x);
}

unsigned
SgAsmLESectionTableEntry::get_pagemap_nentries() const {
    return get_pageMapNEntries();
}

void
SgAsmLESectionTableEntry::set_pagemap_nentries(unsigned x) {
    set_pageMapNEntries(x);
}

rose_addr_t
SgAsmLESectionTableEntry::get_mapped_size() const {
    return get_mappedSize();
}

void
SgAsmLESectionTableEntry::set_mapped_size(rose_addr_t x) {
    set_mappedSize(x);
}

rose_addr_t
SgAsmLESectionTableEntry::get_base_addr() const {
    return get_baseAddr();
}

void
SgAsmLESectionTableEntry::set_base_addr(rose_addr_t x) {
    set_baseAddr(x);
}

SgAsmLESectionTableEntry*
SgAsmLESection::get_st_entry() const {
    return get_sectionTableEntry();
}

void
SgAsmLESection::set_st_entry(SgAsmLESectionTableEntry *x) {
    set_sectionTableEntry(x);
}

#endif
