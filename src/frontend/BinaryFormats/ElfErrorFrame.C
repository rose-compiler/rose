/* ELF Error Handling Frames (SgAsmElfEHFrameSection and related classes) */

// tps (01/14/2010) : Switching from rose.h to sage3.
#include "sage3basic.h"
#define __STDC_FORMAT_MACROS
#include <inttypes.h>

static const size_t WARNING_LIMIT=10;
static size_t nwarnings=0;

/** Non-parsing constructor */
void
SgAsmElfEHFrameEntryCI::ctor(SgAsmElfEHFrameSection *ehframe)
{
    ROSE_ASSERT(ehframe->get_ci_entries()!=NULL);
    ehframe->get_ci_entries()->get_entries().push_back(this);
    ROSE_ASSERT(ehframe->get_ci_entries()->get_entries().size()>0);
    set_parent(ehframe->get_ci_entries());

    p_fd_entries = new SgAsmElfEHFrameEntryFDList;
    p_fd_entries->set_parent(this);
}

/** Unparse one Common Information Entry (CIE) without unparsing the Frame Description Entries (FDE) to which it points. The
 *  initial length fields are not included in the result string. */
std::string
SgAsmElfEHFrameEntryCI::unparse(const SgAsmElfEHFrameSection *ehframe) const 
{
    SgAsmElfFileHeader *fhdr = ehframe->get_elf_header();
    ROSE_ASSERT(fhdr!=NULL);

    /* Allocate worst-case size for results */
    size_t worst_size = (4+
                         1+
                         get_augmentation_string().size()+1+
                         10+
                         10+
                         10+
                         get_augmentation_data_length()+
                         get_instructions().size()+
                         fhdr->get_word_size());
    unsigned char *buf = new unsigned char[worst_size];

    rose_addr_t at = 0;
    uint32_t u32_disk;
    unsigned char u8_disk;

    /* CIE back offset (always zero) */
    u32_disk=0;
    memcpy(buf+at, &u32_disk, 4); at+=4;

    /* Version */
    u8_disk = get_version();
    memcpy(buf+at, &u8_disk, 1); at+=1;

    /* NUL-terminated Augmentation String */
    size_t sz = get_augmentation_string().size()+1;
    memcpy(buf+at, get_augmentation_string().c_str(), sz); at+=sz;

    /* Alignment factors */
    at = ehframe->write_uleb128(buf, at, get_code_alignment_factor());
    at = ehframe->write_sleb128(buf, at, get_data_alignment_factor());

    /* Augmentation data */
    at = ehframe->write_uleb128(buf, at, get_augmentation_data_length());
    std::string astr = get_augmentation_string();
    if (astr[0]=='z') {
        for (size_t i=1; i<astr.size(); i++) {
            if ('L'==astr[i]) {
                u8_disk = get_lsda_encoding();
                buf[at++] = u8_disk;
            } else if ('P'==astr[i]) {
                u8_disk = get_prh_encoding();
                buf[at++] = u8_disk;
                switch (get_prh_encoding()) {
                    case 0x05:
                    case 0x06:
                    case 0x07:
                        buf[at++] = get_prh_arg();
                        host_to_le(get_prh_addr(), &u32_disk);
                        memcpy(buf+at, &u32_disk, 4); at+=4;
                        break;
                    default:
                        /* See parser */
                        if (++nwarnings<=WARNING_LIMIT) {
                            fprintf(stderr, "%s:%u: warning: unknown PRH encoding (0x%02x)\n",
                                    __FILE__, __LINE__, get_prh_encoding());
                            if (WARNING_LIMIT==nwarnings)
                                fprintf(stderr, "    (additional frame warnings will be suppressed)\n");
                        }
                        break;
                }
            } else if ('R'==astr[i]) {
                u8_disk = get_addr_encoding();
                buf[at++] = u8_disk;
            } else if ('S'==astr[i]) {
                /* Signal frame; no auxilliary data */
            } else {
                ROSE_ASSERT(!"invalid .eh_frame augmentation string");
                abort();
            }
        }
    }

    /* Initial instructions */
    sz = get_instructions().size();
    if (sz>0) {
        memcpy(buf+at, &(get_instructions()[0]), sz);
        at += sz;
    }

    std::string retval((char*)buf, at);
    delete[] buf;
    return retval;
}

/** Print some debugging info */
void
SgAsmElfEHFrameEntryCI::dump(FILE *f, const char *prefix, ssize_t idx) const
{
    char p[4096];
    if (idx>=0) {
        sprintf(p, "%sCIE[%zd].", prefix, idx);
    } else {
        sprintf(p, "%sCIE.", prefix);
    }
    const int w = std::max(1, DUMP_FIELD_WIDTH-(int)strlen(p));

    fprintf(f, "%s%-*s = %d\n", p, w, "version", get_version());
    fprintf(f, "%s%-*s = \"%s\"\n", p, w, "augStr", get_augmentation_string().c_str());
    fprintf(f, "%s%-*s = %s\n", p, w, "sig_frame", get_sig_frame()?"yes":"no");
    fprintf(f, "%s%-*s = 0x%08"PRIx64" (%"PRIu64")\n", p, w, "code_align",
            get_code_alignment_factor(), get_code_alignment_factor());
    fprintf(f, "%s%-*s = 0x%08"PRIx64" (%"PRId64")\n", p, w, "data_align",
            get_data_alignment_factor(), get_data_alignment_factor());
    fprintf(f, "%s%-*s = 0x%08"PRIx64" (%"PRIu64")\n", p, w, "aug_length",
            get_augmentation_data_length(), get_augmentation_data_length());
    fprintf(f, "%s%-*s = %d\n", p, w, "lsda_encoding", get_lsda_encoding());
    fprintf(f, "%s%-*s = %d\n", p, w, "prh_encoding", get_prh_encoding());
    if (get_prh_encoding()>=0) {
        fprintf(f, "%s%-*s = 0x%02x (%u)\n", p, w, "prh_arg", get_prh_arg(), get_prh_arg());
        fprintf(f, "%s%-*s = 0x%08"PRIx64" (%"PRIu64")\n", p, w, "prh_addr", get_prh_addr(), get_prh_addr());
    }
    fprintf(f, "%s%-*s = %d\n", p, w, "addr_encoding", get_addr_encoding());
    if (get_instructions().size()>0) {
        fprintf(f, "%s%-*s = 0x%08zx (%zu) bytes\n", p, w, "instructions",
                get_instructions().size(), get_instructions().size());
        hexdump(f, 0, std::string(p)+"insns at ", get_instructions());
    }
    for (size_t i=0; i<get_fd_entries()->get_entries().size(); i++) {
        SgAsmElfEHFrameEntryFD *fde = get_fd_entries()->get_entries()[i];
        fde->dump(f, p, i);
    }
}

/** Non-parsing constructor */
void
SgAsmElfEHFrameEntryFD::ctor(SgAsmElfEHFrameEntryCI *cie)
{
    ROSE_ASSERT(cie->get_fd_entries()!=NULL);
    cie->get_fd_entries()->get_entries().push_back(this);
    ROSE_ASSERT(cie->get_fd_entries()->get_entries().size()>0);
    set_parent(cie->get_fd_entries());
}

/** Unparse the Frame Description Entry (FDE) into a string but do not include the leading length field(s) or the CIE back
 *  pointer. */
std::string
SgAsmElfEHFrameEntryFD::unparse(const SgAsmElfEHFrameSection *ehframe, SgAsmElfEHFrameEntryCI *cie) const
{
    SgAsmElfFileHeader *fhdr = ehframe->get_elf_header();
    ROSE_ASSERT(fhdr!=NULL);

    /* Allocate worst-case size for results */
    size_t worst_size = 8 + get_augmentation_data().size() + get_instructions().size() + fhdr->get_word_size();
    unsigned char *buf = new unsigned char[worst_size];

    size_t sz;
    rose_addr_t at = 0;
    uint32_t u32_disk;

    /* PC Begin (begin_rva) and size */
    switch (cie->get_addr_encoding()) {
        case -1:          /* No address encoding specified */
        case 0x01:
        case 0x03:
        case 0x1b: {
            host_to_le(get_begin_rva().get_rva(), &u32_disk);
            memcpy(buf+at, &u32_disk, 4); at+=4;
            host_to_le(get_size(), &u32_disk);
            memcpy(buf+at, &u32_disk, 4); at+=4;
            break;
        }
        default:
            /* See parser */
            if (++nwarnings<=WARNING_LIMIT) {
                fprintf(stderr, "%s:%u: warning: unknown FDE address encoding (0x%02x)\n",
                        __FILE__, __LINE__, cie->get_addr_encoding());
                if (WARNING_LIMIT==nwarnings)
                    fprintf(stderr, "    (additional frame warnings will be suppressed)\n");
            }
            break;
    }

    /* Augmentation Data */
    std::string astr = cie->get_augmentation_string();
    if (astr.size()>0 && astr[0]=='z') {
        at = ehframe->write_uleb128(buf, at, get_augmentation_data().size());
        sz = get_augmentation_data().size();
        if (sz>0) {
            memcpy(buf+at, &(get_augmentation_data()[0]), sz);
            at += sz;
        }
    }

    /* Call frame instructions */
    sz = get_instructions().size();
    if (sz>0) {
        memcpy(buf+at, &(get_instructions()[0]), sz);
        at += sz;
    }

    std::string retval((char*)buf, at);
    delete[] buf;
    return retval;
}


/** Print some debugging info */
void
SgAsmElfEHFrameEntryFD::dump(FILE *f, const char *prefix, ssize_t idx) const
{
    char p[4096];
    if (idx>=0) {
        sprintf(p, "%sFDE[%zd].", prefix, idx);
    } else {
        sprintf(p, "%sFDE.", prefix);
    }
    const int w = std::max(1, DUMP_FIELD_WIDTH-(int)strlen(p));

    fprintf(f, "%s%-*s = %s\n", p, w, "begin_rva", get_begin_rva().to_string().c_str());
    fprintf(f, "%s%-*s = 0x%08"PRIx64" (%"PRIu64") bytes\n", p, w, "size", get_size(), get_size());
    fprintf(f, "%s%-*s = 0x%08zx (%zu) bytes\n", p, w, "aug_data",
            get_augmentation_data().size(), get_augmentation_data().size());
    hexdump(f, 0, std::string(p)+"data at ", get_augmentation_data());
    fprintf(f, "%s%-*s = 0x%08zx (%zu) bytes\n", p, w, "instructions",
            get_instructions().size(), get_instructions().size());
    hexdump(f, 0, std::string(p)+"insns at ", get_instructions());
}

/** Non-parsing constructor */
void
SgAsmElfEHFrameSection::ctor()
{
    p_ci_entries = new SgAsmElfEHFrameEntryCIList;
    p_ci_entries->set_parent(this);
}

/** Initialize by parsing a file. */
SgAsmElfEHFrameSection *
SgAsmElfEHFrameSection::parse()
{
    SgAsmElfSection::parse();
    SgAsmElfFileHeader *fhdr = get_elf_header();
    ROSE_ASSERT(fhdr!=NULL);

    rose_addr_t record_offset=0;
    std::map<rose_addr_t, SgAsmElfEHFrameEntryCI*> cies;

    while (record_offset<get_size()) {
        rose_addr_t at = record_offset;
        unsigned char u8_disk;
        uint32_t u32_disk;
        uint64_t u64_disk;

        /* Length or extended length */
        rose_addr_t length_field_size = 4; /*number of bytes not counted in length*/
        read_content_local(at, &u32_disk, 4); at += 4;
        rose_addr_t record_size = disk_to_host(fhdr->get_sex(), u32_disk);
        if (record_size==0xffffffff) {
            read_content_local(at, &u64_disk, 8); at += 8;
            record_size = disk_to_host(fhdr->get_sex(), u64_disk);
            length_field_size += 8; /*FIXME: it's not entirely clear whether ExtendedLength includes this field*/
        }
        if (0==record_size)
            break;

        /* Backward offset to CIE record, or zero if this is a CIE record. */
        read_content_local(at, &u32_disk, 4); at += 4;
        rose_addr_t cie_back_offset = disk_to_host(fhdr->get_sex(), u32_disk);
        if (0==cie_back_offset) {
            /* This is a CIE record */
            SgAsmElfEHFrameEntryCI *cie = new SgAsmElfEHFrameEntryCI(this);
            cies[record_offset] = cie;

            /* Version */
            uint8_t cie_version;
            read_content_local(at++, &cie_version, 1);
            cie->set_version(cie_version);

            /* Augmentation String */
            std::string astr = read_content_local_str(at);
            at += astr.size() + 1;
            cie->set_augmentation_string(astr);

            /* Alignment factors */
            cie->set_code_alignment_factor(read_content_local_uleb128(&at));
            cie->set_data_alignment_factor(read_content_local_sleb128(&at));

            /* Augmentation data length. This is apparently the length of the data described by the Augmentation String plus
             * the Initial Instructions plus any padding. [RPM 2009-01-15] */
            cie->set_augmentation_data_length(read_content_local_uleb128(&at));

            /* Augmentation data. The format of the augmentation data in the CIE record is determined by reading the
             * characters of the augmentation string. */ 
            if (astr[0]=='z') {
                for (size_t i=1; i<astr.size(); i++) {
                    if ('L'==astr[i]) {
                        read_content_local(at++, &u8_disk, 1);
                        cie->set_lsda_encoding(u8_disk);
                    } else if ('P'==astr[i]) {
                        /* The first byte is an encoding method which describes the following bytes, which are the address of
                         * a Personality Routine Handler. There appears to be very little documentation about these fields. */
                        read_content_local(at++, &u8_disk, 1);
                        cie->set_prh_encoding(u8_disk);
                        switch (cie->get_prh_encoding()) {
                            case 0x05:          /* See Ubuntu 32bit /usr/bin/aptitude */
                            case 0x06:          /* See second CIE record for Gentoo-Amd64 /usr/bin/addftinfo */
                            case 0x07:          /* See first CIE record for Gentoo-Amd64 /usr/bin/addftinfo */
                                read_content_local(at++, &u8_disk, 1); /* not sure what this is; arg for __gxx_personality_v0? */
                                cie->set_prh_arg(u8_disk);
                                read_content_local(at, &u32_disk, 4); at+=4; /* address of <__gxx_personality_v0@plt> */
                                cie->set_prh_addr(le_to_host(u32_disk));
                                break;
                            case 0x09:          /* *.o file generated by gcc-4.0.x */
                                /* FIXME: Cannot find any info about this entry. Fix SgAsmElfEHFrameSection::parse() if we
                                 *        ever figure this out. [RPM 2009-09-29] */
                                /*fallthrough*/
                            default: {
                                if (++nwarnings<=WARNING_LIMIT) {
                                    fprintf(stderr, "%s:%u: warning: ELF CIE 0x%08"PRIx64" has unknown PRH encoding 0x%02x\n", 
                                            __FILE__, __LINE__, get_offset()+record_offset, cie->get_prh_encoding());
                                    if (WARNING_LIMIT==nwarnings)
                                        fprintf(stderr, "    (additional frame warnings will be suppressed)\n");
                                }
                                break;
                            }
                        }
                    } else if ('R'==astr[i]) {
                        read_content_local(at++, &u8_disk, 1);
                        cie->set_addr_encoding(u8_disk);
                    } else if ('S'==astr[i]) {
                        /* See http://lkml.indiana.edu/hypermail/linux/kernel/0602.3/1144.html and GCC PR #26208*/
                        cie->set_sig_frame(true);
                    } else {
                        /* Some stuff we don't handle yet. Warn about it and don't read anything. */
                        if (++nwarnings<=WARNING_LIMIT) {
                            fprintf(stderr, "%s:%u: warning: ELF CIE 0x%08"PRIx64" has invalid augmentation string \"%s\"\n", 
                                    __FILE__, __LINE__, get_offset()+record_offset, astr.c_str());
                            if (WARNING_LIMIT==nwarnings)
                                fprintf(stderr, "    (additional frame warnings will be suppressed)\n");
                        }
                    }
                }
            }

            /* Initial instructions. These are apparently included in the augmentation_data_length. The final instructions can
             * be zero padding (no-op instructions) to bring the record up to a multiple of the word size. */
            rose_addr_t init_insn_size = (length_field_size + record_size) - (at - record_offset);
            cie->get_instructions() = read_content_local_ucl(at, init_insn_size);
            ROSE_ASSERT(cie->get_instructions().size()==init_insn_size);

        } else {
            /* This is a FDE record */
            rose_addr_t cie_offset = record_offset + length_field_size - cie_back_offset;
            assert(cies.find(cie_offset)!=cies.end());
            SgAsmElfEHFrameEntryCI *cie = cies[cie_offset];
            SgAsmElfEHFrameEntryFD *fde = new SgAsmElfEHFrameEntryFD(cie);

            /* PC Begin (begin_rva) and size */
            switch (cie->get_addr_encoding()) {
              case -1:          /* No address encoding specified */
              case 0x01:
              case 0x03:
              case 0x1b:        /* Address doesn't look valid (e.g., 0xfffd74e8) but still four bytes [RPM 2008-01-16]*/
              {
                  read_content_local(at, &u32_disk, 4); at+=4;
                  fde->set_begin_rva(le_to_host(u32_disk));
                  read_content_local(at, &u32_disk, 4); at+=4;
                  fde->set_size(le_to_host(u32_disk));
                  break;
              }
              default:
                fprintf(stderr, "%s:%u: ELF CIE 0x%08"PRIx64", FDE 0x%08"PRIx64": unknown address encoding: 0x%02x\n", 
                        __FILE__, __LINE__, get_offset()+cie_offset, get_offset()+record_offset, cie->get_addr_encoding());
                abort();
            }

            /* Augmentation Data */
            std::string astring = cie->get_augmentation_string();
            if (astring.size()>0 && astring[0]=='z') {
                rose_addr_t aug_length = read_content_local_uleb128(&at);
                fde->get_augmentation_data() = read_content_local_ucl(at, aug_length);
                at += aug_length;
                ROSE_ASSERT(fde->get_augmentation_data().size()==aug_length);
            }

            /* Call frame instructions */
            rose_addr_t cf_insn_size = (length_field_size + record_size) - (at - record_offset);
            fde->get_instructions() = read_content_local_ucl(at, cf_insn_size);
            ROSE_ASSERT(fde->get_instructions().size()==cf_insn_size);
        }

        record_offset += length_field_size + record_size;
    }
    return this;
}

/** Return sizes for various parts of the table. See doc for SgAsmElfSection::calculate_sizes. Since EH Frame Sections are
 *  run-length encoded, we need to actually unparse the section in order to determine its size. */
rose_addr_t
SgAsmElfEHFrameSection::calculate_sizes(size_t *entsize, size_t *required, size_t *optional, size_t *entcount) const
{
    rose_addr_t whole = unparse(NULL);
    if (entsize)
        *entsize = 0;
    if (required)
        *required = 0;
    if (optional)
        *optional = 0;
    if (entcount)
        *entcount = 0;
    return whole;
}

/** Write data to .eh_frame section */
void
SgAsmElfEHFrameSection::unparse(std::ostream &f) const
{
    unparse(&f);
}

/** Unparses the section into the optional output stream and returns the number of bytes written. If there is no output stream
 *  we still go through the actions but don't write anything. This is the only way to determine the amount of memory required
 *  to store the section since the section is run-length encoded. */
rose_addr_t
SgAsmElfEHFrameSection::unparse(std::ostream *fp) const
{
    SgAsmElfFileHeader *fhdr = get_elf_header();
    ROSE_ASSERT(fhdr!=NULL);

    rose_addr_t at=0;
    uint32_t u32_disk;
    uint64_t u64_disk;

    for (size_t i=0; i<get_ci_entries()->get_entries().size(); i++) {
        rose_addr_t last_cie_offset = at;
        SgAsmElfEHFrameEntryCI *cie = get_ci_entries()->get_entries()[i];
        std::string s = cie->unparse(this);
        if (s.size()<0xffffffff) {
            host_to_disk(fhdr->get_sex(), s.size(), &u32_disk);
            if (fp)
                write(*fp, at, 4, &u32_disk);
            at += 4;
        } else {
            u32_disk = 0xffffffff;
            if (fp)
                write(*fp, at, 4, &u32_disk);
            at += 4;
            host_to_disk(fhdr->get_sex(), s.size(), &u64_disk);
            if (fp)
                write(*fp, at, 8, &u64_disk);
            at += 8;
        }
        if (fp)
            write(*fp, at, s);
        at += s.size();

        for (size_t j=0; j<cie->get_fd_entries()->get_entries().size(); j++) {
            SgAsmElfEHFrameEntryFD *fde = cie->get_fd_entries()->get_entries()[j];
            std::string s = fde->unparse(this, cie);

            /* Record size, not counting run-length coded size field, but counting CIE back offset. */
            rose_addr_t record_size = 4 + s.size();
            if (record_size<0xffffffff) {
                host_to_disk(fhdr->get_sex(), record_size, &u32_disk);
                if (fp)
                    write(*fp, at, 4, &u32_disk);
                at += 4;
            } else {
                u32_disk = 0xffffffff;
                if (fp)
                    write(*fp, at, 4, &u32_disk);
                at += 4;
                host_to_disk(fhdr->get_sex(), record_size, &u64_disk);
                if (fp)
                    write(*fp, at, 8, &u64_disk);
                at += 8;
            }

            /* CIE back offset. Number of bytes from the beginning of the current CIE record (including the Size fields) to
             * the beginning of the FDE record (excluding the Size fields but including the CIE back offset). */
            rose_addr_t cie_back_offset = at - last_cie_offset;
            host_to_disk(fhdr->get_sex(), cie_back_offset, &u32_disk);
            if (fp)
                write(*fp, at, 4, &u32_disk);
            at += 4;

            /* The FDE record itself */
            if (fp)
                write(*fp, at, s);
            at += s.size();
        }
    }

    /* Write a zero length to indicate the end of the CIE list */
    u32_disk = 0;
    if (fp)
        write(*fp, at, 4, &u32_disk);
    at += 4;

    return at;
}

/* Print some debugging info */
void
SgAsmElfEHFrameSection::dump(FILE *f, const char *prefix, ssize_t idx) const
{
    char p[4096];
    if (idx>=0) {
        sprintf(p, "%sElfEHFrameSection[%zd].", prefix, idx);
    } else {
        sprintf(p, "%sElfEHFrameSection.", prefix);
    }

    SgAsmElfSection::dump(f, p, -1);
    for (size_t i=0; i<get_ci_entries()->get_entries().size(); i++) {
        SgAsmElfEHFrameEntryCI *cie = get_ci_entries()->get_entries()[i];
        cie->dump(f, p, i);
    }
}
