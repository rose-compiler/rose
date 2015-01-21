/** @class SgAsmPEImportItem
 *
 *  Portable Executable Import Object
 *
 *  This node represents a single import object described by data structures referenced by a PE Import Directory.  Such a node
 *  represents data from two, possibly three, distinct data structures in the PE file:  (1) An entry in the Import Lookup
 *  Table, (2) an entry in the Import Address Table, and (3) an optional Hint/Name pair in the (implicit) Hint/Name Table. */

#include "sage3basic.h"
#include "MemoryMap.h"

void
SgAsmPEImportItem::ctor(SgAsmPEImportItemList *parent)
{
    set_parent(parent);
    if (parent)
        parent->get_vector().push_back(this);
    set_by_ordinal(true);
    set_ordinal(0);
    set_hint(0);
    set_hintname_rva(0);
    set_hintname_nalloc(0);
    set_bound_rva(0);

    SgAsmBasicString *name = new SgAsmBasicString("");
    set_name(name);
    name->set_parent(this);
}

void
SgAsmPEImportItem::ctor(SgAsmPEImportDirectory *idir)
{
    ctor(idir->get_imports());
}

void
SgAsmPEImportItem::ctor(SgAsmPEImportDirectory *idir, const std::string &name, unsigned hint)
{
    ctor(idir);
    set_by_ordinal(false);
    get_name()->set_string(name);
    set_hint(hint);
}

void
SgAsmPEImportItem::ctor(SgAsmPEImportDirectory *idir, unsigned ordinal)
{
    ctor(idir);
    set_by_ordinal(true);
    set_ordinal(ordinal);
}

/** Bytes needed to store hint/name pair.  A hint/name pair consists of a two-byte, little endian, unsigned hint and a
 *  NUL-terminated ASCII string.  An optional zero byte padding appears after the string's NUL terminator if necessary to make
 *  the total size of the hint/name pair a multiple of two. */
size_t
SgAsmPEImportItem::hintname_required_size() const
{
    return ALIGN_UP(2/*hint*/ + p_name->get_string().size() + 1/*NUL*/, 2);
}

/** Virtual address of an IAT entry.  Returns the virtual address of the IAT slot for this import item.  This import item must
 *  be linked into the AST in order for this method to succeed. */
rose_addr_t
SgAsmPEImportItem::get_iat_entry_va() const
{
    SgAsmPEImportDirectory *idir = SageInterface::getEnclosingNode<SgAsmPEImportDirectory>(this);
    assert(idir!=NULL); // we need some context in order to find the address
    int idx = idir->find_import_item(this);
    assert(idx>=0); // parent/child connectivity problem
    SgAsmPEFileHeader *fhdr = SageInterface::getEnclosingNode<SgAsmPEFileHeader>(idir);
    assert(fhdr!=NULL);
    rose_addr_t entry_size = fhdr->get_word_size();
    return idir->get_iat_rva().get_va() + idx * entry_size;
}

/** Print debugging info. */
void
SgAsmPEImportItem::dump(FILE *f, const char *prefix, ssize_t idx) const
{
    char p[4096];
    if (idx>=0) {
        sprintf(p, "%sImportItem[%zd].", prefix, idx);
    } else {
        sprintf(p, "%sImportItem.", prefix);
    }

    const int w = std::max(1, DUMP_FIELD_WIDTH-(int)strlen(p));

    if (p_by_ordinal) {
        fprintf(f, "%s%-*s = 0x%04x (%u)", p, w, "ordinal", p_ordinal, p_ordinal);
        if (p_hintname_rva.get_rva()!=0 || p_hint!=0 || !p_name->get_string().empty())
            fprintf(f, " (hint/name rva %s %" PRIuPTR " bytes, hint=0x%04x (%u), name=\"%s\")",
                    p_hintname_rva.to_string().c_str(), p_hintname_nalloc, p_hint, p_hint, p_name->get_string(true).c_str());
        fprintf(f, "\n");
    } else {
        fprintf(f, "%s%-*s = rva %s %" PRIuPTR " bytes, hint=0x%04x (%u), name=\"%s\"",
                p, w, "hint/name", p_hintname_rva.to_string().c_str(), p_hintname_nalloc,
                p_hint, p_hint, p_name->get_string(true).c_str());
        if (p_ordinal!=0)
            fprintf(f, " (ordinal=0x%02x (%u))", p_ordinal, p_ordinal);
        fprintf(f, "\n");
    }

    if (p_bound_rva.get_rva()!=0)
        fprintf(f, "%s%-*s = %s\n", p, w, "bound", p_bound_rva.to_string().c_str());
}
