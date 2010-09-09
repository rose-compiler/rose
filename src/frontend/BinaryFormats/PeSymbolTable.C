/* Windows PE COFF Symbol Tables (SgAsmCoffSymbolTable and related objects) */

// tps (01/14/2010) : Switching from rose.h to sage3.
#include "sage3basic.h"
#define __STDC_FORMAT_MACROS
#include <inttypes.h>

/* Constructor reads symbol table entries beginning at entry 'i'. We can't pass an array of COFFSymbolEntry_disk structs
 * because the disk size is 18 bytes, which is not properly aligned according to the C standard. Therefore we pass the actual
 * section and table index. The symbol occupies the specified table slot and st_num_aux_entries additional slots. */
void
SgAsmCoffSymbol::ctor(SgAsmPEFileHeader *fhdr, SgAsmGenericSection *symtab, SgAsmGenericSection *strtab, size_t idx)
{
    static const bool debug = false;
    COFFSymbol_disk disk;
    symtab->read_content_local(idx * COFFSymbol_disk_size, &disk, COFFSymbol_disk_size);
    if (disk.st_zero == 0) {
        p_st_name_offset = le_to_host(disk.st_offset);
        if (p_st_name_offset < 4) throw FormatError("name collides with size field");
        std::string s = strtab->read_content_local_str(p_st_name_offset);
        set_name(new SgAsmBasicString(s));
    } else {
        char temp[9];
        memcpy(temp, disk.st_name, 8);
        temp[8] = '\0';
        set_name(new SgAsmBasicString(temp));
        p_st_name_offset = 0;
    }

    p_st_name            = get_name()->get_string();
    p_st_section_num     = le_to_host(disk.st_section_num);
    p_st_type            = le_to_host(disk.st_type);
    p_st_storage_class   = le_to_host(disk.st_storage_class);
    p_st_num_aux_entries = le_to_host(disk.st_num_aux_entries);

    /* Bind to section number. We can do this now because we've already parsed the PE Section Table */
    ROSE_ASSERT(fhdr->get_section_table()!=NULL);
    if (p_st_section_num > 0) {
        p_bound = fhdr->get_file()->get_section_by_id(p_st_section_num);
        ROSE_ASSERT(p_bound != NULL);
    }
    
    /* Make initial guesses for storage class, type, and definition state. We'll adjust them after reading aux entries. */
    p_value = le_to_host(disk.st_value);
    p_def_state = SYM_DEFINED;
    switch (p_st_storage_class) {
      case 0:    p_binding = SYM_NO_BINDING; break; /*none*/
      case 1:    p_binding = SYM_LOCAL;      break; /*stack*/
      case 2:    p_binding = SYM_GLOBAL;     break; /*extern*/
      case 3:    p_binding = SYM_GLOBAL;     break; /*static*/
      case 4:    p_binding = SYM_LOCAL;      break; /*register*/
      case 5:    p_binding = SYM_GLOBAL;     break; /*extern def*/
      case 6:    p_binding = SYM_LOCAL;      break; /*label*/
      case 7:    p_binding = SYM_LOCAL;      break; /*label(undef)*/
      case 8:    p_binding = SYM_LOCAL;      break; /*struct member*/
      case 9:    p_binding = SYM_LOCAL;      break; /*formal arg*/
      case 10:   p_binding = SYM_LOCAL;      break; /*struct tag*/
      case 11:   p_binding = SYM_LOCAL;      break; /*union member*/
      case 12:   p_binding = SYM_GLOBAL;     break; /*union tag*/
      case 13:   p_binding = SYM_GLOBAL;     break; /*typedef*/
      case 14:   p_binding = SYM_GLOBAL;     break; /*static(undef)*/
      case 15:   p_binding = SYM_GLOBAL;     break; /*enum tag*/
      case 16:   p_binding = SYM_LOCAL;      break; /*enum member*/
      case 17:   p_binding = SYM_GLOBAL;     break; /*register param*/
      case 18:   p_binding = SYM_LOCAL;      break; /*bit field*/
      case 100:  p_binding = SYM_GLOBAL;     break; /*block(bb or eb)*/
      case 101:  p_binding = SYM_GLOBAL;     break; /*function*/
      case 102:  p_binding = SYM_LOCAL;      break; /*struct end*/
      case 103:  p_binding = SYM_GLOBAL;     break; /*file*/
      case 104:  p_binding = SYM_GLOBAL;     break; /*section*/
      case 105:  p_binding = SYM_WEAK;       break; /*weak extern*/
      case 107:  p_binding = SYM_LOCAL;      break; /*CLR token*/
      case 0xff: p_binding = SYM_GLOBAL;     break; /*end of function*/
    }
    switch (p_st_type & 0xf0) {
      case 0x00: p_type = SYM_NO_TYPE; break;     /*none*/
      case 0x10: p_type = SYM_DATA;    break;     /*ptr*/
      case 0x20: p_type = SYM_FUNC;    break;     /*function*/
      case 0x30: p_type = SYM_ARRAY;   break;     /*array*/
    }
    
    /* Read additional aux entries. We keep this as 'char' to avoid alignment problems. */
    if (p_st_num_aux_entries > 0) {
        p_aux_data = symtab->read_content_local_ucl((idx+1)*COFFSymbol_disk_size, p_st_num_aux_entries * COFFSymbol_disk_size);

        if (get_type() == SYM_FUNC && p_st_section_num > 0) {
            /* Function */
            unsigned bf_idx      = le_to_host(*(uint32_t*)&(p_aux_data[0]));
            unsigned size        = le_to_host(*(uint32_t*)&(p_aux_data[4]));
            unsigned lnum_ptr    = le_to_host(*(uint32_t*)&(p_aux_data[8]));
            unsigned next_fn_idx = le_to_host(*(uint32_t*)&(p_aux_data[12]));
            unsigned res1        = le_to_host(*(uint16_t*)&(p_aux_data[16]));
            set_size(size);
            if (debug) {
                fprintf(stderr, "COFF aux func %s: bf_idx=%u, size=%u, lnum_ptr=%u, next_fn_idx=%u, res1=%u\n", 
                        p_st_name.c_str(), bf_idx, size, lnum_ptr, next_fn_idx, res1);
            }
            
        } else if (p_st_storage_class == 101 /*function*/ && (0 == p_st_name.compare(".bf") || 0 == p_st_name.compare(".ef"))) {
            /* Beginning/End of function */
            unsigned res1        = le_to_host(*(uint32_t*)&(p_aux_data[0]));
            unsigned lnum        = le_to_host(*(uint16_t*)&(p_aux_data[4])); /*line num within source file*/
            unsigned res2        = le_to_host(*(uint16_t*)&(p_aux_data[6]));
            unsigned res3        = le_to_host(*(uint32_t*)&(p_aux_data[8]));
            unsigned next_bf     = le_to_host(*(uint32_t*)&(p_aux_data[12])); /*only for .bf; reserved in .ef*/
            unsigned res4        = le_to_host(*(uint16_t*)&(p_aux_data[16]));
            if (debug) {
                fprintf(stderr, "COFF aux %s: res1=%u, lnum=%u, res2=%u, res3=%u, next_bf=%u, res4=%u\n", 
                        p_st_name.c_str(), res1, lnum, res2, res3, next_bf, res4);
            }
            
        } else if (p_st_storage_class == 2/*external*/ && p_st_section_num == 0/*undef*/ && get_value()==0) {
            /* Weak External */
            unsigned sym2_idx    = le_to_host(*(uint32_t*)&(p_aux_data[0]));
            unsigned flags       = le_to_host(*(uint32_t*)&(p_aux_data[4]));
            unsigned res1        = le_to_host(*(uint32_t*)&(p_aux_data[8]));
            unsigned res2        = le_to_host(*(uint32_t*)&(p_aux_data[12]));
            unsigned res3        = le_to_host(*(uint16_t*)&(p_aux_data[16]));
            if (debug) {
                fprintf(stderr, "COFF aux weak %s: sym2_idx=%u, flags=%u, res1=%u, res2=%u, res3=%u\n",
                        p_st_name.c_str(), sym2_idx, flags, res1, res2, res3);
            }
            
        } else if (p_st_storage_class == 103/*file*/ && 0 == p_st_name.compare(".file")) {
            /* This symbol is a file. The file name is stored in the aux data as either the name itself or an offset
             * into the string table. Replace the fake ".file" with the real file name. */
            const COFFSymbol_disk *d = (const COFFSymbol_disk*) &(p_aux_data[0]);
            if (0 == d->st_zero) {
                addr_t fname_offset = le_to_host(d->st_offset);
                if (fname_offset < 4) throw FormatError("name collides with size field");
                set_name(new SgAsmBasicString(strtab->read_content_local_str(fname_offset)));
                if (debug)
                    fprintf(stderr, "COFF aux file: offset=%"PRIu64", name=\"%s\"\n", fname_offset, get_name()->c_str());
            } else {
                /* Aux data contains a NUL-padded name; the NULs (if any) are not part of the name. */
                ROSE_ASSERT(p_st_num_aux_entries == 1);
                char fname[COFFSymbol_disk_size+1];
                memcpy(fname, &(p_aux_data[0]), COFFSymbol_disk_size);
                fname[COFFSymbol_disk_size] = '\0';
                set_name(new SgAsmBasicString(fname));
                if (debug)
                    fprintf(stderr, "COFF aux file: inline-name=\"%s\"\n", get_name()->c_str());
            }
            set_type(SYM_FILE);

        } else if (p_st_storage_class == 3/*static*/ && NULL != fhdr->get_file()->get_section_by_name(p_st_name, '$')) {
            /* Section */
            unsigned size         = le_to_host(*(uint32_t*)&(p_aux_data[0])); /*same as section header SizeOfRawData */
            unsigned nrel         = le_to_host(*(uint16_t*)&(p_aux_data[4])); /*number of relocations*/
            unsigned nln_ents     = le_to_host(*(uint16_t*)&(p_aux_data[6])); /*number of line number entries */
            unsigned cksum        = le_to_host(*(uint32_t*)&(p_aux_data[8]));
            unsigned sect_id      = le_to_host(*(uint16_t*)&(p_aux_data[12])); /*1-base index into section table*/
            unsigned comdat       = p_aux_data[14]; /*comdat selection number if section is a COMDAT section*/
            unsigned res1         = p_aux_data[15];
            unsigned res2         = le_to_host(*(uint16_t*)&(p_aux_data[16]));
            set_size(size);
            set_type(SYM_SECTION);
            if (debug) {
                fprintf(stderr, 
                        "COFF aux section: size=%u, nrel=%u, nln_ents=%u, cksum=%u, sect_id=%u, comdat=%u, res1=%u, res2=%u\n", 
                        size, nrel, nln_ents, cksum, sect_id, comdat, res1, res2);
            }
            
        } else if (p_st_storage_class==3/*static*/ && (p_st_type & 0xf)==0/*null*/ &&
                   get_value()==0 && NULL!=fhdr->get_file()->get_section_by_name(p_st_name)) {
            /* COMDAT section */
            /*FIXME: not implemented yet*/
            fprintf(stderr, "COFF aux comdat %s: (FIXME) not implemented yet\n", p_st_name.c_str());
            hexdump(stderr, (addr_t) symtab->get_offset()+(idx+1)*COFFSymbol_disk_size, "    ", p_aux_data);

        } else {
            fprintf(stderr, "COFF aux unknown %s: (FIXME) st_storage_class=%u, st_type=0x%02x, st_section_num=%d\n", 
                    p_st_name.c_str(), p_st_storage_class, p_st_type, p_st_section_num);
            hexdump(stderr, symtab->get_offset()+(idx+1)*COFFSymbol_disk_size, "    ", p_aux_data);
        }
    }

}

/* Encode a symbol back into disk format */
void *
SgAsmCoffSymbol::encode(COFFSymbol_disk *disk) const
{
    if (0 == p_st_name_offset) {
        /* Name is stored in entry */
        memset(disk->st_name, 0, sizeof(disk->st_name));
        ROSE_ASSERT(p_st_name.size() <= sizeof(disk->st_name));
        memcpy(disk->st_name, p_st_name.c_str(), p_st_name.size());
    } else {
        /* Name is an offset into the string table */
        disk->st_zero = 0;
        host_to_le(p_st_name_offset, &(disk->st_offset));
    }
    
 // host_to_le(get_value(),          &(disk->st_value));
    host_to_le(p_value,              &(disk->st_value));
    host_to_le(p_st_section_num,     &(disk->st_section_num));
    host_to_le(p_st_type,            &(disk->st_type));
    host_to_le(p_st_storage_class,   &(disk->st_storage_class));
    host_to_le(p_st_num_aux_entries, &(disk->st_num_aux_entries));
    return disk;
}

/* Print some debugging info */
void
SgAsmCoffSymbol::dump(FILE *f, const char *prefix, ssize_t idx) const
{
    char p[4096], ss[128], tt[128];
    const char *s=NULL, *t=NULL;
    if (idx>=0) {
        sprintf(p, "%sCOFFSymbol[%zd].", prefix, idx);
    } else {
        sprintf(p, "%sCOFFSymbol.", prefix);
    }

    const int w = std::max(1, DUMP_FIELD_WIDTH-(int)strlen(p));


    SgAsmGenericSymbol::dump(f, p, -1);

    switch (p_st_section_num) {
      case 0:  s = "external, not assigned";    break;
      case -1: s = "absolute value";            break;
      case -2: s = "general debug, no section"; break;
      default: sprintf(ss, "%d", p_st_section_num); s = ss; break;
    }
    fprintf(f, "%s%-*s = %s\n", p, w, "st_section_num", s);

    switch (p_st_type & 0xf0) {
      case 0x00: s = "none";     break;
      case 0x10: s = "pointer";  break;
      case 0x20: s = "function"; break;
      case 0x30: s = "array";    break;
      default:
        sprintf(ss, "%u", p_st_type >> 8);
        s = ss;
        break;
    }
    switch (p_st_type & 0xf) {
      case 0x00: t = "none";            break;
      case 0x01: t = "void";            break;
      case 0x02: t = "char";            break;
      case 0x03: t = "short";           break;
      case 0x04: t = "int";             break;
      case 0x05: t = "long";            break;
      case 0x06: t = "float";           break;
      case 0x07: t = "double";          break;
      case 0x08: t = "struct";          break;
      case 0x09: t = "union";           break;
      case 0x0a: t = "enum";            break;
      case 0x0b: t = "enum member";     break;
      case 0x0c: t = "byte";            break;
      case 0x0d: t = "2-byte word";     break;
      case 0x0e: t = "unsigned int";    break;
      case 0x0f: t = "4-byte unsigned"; break;
      default:
        sprintf(tt, "%u", p_st_type & 0xf);
        t = tt;
        break;
    }
    fprintf(f, "%s%-*s = %s / %s\n",          p, w, "st_type", s, t);

    switch (p_st_storage_class) {
      case 0:    s = "none";            t = "";                                  break;
      case 1:    s = "auto variable";   t = "stack frame offset";                break;
      case 2:    s = "external";        t = "size or section offset";            break;
      case 3:    s = "static";          t = "offset in section or section name"; break;
      case 4:    s = "register";        t = "register number";                   break;
      case 5:    s = "extern_def";      t = "";                                  break;
      case 6:    s = "label";           t = "offset in section";                 break;
      case 7:    s = "label(undef)";    t = "";                                  break;
      case 8:    s = "struct member";   t = "member number";                     break;
      case 9:    s = "formal arg";      t = "argument number";                   break;
      case 10:   s = "struct tag";      t = "tag name";                          break;
      case 11:   s = "union member";    t = "member number";                     break;
      case 12:   s = "union tag";       t = "tag name";                          break;
      case 13:   s = "typedef";         t = "";                                  break;
      case 14:   s = "static(undef)";   t = "";                                  break;
      case 15:   s = "enum tag";        t = "";                                  break;
      case 16:   s = "enum member";     t = "member number";                     break;
      case 17:   s = "register param";  t = "";                                  break;
      case 18:   s = "bit field";       t = "bit number";                        break;
      case 19:   s = "auto arg";        t = "";                                  break;
      case 20:   s = "dummy entry (EOB)"; t="";                                  break;
      case 100:  s = "block(bb,eb)";    t = "relocatable address";               break;
      case 101:  s = "function";        t = "nlines or size";                    break;
      case 102:  s = "struct end";      t = "";                                  break;
      case 103:  s = "file";            t = "";                                  break;
      case 104:  s = "section/line#";   t = "";                                  break;
      case 105:  s = "weak extern";     t = "";                                  break;
      case 106:  s = "ext in dmert pub lib";t="";                                break;
      case 107:  s = "CLR token";       t = "";                                  break;
      case 0xff: s = "end of function"; t = "";                                  break;
      default:
        sprintf(ss, "%u", p_st_storage_class);
        s = ss;
        t = "";  
        break;
    }
    fprintf(f, "%s%-*s = %s\n",               p, w, "st_storage_class", s);
    fprintf(f, "%s%-*s = \"%s\"\n",           p, w, "st_name", p_st_name.c_str());
    fprintf(f, "%s%-*s = %u\n",               p, w, "st_num_aux_entries", p_st_num_aux_entries);
    fprintf(f, "%s%-*s = %zu bytes\n",        p, w, "aux_data", p_aux_data.size());
    hexdump(f, 0, std::string(p)+"aux_data at ", p_aux_data);
}

/* Constructor */
void
SgAsmCoffSymbolTable::ctor()
{
    set_synthesized(true);
    set_name(new SgAsmBasicString("COFF Symbols"));
    set_purpose(SP_SYMTAB);

    p_symbols = new SgAsmCoffSymbolList;
    p_symbols->set_parent(this);
}

SgAsmCoffSymbolTable*
SgAsmCoffSymbolTable::parse()
{
    /* Set the section size according to the number of entries indicated in the header. */
    SgAsmPEFileHeader *fhdr = dynamic_cast<SgAsmPEFileHeader*>(get_header());
    ROSE_ASSERT(fhdr!=NULL);
    set_offset(fhdr->get_e_coff_symtab());
    set_size(fhdr->get_e_coff_nsyms()*SgAsmCoffSymbol::COFFSymbol_disk_size);

    SgAsmGenericSection::parse();

    /* The string table immediately follows the symbols. The first four bytes of the string table are the size of the
     * string table in little endian. */
    addr_t strtab_offset = get_offset() + fhdr->get_e_coff_nsyms() * SgAsmCoffSymbol::COFFSymbol_disk_size;
    p_strtab = new SgAsmGenericSection(fhdr->get_file(), fhdr);
    p_strtab->set_offset(strtab_offset);
    p_strtab->set_size(sizeof(uint32_t));
    p_strtab->set_synthesized(true);
    p_strtab->set_name(new SgAsmBasicString("COFF Symbol Strtab"));
    p_strtab->set_purpose(SP_HEADER);
    p_strtab->parse();

    uint32_t word;
    p_strtab->read_content(0, &word, sizeof word);
    addr_t strtab_size = le_to_host(word);
    if (strtab_size < sizeof(uint32_t))
        throw FormatError("COFF symbol table string table size is less than four bytes");
    p_strtab->extend(strtab_size - sizeof(uint32_t));

    for (size_t i = 0; i < fhdr->get_e_coff_nsyms(); i++) {
        SgAsmCoffSymbol *symbol = new SgAsmCoffSymbol(fhdr, this, p_strtab, i);
        i += symbol->get_st_num_aux_entries();
        p_symbols->get_symbols().push_back(symbol);
    }
    return this;
}

/** Returns the number of COFF Symbol Table slots occupied by the symbol table. The number of slots can be larger than the
 *  number of symbols since some symbols might have auxiliary entries. */
size_t
SgAsmCoffSymbolTable::get_nslots() const
{
    size_t nsyms = p_symbols->get_symbols().size();
    size_t nslots = nsyms;
    for (size_t i=0; i<nsyms; i++) {
        SgAsmCoffSymbol *symbol = p_symbols->get_symbols()[i];
        nslots += symbol->get_st_num_aux_entries();
    }
    return nslots;
}

/* Write symbol table back to disk */
void
SgAsmCoffSymbolTable::unparse(std::ostream &f) const
{
    addr_t spos = 0; /*section offset*/
    
    for (size_t i=0; i < p_symbols->get_symbols().size(); i++) {
        SgAsmCoffSymbol *symbol = p_symbols->get_symbols()[i];
        SgAsmCoffSymbol::COFFSymbol_disk disk;
        symbol->encode(&disk);
        spos = write(f, spos, SgAsmCoffSymbol::COFFSymbol_disk_size, &disk);
        spos = write(f, (addr_t) spos, symbol->get_aux_data());
    }
    if (get_strtab())
        get_strtab()->unparse(f);
}

/* Print some debugging info */
void
SgAsmCoffSymbolTable::dump(FILE *f, const char *prefix, ssize_t idx) const
{
    char p[4096];
    if (idx>=0) {
        sprintf(p, "%sCOFFSymtab[%zd].", prefix, idx);
    } else {
        sprintf(p, "%sCOFFSymtab.", prefix);
    }


    const int w = std::max(1, DUMP_FIELD_WIDTH-(int)strlen(p));

    SgAsmGenericSection::dump(f, p, -1);
    fprintf(f, "%s%-*s = %zu symbols\n", p, w, "size", p_symbols->get_symbols().size());
    for (size_t i = 0; i < p_symbols->get_symbols().size(); i++) {
        p_symbols->get_symbols()[i]->dump(f, p, i);
    }

    if (variantT() == V_SgAsmCoffSymbolTable) //unless a base class
        hexdump(f, 0, std::string(p)+"data at ", p_data);
}
