/* Copyright 2008 Lawrence Livermore National Security, LLC */
#ifndef Exec_ExecDOS_h
#define Exec_ExecDOS_h

#include "ExecGeneric.h"

namespace Exec {
namespace DOS {

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// MS-DOS Real Mode File Header
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* File format of an MS-DOS Real Mode File Header. All fields are little endian. */
struct DOSFileHeader_disk {
    unsigned char e_magic[2];           /* 0x4d, 0x5a */
    uint16_t    e_last_page_size;       /* bytes used on last page of file (1 page == 512 bytes); zero if last page is full */
    uint16_t    e_total_pages;          /* number of pages (including last possibly partial page) in file */
    uint16_t    e_nrelocs;              /* number of relocation entries stored after this header */
    uint16_t    e_header_paragraphs;    /* header size in paragraphs (16-byte blocks) including relocations */
    uint16_t    e_minalloc;             /* number of extra paragraphs needed, similar to BSS in Unix */
    uint16_t    e_maxalloc;
    uint16_t    e_ss;                   /* initial value of SS register relative to program load segment */
    uint16_t    e_sp;                   /* initial value for SP register */
    uint16_t    e_cksum;                 /* checksum; 16-bit sum of all words in file should be zero (usually not filled in) */
    uint16_t    e_ip;                   /* initial value for IP register */
    uint16_t    e_cs;                   /* initial value for CS register relative to program load segment */
    uint16_t    e_relocs_offset;        /* file address of relocation table */
    uint16_t    e_overlay;              /* overlay number (zero indicates main program) */
} __attribute__((packed));

class DOSFileHeader : public ExecHeader {
  public:
    DOSFileHeader(ExecFile *f, addr_t offset)
        : ExecHeader(f, offset, sizeof(DOSFileHeader_disk)),
        relocs(NULL), rm_section(NULL)
        {ctor(f, offset);}
    virtual ~DOSFileHeader() {}
    virtual void unparse(FILE*);
    virtual void dump(FILE*, const char *prefix, ssize_t idx);

    /* Accessors for protected/private data members */
    ExecSection *get_relocs() {return relocs;}
    void set_relocs(ExecSection *s) {relocs=s;}
    ExecSection *get_rm_section() {return rm_section;}
    void set_rm_section(ExecSection *s) {rm_section=s;}

    /* These are the native-format versions of the same members described in the DOSFileHeader_disk format struct */
    unsigned            e_last_page_size, e_total_pages, e_nrelocs, e_header_paragraphs, e_minalloc, e_maxalloc;
    unsigned            e_ss, e_sp, e_cksum, e_ip, e_cs, e_overlay;
    addr_t              e_relocs_offset;
    
  private:
    void ctor(ExecFile *f, addr_t offset);
    void *encode(DOSFileHeader_disk*);
    ExecSection         *relocs;        /* Relocation info */
    ExecSection         *rm_section;    /* Real mode code segment */
};

struct DOSRelocEntry_disk {
    uint16_t    offset;
    uint16_t    segment;
} __attribute__((packed));


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* Functions */
bool is_DOS(ExecFile*);
DOSFileHeader *parse(ExecFile*);

}; //namespace DOS
}; //namespace Exec

#endif /*!Exec_ExecDOS_h*/


