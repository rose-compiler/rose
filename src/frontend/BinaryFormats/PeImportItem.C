#include <rosePublicConfig.h>
#ifdef ROSE_BUILD_BINARY_ANALYSIS_SUPPORT
#include "sage3basic.h"

#include "MemoryMap.h"

using namespace Rose::BinaryAnalysis;

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

size_t
SgAsmPEImportItem::hintname_required_size() const
{
    return alignUp(2/*hint*/ + p_name->get_string().size() + 1/*NUL*/, (size_t)2);
}

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

#endif
