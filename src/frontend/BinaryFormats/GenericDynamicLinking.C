/* Generic Dynamic Linking */
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include "sage3basic.h"

#include "stringify.h"

using namespace Rose;

SgAsmGenericDLL::SgAsmGenericDLL(SgAsmGenericString *s) {
    initializeProperties();
    set_name(s);
}

void
SgAsmGenericDLL::set_name(SgAsmGenericString *s)
{
    if (s!=p_name) {
        if (p_name) {
            p_name->set_parent(NULL);
            SageInterface::deleteAST(p_name);
        }
        p_name = s;
        if (p_name)
            p_name->set_parent(this);
        set_isModified(true);
    }
}

void
SgAsmGenericDLL::dump(FILE *f, const char *prefix, ssize_t idx) const
{
    char p[4096];
    if (idx>=0) {
        snprintf(p, sizeof(p), "%sDLL[%zd].", prefix, idx);
    } else {
        snprintf(p, sizeof(p), "%sDLL.", prefix);
    }
    const int w = std::max(size_t{1}, DUMP_FIELD_WIDTH - strlen(p));

    fprintf(f, "%s%-*s = \"%s\"\n", p, w, "lib_name", p_name->get_string(true).c_str());
    for (size_t i = 0; i < p_symbols.size(); i++)
        fprintf(f, "%s%-*s = [%zd] \"%s\"\n", p, w, "symbol_name", i, escapeString(p_symbols[i]).c_str());
}

void
SgAsmGenericDLL::add_symbol(const std::string &s) {
    addSymbol(s);
}

void
SgAsmGenericDLL::addSymbol(const std::string &s) {
    p_symbols.push_back(s);
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Symbols and symbol tables
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

SgAsmGenericString *
SgAsmGenericSymbol::get_name() const
{
    ASSERT_not_null(p_name);                            // [Robb Matzke 2023-03-21]
    return p_name;
}
void
SgAsmGenericSymbol::set_name(SgAsmGenericString *s)
{
    if (s!=p_name) {
        if (p_name) {
            p_name->set_parent(NULL);
            SageInterface::deleteAST(p_name);
        }
        p_name = s;
        if (p_name)
            p_name->set_parent(this);
        set_isModified(true);
    }
}

void
SgAsmGenericSymbol::dump(FILE *f, const char *prefix, ssize_t idx) const
{
    char p[4096];
    if (idx>=0) {
        snprintf(p, sizeof(p), "%sSymbol[%zd].", prefix, idx);
    } else {
        snprintf(p, sizeof(p), "%sSymbol.", prefix);
    }
    const int w = std::max(size_t{1}, DUMP_FIELD_WIDTH - strlen(p));

    fprintf(f, "%s%-*s = \"%s\"\n", p, w, "name", p_name->get_string(true).c_str());

    const char *s_def_state = NULL;
    switch (get_definitionState()) {
      case SYM_UNDEFINED: s_def_state = "undefined"; break;
      case SYM_TENTATIVE: s_def_state = "tentative"; break;
      case SYM_DEFINED:   s_def_state = "defined";   break;
    }
    fprintf(f, "%s%-*s = %s\n", p, w, "def_state", s_def_state);

    const char *s_bind = NULL;
    switch (p_binding) {
      case SYM_NO_BINDING: s_bind = "no-binding"; break;
      case SYM_LOCAL:      s_bind = "local";      break;
      case SYM_GLOBAL:     s_bind = "global";     break;
      case SYM_WEAK:       s_bind = "weak";       break;
    }
    fprintf(f, "%s%-*s = %s\n", p, w, "binding", s_bind);
    
    const char *s_type = NULL;
    switch (p_type) {
      case SYM_NO_TYPE:  s_type = "no-type";  break;
      case SYM_DATA:     s_type = "data";     break;
      case SYM_IFUNC:    s_type = "ifunc";    break;
      case SYM_FUNC:     s_type = "function"; break;
      case SYM_SECTION:  s_type = "section";  break;
      case SYM_FILE:     s_type = "file";     break;
      case SYM_TLS:      s_type = "thread";   break;
      case SYM_REGISTER: s_type = "register"; break;
      case SYM_ARRAY:    s_type = "array";    break;
      case SYM_COMMON:   s_type = "common";   break;
    }
    fprintf(f, "%s%-*s = %s\n", p, w, "type", s_type);
    if (p_bound) {
        fprintf(f, "%s%-*s = [%d] \"%s\"\n", p, w, "bound", p_bound->get_id(), p_bound->get_name()->get_string(true).c_str());
    } else {
        fprintf(f, "%s%-*s = none\n", p, w, "bound");
    }
    
    fprintf(f, "%s%-*s = 0x%08" PRIx64, p, w, "value", p_value);
    if (p_value > 9) {
        fprintf(f, " (unsigned)%" PRIu64, p_value);
        if ((int64_t)p_value < 0) fprintf(f, " (signed)%" PRId64, (int64_t)p_value);
    }
    fputc('\n', f);

    fprintf(f, "%s%-*s = %" PRIu64 " bytes\n", p, w, "size", p_size);
}

SgAsmGenericSymbol::SymbolDefState
SgAsmGenericSymbol::get_def_state() const {
    return get_definitionState();
}

void
SgAsmGenericSymbol::set_def_state(SymbolDefState x) {
    set_definitionState(x);
}

#endif
