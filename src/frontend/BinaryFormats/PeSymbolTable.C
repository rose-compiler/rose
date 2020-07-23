/* Windows PE COFF Symbol Tables (SgAsmCoffSymbolTable and related objects) */
#include <rosePublicConfig.h>
#ifdef ROSE_BUILD_BINARY_ANALYSIS_SUPPORT
#include "sage3basic.h"

#include "Diagnostics.h"

// In order to efficiently (in terms of amount of code) parse a file format that's defined for a different architecture, we
// need to occassionally take addresses of structs that don't follow alignment rules for this architecture.
#pragma GCC diagnostic ignored "-Waddress-of-packed-member"

using namespace Rose;
using namespace Rose::Diagnostics;

/* Constructor reads symbol table entries beginning at entry 'i'. We can't pass an array of COFFSymbolEntry_disk structs
 * because the disk size is 18 bytes, which is not properly aligned according to the C standard. Therefore we pass the actual
 * section and table index. The symbol occupies the specified table slot and st_num_aux_entries additional slots.
 *
 * See http://www.skyfree.org/linux/references/coff.pdf */
void
SgAsmCoffSymbol::ctor(SgAsmPEFileHeader *fhdr, SgAsmGenericSection *symtab, SgAsmGenericSection *strtab, size_t idx)
{
    ASSERT_not_null(fhdr);
    ASSERT_not_null(symtab);
    ASSERT_not_null(strtab);

    Sawyer::Message::Stream debug(mlog[DEBUG]);

    static uint8_t zeroRecord[COFFSymbol_disk_size];
    memset(zeroRecord, 0, sizeof zeroRecord);
    COFFSymbol_disk disk;
    symtab->read_content_local(idx * COFFSymbol_disk_size, &disk, COFFSymbol_disk_size);
    if (0 == memcmp(&disk, zeroRecord, COFFSymbol_disk_size))
        throw FormatError("zero symbol record");

    if (disk.st_zero == 0) {
        p_st_name_offset = ByteOrder::le_to_host(disk.st_offset);
        if (p_st_name_offset < 4)
            throw FormatError("name collides with size field");
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
    p_st_section_num     = ByteOrder::le_to_host(disk.st_section_num);
    p_st_type            = ByteOrder::le_to_host(disk.st_type);
    p_st_storage_class   = ByteOrder::le_to_host(disk.st_storage_class);
    p_st_num_aux_entries = ByteOrder::le_to_host(disk.st_num_aux_entries);

    /* Bind to section number. We can do this now because we've already parsed the PE Section Table */
    ASSERT_not_null(fhdr->get_section_table());
    if (p_st_section_num > 0) {
        p_bound = fhdr->get_file()->get_section_by_id(p_st_section_num);
        if (NULL==p_bound) {
            mlog[WARN] <<"PE symbol \"" <<StringUtility::cEscape(p_st_name) <<"\" (index " <<idx <<")"
                       <<" is not bound to any section (section " <<p_st_section_num <<")\n";
        }
    }
    
    /* Make initial guesses for storage class, type, and definition state. We'll adjust them after reading aux entries. */
    p_value = ByteOrder::le_to_host(disk.st_value);
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

    if (debug) {
        debug <<"COFF symbol entry #" <<idx <<" before AUX parsing"
              <<": p_st_name_offset=" <<p_st_name_offset
              <<", p_st_name=\"" <<StringUtility::cEscape(p_st_name) <<"\""
              <<", p_st_section_num=" <<p_st_section_num
              <<", p_st_type=" <<p_st_type
              <<", p_st_storage_class=" <<p_st_storage_class
              <<", p_st_num_aux_entries=" <<p_st_num_aux_entries
              <<", p_value=" <<p_value
              <<", p_def_state=" <<p_def_state
              <<", p_binding=" <<p_binding
              <<", p_type=" <<p_type <<"\n";
    }
    
    /* Read additional aux entries. We keep this as 'char' to avoid alignment problems. */
    if (p_st_num_aux_entries > 0) {
        p_aux_data = symtab->read_content_local_ucl((idx+1)*COFFSymbol_disk_size, p_st_num_aux_entries * COFFSymbol_disk_size);

        if (2 /*external*/ == p_st_storage_class && get_type() == SYM_FUNC && p_st_section_num > 0) {
            // Auxiliary record format 1: Function definitions
            unsigned bf_idx      = ByteOrder::le_to_host(*(uint32_t*)&(p_aux_data[0]));
            unsigned size        = ByteOrder::le_to_host(*(uint32_t*)&(p_aux_data[4]));
            unsigned lnum_ptr    = ByteOrder::le_to_host(*(uint32_t*)&(p_aux_data[8]));
            unsigned next_fn_idx = ByteOrder::le_to_host(*(uint32_t*)&(p_aux_data[12]));
            unsigned res1        = ByteOrder::le_to_host(*(uint16_t*)&(p_aux_data[16]));
            set_size(size);
            SAWYER_MESG(debug) <<"COFF aux func " <<escapeString(p_st_name) <<": bf_idx=" <<bf_idx
                               <<", size=" <<size <<", lnum_ptr=" <<StringUtility::addrToString(lnum_ptr)
                               <<", next_fn_idx=" <<next_fn_idx <<", res1=" <<res1 <<"\n";
            
        } else if (p_st_storage_class == 101 /*function*/ && (p_st_name == ".bf" || p_st_name == ".ef")) {
            // Auxiliary record format 2: .bf and .ef symbols
            unsigned res1        = ByteOrder::le_to_host(*(uint32_t*)&(p_aux_data[0]));
            unsigned lnum        = ByteOrder::le_to_host(*(uint16_t*)&(p_aux_data[4])); /*line num within source file*/
            unsigned res2        = ByteOrder::le_to_host(*(uint16_t*)&(p_aux_data[6]));
            unsigned res3        = ByteOrder::le_to_host(*(uint32_t*)&(p_aux_data[8]));
            unsigned next_bf     = ByteOrder::le_to_host(*(uint32_t*)&(p_aux_data[12])); /*only for .bf; reserved in .ef*/
            unsigned res4        = ByteOrder::le_to_host(*(uint16_t*)&(p_aux_data[16]));
            SAWYER_MESG(debug) <<"COFF aux " <<escapeString(p_st_name) <<": res1=" <<res1 <<", lnum=" <<lnum
                               <<", res2=" <<res2 <<", res3=" <<res3 <<", next_bf=" <<next_bf <<", res4=" <<res4 <<"\n";
            
        } else if (p_st_storage_class == 2/*external*/ && p_st_section_num == 0/*undef*/ && get_value()==0) {
            // Auxiliary record format 3: weak externals
            unsigned sym2_idx    = ByteOrder::le_to_host(*(uint32_t*)&(p_aux_data[0]));
            unsigned flags       = ByteOrder::le_to_host(*(uint32_t*)&(p_aux_data[4]));
            unsigned res1        = ByteOrder::le_to_host(*(uint32_t*)&(p_aux_data[8]));
            unsigned res2        = ByteOrder::le_to_host(*(uint32_t*)&(p_aux_data[12]));
            unsigned res3        = ByteOrder::le_to_host(*(uint16_t*)&(p_aux_data[16]));
            SAWYER_MESG(debug) <<"COFF aux weak " <<escapeString(p_st_name) <<": sym2_idx=" <<sym2_idx
                               <<", flags=" <<flags <<", res1=" <<res1 <<", res2=" <<res2 <<", res3=" <<res3 <<"\n";
            
        } else if (p_st_storage_class == 103/*file*/ && 0 == p_st_name.compare(".file")) {
            // Auxiliary record format 4: files. The file name is stored in the aux data as either the name itself or an offset
            // into the string table. Replace the fake ".file" with the real file name.
            const COFFSymbol_disk *d = (const COFFSymbol_disk*) &(p_aux_data[0]);
            if (0 == d->st_zero) {
                rose_addr_t fname_offset = ByteOrder::le_to_host(d->st_offset);
                if (fname_offset < 4)
                    throw FormatError("name collides with size field");
                set_name(new SgAsmBasicString(strtab->read_content_local_str(fname_offset)));
                SAWYER_MESG(debug) <<"COFF aux file: offset=" <<fname_offset
                                   <<", name=\"" <<get_name()->get_string(true) <<"\"\n";

            } else {
                /* Aux data contains a NUL-padded name; the NULs (if any) are not part of the name. */
                ASSERT_require(p_st_num_aux_entries == 1);
                char fname[COFFSymbol_disk_size+1];
                memcpy(fname, &(p_aux_data[0]), COFFSymbol_disk_size);
                fname[COFFSymbol_disk_size] = '\0';
                set_name(new SgAsmBasicString(fname));
                SAWYER_MESG(debug) <<"COFF aux file: inline-name=\"" <<get_name()->get_string(true) <<"\"\n";
            }
            set_type(SYM_FILE);

        } else if (p_st_storage_class == 3/*static*/ && NULL != fhdr->get_file()->get_section_by_name(p_st_name, '$')) {
            // Auxiliary record format 5: Section definition.
            unsigned size         = ByteOrder::le_to_host(*(uint32_t*)&(p_aux_data[0])); /*same as section header SizeOfRawData */
            unsigned nrel         = ByteOrder::le_to_host(*(uint16_t*)&(p_aux_data[4])); /*number of relocations*/
            unsigned nln_ents     = ByteOrder::le_to_host(*(uint16_t*)&(p_aux_data[6])); /*number of line number entries */
            unsigned cksum        = ByteOrder::le_to_host(*(uint32_t*)&(p_aux_data[8]));
            unsigned sect_id      = ByteOrder::le_to_host(*(uint16_t*)&(p_aux_data[12])); /*1-base index into section table*/
            unsigned comdat       = p_aux_data[14]; /*comdat selection number if section is a COMDAT section*/
            unsigned res1         = p_aux_data[15];
            unsigned res2         = ByteOrder::le_to_host(*(uint16_t*)&(p_aux_data[16]));
            set_size(size);
            set_type(SYM_SECTION);
            SAWYER_MESG(debug) <<"COFF aux section: size=" <<size <<", nrel=" <<nrel <<", nln_ents=" <<nln_ents
                               <<", cksum=" <<cksum <<", sect_id=" <<sect_id <<", comdat=" <<comdat
                               <<", res1=" <<res1 <<", res2=" <<res2 <<"\n";
            
        } else if (p_st_storage_class==3/*static*/ && (p_st_type & 0xf)==0/*null*/ &&
                   get_value()==0 && NULL!=fhdr->get_file()->get_section_by_name(p_st_name)) {
            // Auxiliary record for common data (COMDAT) sections
            // FIXME[Robb P Matzke 2017-05-17]: The record format isn't documented in the reference listed above.
            if (debug) {
                debug <<"COFF aux comdat " <<escapeString(p_st_name) <<": aux record ignored\n";
                hexdump(debug, (rose_addr_t) symtab->get_offset()+(idx+1)*COFFSymbol_disk_size, "    ", p_aux_data);
            }

        } else {
            if (mlog[WARN]) {
                mlog[WARN] <<"COFF aux unknown " <<escapeString(p_st_name)
                           <<": st_storage_class=" <<p_st_storage_class
                           <<", st_type=" <<p_st_type <<", st_section_num=" <<p_st_section_num <<"\n";
                hexdump(mlog[WARN], symtab->get_offset()+(idx+1)*COFFSymbol_disk_size, "    ", p_aux_data);
            }
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
        ByteOrder::host_to_le(p_st_name_offset, &(disk->st_offset));
    }
    
 // ByteOrder::host_to_le(get_value(),          &(disk->st_value));
    ByteOrder::host_to_le(p_value,              &(disk->st_value));
    ByteOrder::host_to_le(p_st_section_num,     &(disk->st_section_num));
    ByteOrder::host_to_le(p_st_type,            &(disk->st_type));
    ByteOrder::host_to_le(p_st_storage_class,   &(disk->st_storage_class));
    ByteOrder::host_to_le(p_st_num_aux_entries, &(disk->st_num_aux_entries));
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
    fprintf(f, "%s%-*s = \"%s\"\n",           p, w, "st_name", escapeString(p_st_name).c_str());
    fprintf(f, "%s%-*s = %u\n",               p, w, "st_num_aux_entries", p_st_num_aux_entries);
    fprintf(f, "%s%-*s = %" PRIuPTR " bytes\n",        p, w, "aux_data", p_aux_data.size());
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
    rose_addr_t strtab_offset = get_offset() + fhdr->get_e_coff_nsyms() * SgAsmCoffSymbol::COFFSymbol_disk_size;
    p_strtab = new SgAsmGenericSection(fhdr->get_file(), fhdr);
    p_strtab->set_offset(strtab_offset);
    p_strtab->set_size(sizeof(uint32_t));
    p_strtab->set_synthesized(true);
    p_strtab->set_name(new SgAsmBasicString("COFF Symbol Strtab"));
    p_strtab->set_purpose(SP_HEADER);
    p_strtab->parse();

    uint32_t word;
    p_strtab->read_content(0, &word, sizeof word);
    rose_addr_t strtab_size = ByteOrder::le_to_host(word);
    if (strtab_size < sizeof(uint32_t))
        throw FormatError("COFF symbol table string table size is less than four bytes");
    p_strtab->extend(strtab_size - sizeof(uint32_t));

    /* Parse symbols until we've parsed the required number or we run off the end of the section. */
    for (size_t i = 0; i < fhdr->get_e_coff_nsyms(); i++) {
        try {
            SgAsmCoffSymbol *symbol = new SgAsmCoffSymbol(fhdr, this, p_strtab, i);
            i += symbol->get_st_num_aux_entries();
            p_symbols->get_symbols().push_back(symbol);
        } catch (const FormatError &e) {
            mlog[WARN] <<"SgAsmCoffSymbolTable::parse: invalid symbol: " <<e.what() <<"\n"
                       <<"    in section \"" <<get_name()->get_string(true) <<"\"[" <<get_id() <<"]\n"
                       <<"    symbol #" <<i <<" at file offset "
                       <<StringUtility::addrToString(get_offset() + i*SgAsmCoffSymbol::COFFSymbol_disk_size) <<"\n"
                       <<"    discarding this symbol\n";
        } catch (const ShortRead &e) {
            mlog[WARN] <<"SgAsmCoffSymbolTable::parse: read past end of section \"" <<get_name()->get_string(true) <<"\""
                       <<"[" <<get_id() <<"]\n"
                       <<"    symbol #" <<i <<" at file offset " <<StringUtility::addrToString(e.offset) <<"\n"
                       <<"    skipping " <<StringUtility::plural(fhdr->get_e_coff_nsyms()-i, "symbols")
                       <<" (including this one)\n";
            break;
        }
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
    rose_addr_t spos = 0; /*section offset*/
    
    for (size_t i=0; i < p_symbols->get_symbols().size(); i++) {
        SgAsmCoffSymbol *symbol = p_symbols->get_symbols()[i];
        SgAsmCoffSymbol::COFFSymbol_disk disk;
        symbol->encode(&disk);
        spos = write(f, spos, SgAsmCoffSymbol::COFFSymbol_disk_size, &disk);
        spos = write(f, (rose_addr_t) spos, symbol->get_aux_data());
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
    fprintf(f, "%s%-*s = %" PRIuPTR " symbols\n", p, w, "size", p_symbols->get_symbols().size());
    for (size_t i = 0; i < p_symbols->get_symbols().size(); i++) {
        p_symbols->get_symbols()[i]->dump(f, p, i);
    }

    if (variantT() == V_SgAsmCoffSymbolTable) //unless a base class
        hexdump(f, 0, std::string(p)+"data at ", p_data);
}

#endif
