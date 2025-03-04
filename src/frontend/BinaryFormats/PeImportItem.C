#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include "sage3basic.h"

#include <Rose/BinaryAnalysis/MemoryMap.h>

using namespace Rose::BinaryAnalysis;

void
SgAsmPEImportItem::initFromParent(SgAsmPEImportItemList *parent) {
    ASSERT_not_null(parent);
    set_parent(parent);
    if (parent)
        parent->get_vector().push_back(this);
    set_by_ordinal(true);
    set_ordinal(0);
    set_hint(0);
    set_hintname_rva(0);
    set_hintname_nalloc(0);
    set_bound_rva(0);
}

SgAsmPEImportItem::SgAsmPEImportItem(SgAsmPEImportItemList *parent) {
    initializeProperties();
    initFromParent(parent);
}

SgAsmPEImportItem::SgAsmPEImportItem(SgAsmPEImportDirectory *idir) {
    initializeProperties();

    ASSERT_not_null(idir);
    SgAsmPEImportItemList *parent = idir->get_imports();
    initFromParent(parent);
}

SgAsmPEImportItem::SgAsmPEImportItem(SgAsmPEImportDirectory *idir, const std::string &name, unsigned hint) {
    initializeProperties();

    ASSERT_not_null(idir);
    SgAsmPEImportItemList *parent = idir->get_imports();
    initFromParent(parent);

    set_by_ordinal(false);
    get_name()->set_string(name);
    set_hint(hint);
}

SgAsmPEImportItem::SgAsmPEImportItem(SgAsmPEImportDirectory *idir, unsigned ordinal) {
    initializeProperties();

    ASSERT_not_null(idir);
    SgAsmPEImportItemList *parent = idir->get_imports();
    initFromParent(parent);

    set_by_ordinal(true);
    set_ordinal(ordinal);
}

size_t
SgAsmPEImportItem::hintname_required_size() const
{
    return hintNameRequiredSize();
}

size_t
SgAsmPEImportItem::hintNameRequiredSize() const
{
    return alignUp(2/*hint*/ + p_name->get_string().size() + 1/*NUL*/, (size_t)2);
}

Address
SgAsmPEImportItem::get_iat_entry_va() const
{
    return get_iatEntryVa();
}

Address
SgAsmPEImportItem::get_iatEntryVa() const
{
    SgAsmPEImportDirectory *idir = SageInterface::getEnclosingNode<SgAsmPEImportDirectory>(this);
    assert(idir!=NULL); // we need some context in order to find the address
    int idx = idir->findImportItem(this);
    assert(idx>=0); // parent/child connectivity problem
    SgAsmPEFileHeader *fhdr = SageInterface::getEnclosingNode<SgAsmPEFileHeader>(idir);
    assert(fhdr!=NULL);
    Address entry_size = fhdr->get_wordSize();
    return *idir->get_iat_rva().va() + idx * entry_size;
}

void
SgAsmPEImportItem::dump(FILE *f, const char *prefix, ssize_t idx) const
{
    char p[4096];
    if (idx>=0) {
        snprintf(p, sizeof(p), "%sImportItem[%zd].", prefix, idx);
    } else {
        snprintf(p, sizeof(p), "%sImportItem.", prefix);
    }

    const int w = std::max(size_t{1}, Rose::DUMP_FIELD_WIDTH - strlen(p));

    if (p_by_ordinal) {
        fprintf(f, "%s%-*s = 0x%04x (%u)", p, w, "ordinal", p_ordinal, p_ordinal);
        if (p_hintname_rva.rva()!=0 || p_hint!=0 || !p_name->get_string().empty())
            fprintf(f, " (hint/name rva %s %" PRIuPTR " bytes, hint=0x%04x (%u), name=\"%s\")",
                    p_hintname_rva.toString().c_str(), p_hintname_nalloc, p_hint, p_hint, p_name->get_string(true).c_str());
        fprintf(f, "\n");
    } else {
        fprintf(f, "%s%-*s = rva %s %" PRIuPTR " bytes, hint=0x%04x (%u), name=\"%s\"",
                p, w, "hint/name", p_hintname_rva.toString().c_str(), p_hintname_nalloc,
                p_hint, p_hint, p_name->get_string(true).c_str());
        if (p_ordinal!=0)
            fprintf(f, " (ordinal=0x%02x (%u))", p_ordinal, p_ordinal);
        fprintf(f, "\n");
    }

    if (p_bound_rva.rva()!=0)
        fprintf(f, "%s%-*s = %s\n", p, w, "bound", p_bound_rva.toString().c_str());
}

#endif
