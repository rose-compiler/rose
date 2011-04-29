/* PE Import Lookup Table Entry (also used for Import Address Table Entries) */
#include "sage3basic.h"
#define __STDC_FORMAT_MACROS
#include <inttypes.h>

/** Construct an Import Lookup Table Entry (or Import Address Table Entry) and links it into the parent Import Lookup Table (or
 *  Import Address Table). If the Import Lookup Table, @p ilt, is the null pointer then the entry is not added to the table and
 *  the user must call SgAsmPEImportLookupTable::add_entry() to add it later. */
void
SgAsmPEImportILTEntry::ctor(SgAsmPEImportLookupTable *ilt)
{
    if (ilt)
        ilt->add_entry(this);
}

/** Initialize an Import Lookup Table Entry (or Import Address Table Entry) by parsing a quadword. */
SgAsmPEImportILTEntry *
SgAsmPEImportILTEntry::parse(uint64_t ilt_word)
{
    SgAsmPEFileHeader *fhdr = SageInterface::getEnclosingNode<SgAsmPEFileHeader>(this);
    ROSE_ASSERT(fhdr!=NULL);
    p_hnt_entry = NULL;

    /* Initialize */
    p_ordinal = 0;
    p_hnt_entry_rva = 0;
    p_hnt_entry = NULL;
    p_extra_bits = 0;
    p_bound_rva = 0;

    /* Masks for different word sizes */
    uint64_t ordmask;                                           /* if bit is set then ILT Entry is an Ordinal */
    uint64_t hnrvamask = 0x7fffffff;                            /* Hint/Name RVA mask (both word sizes use 31 bits) */
    if (4==fhdr->get_word_size()) {
        ordmask = (uint64_t)1<<31;
    } else if (8==fhdr->get_word_size()) {
        ordmask = (uint64_t)1<<63;
    } else {
        throw FormatError("unsupported PE word size");
    }
    
    if (ilt_word & ordmask) {
        /* Ordinal */
        p_entry_type = ILT_ORDINAL;
        p_ordinal = ilt_word & 0xffff;
        p_extra_bits = ilt_word & ~(ordmask|0xffff);
    } else if (0!=(ilt_word & ~hnrvamask) ||
               NULL==fhdr->get_best_section_by_va((ilt_word&hnrvamask) + fhdr->get_base_va(), false)) {
        /* Bound address */
        p_entry_type = ILT_BOUND_RVA;
        p_bound_rva = ilt_word;
        p_bound_rva.bind(fhdr);
    } else {
        /* Hint/Name Pair RVA */
        p_entry_type = ILT_HNT_ENTRY_RVA;
        p_hnt_entry_rva = ilt_word & hnrvamask;
        p_hnt_entry_rva.bind(fhdr);
    }
    return this;
}

/** Causes the ILT Entry to point to a name/hint entry. */
void
SgAsmPEImportILTEntry::point_to_hnt(SgAsmPEImportHNTEntry *hnt_entry)
{
    set_isModified(true);
    set_entry_type(ILT_HNT_ENTRY_RVA);
    set_hnt_entry(hnt_entry);
    hnt_entry->set_parent(this);
}

/* Encode the PE Import Lookup Table or PE Import Address Table object into a word. */
uint64_t
SgAsmPEImportILTEntry::encode(const SgAsmPEFileHeader *fhdr) const
{
    uint64_t w = 0;

    /* Masks for different word sizes */
    uint64_t ordmask;                                           /* if bit is set then ILT Entry is an Ordinal */
    if (4==fhdr->get_word_size()) {
        ordmask = (uint64_t)1<<31;
    } else if (8==fhdr->get_word_size()) {
        ordmask = (uint64_t)1<<63;
    } else {
        throw FormatError("unsupported PE word size");
    }
    
    switch (p_entry_type) {
      case ILT_ORDINAL:
        w |= ordmask | p_extra_bits | p_ordinal;
        break;
      case ILT_BOUND_RVA:
        w |= p_bound_rva.get_rva();
        break;
      case ILT_HNT_ENTRY_RVA:
        w |= p_hnt_entry_rva.get_rva();
        break;
    }
    return w;
}

void
SgAsmPEImportILTEntry::unparse(std::ostream &f, const SgAsmPEFileHeader *fhdr, rose_rva_t rva, size_t idx) const
{
    ROSE_ASSERT(rva.get_section()!=NULL);
    uint64_t ilt_entry_word = encode(fhdr);
    if (4==fhdr->get_word_size()) {
        uint32_t ilt_entry_disk;
        host_to_le(ilt_entry_word, &ilt_entry_disk);
        rva.get_section()->write(f, rva.get_rel()+idx*4, 4, &ilt_entry_disk);
    } else if (8==fhdr->get_word_size()) {
        uint64_t ilt_entry_disk;
        host_to_le(ilt_entry_word, &ilt_entry_disk);
        rva.get_section()->write(f, rva.get_rel()+idx*8, 8, &ilt_entry_disk);
    } else {
        throw FormatError("unsupported PE word size");
    }

    if (p_hnt_entry)
        p_hnt_entry->unparse(f, p_hnt_entry_rva);
}

/* Print debugging info for an Import Lookup Table Entry or an Import Address Table Entry */
void
SgAsmPEImportILTEntry::dump(FILE *f, const char *prefix, ssize_t idx) const
{
    char p[4096];
    if (idx>=0) {
        sprintf(p, "%sentry[%zd].", prefix, idx);
    } else {
        sprintf(p, "%sentry.", prefix);
    }

    const int w = std::max(1, DUMP_FIELD_WIDTH-(int)strlen(p));

    switch (p_entry_type) {
      case ILT_ORDINAL:
        fprintf(f, "%s%-*s = 0x%04x (%u)\n", p, w, "ordinal", p_ordinal, p_ordinal);
        break;
      case ILT_HNT_ENTRY_RVA:
        fprintf(f, "%s%-*s = %s\n", p, w, "hnt_entry_rva", p_hnt_entry_rva.to_string().c_str());
        break;
      case ILT_BOUND_RVA:
        fprintf(f, "%s%-*s = %s\n", p, w, "bound_rva", p_bound_rva.to_string().c_str());
        break;
      default:
        ROSE_ASSERT(!"PE Import Lookup Table entry type is not valid");
    }
    if (p_extra_bits)
        fprintf(f, "%s%-*s = 0x%08"PRIx64"\n", p, w, "extra_bits", p_extra_bits);
    if (p_hnt_entry)
        p_hnt_entry->dump(f, p, -1);
}

