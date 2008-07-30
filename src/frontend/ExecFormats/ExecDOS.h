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
    unsigned char e_magic[2];           /* 0x00 "MZ" */
    uint16_t    e_last_page_size;       /* 0x02 bytes used on last page of file (1 page == 512 bytes); zero if last page is full */
    uint16_t    e_total_pages;          /* 0x04 number of pages (including last possibly partial page) in file */
    uint16_t    e_nrelocs;              /* 0x06 number of relocation entries stored after this header */
    uint16_t    e_header_paragraphs;    /* 0x08 header size in paragraphs (16-byte blocks) including relocations */
    uint16_t    e_minalloc;             /* 0x0a number of extra paragraphs needed, similar to BSS in Unix */
    uint16_t    e_maxalloc;             /* 0x0c max paragraphs to allocate for BSS */
    uint16_t    e_ss;                   /* 0x0e initial value of SS register relative to program load segment */
    uint16_t    e_sp;                   /* 0x10 initial value for SP register */
    uint16_t    e_cksum;                /* 0x12 checksum; 16-bit sum of all words in file should be zero (usually not filled in) */
    uint16_t    e_ip;                   /* 0x14 initial value for IP register */
    uint16_t    e_cs;                   /* 0x16 initial value for CS register relative to program load segment */
    uint16_t    e_relocs_offset;        /* 0x18 file address of relocation table */
    uint16_t    e_overlay;              /* 0x1a overlay number (zero indicates main program) */
    unsigned char e_res1[4];            /* 0x1c unknown purpose */
} __attribute__((packed));              /* 0x20 */ 

class DOSFileHeader : public ExecHeader {
  public:
    DOSFileHeader(ExecFile *f, addr_t offset)
        : ExecHeader(f, offset, sizeof(DOSFileHeader_disk)),
        relocs(NULL), rm_section(NULL)
        {ctor(f, offset);}
    virtual ~DOSFileHeader() {}
    virtual void unparse(FILE*);
    virtual void dump(FILE*, const char *prefix, ssize_t idx);
    virtual const char *format_name() {return "DOS";}
    ExecSection *add_rm_section(addr_t max_offset=0);

    /* Accessors for protected/private data members */
    ExecSection *get_relocs() {return relocs;}
    void set_relocs(ExecSection *s) {relocs=s;}
    ExecSection *get_rm_section() {return rm_section;}
    void set_rm_section(ExecSection *s) {rm_section=s;}

    /* These are the native-format versions of the same members described in the DOSFileHeader_disk format struct */
    unsigned            e_last_page_size, e_total_pages, e_nrelocs, e_header_paragraphs, e_minalloc, e_maxalloc;
    unsigned            e_ss, e_sp, e_cksum, e_ip, e_cs, e_overlay;
    addr_t              e_relocs_offset;
    unsigned char       e_res1[4];
    
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
DOSFileHeader *parse(ExecFile*, bool define_rm_section=true);

}; //namespace DOS
}; //namespace Exec

#endif /*!Exec_ExecDOS_h*/


