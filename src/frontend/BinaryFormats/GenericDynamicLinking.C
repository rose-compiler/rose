/* Generic Dynamic Linking */
#include "sage3basic.h"
#include "stringify.h"
#define __STDC_FORMAT_MACROS
#include <inttypes.h>

/** Accessors for "name" like ROSETTA's except setting name reparents the SgAsmGenericString. */
SgAsmGenericString *
SgAsmGenericDLL::get_name() const 
{
    return p_name;
}
void
SgAsmGenericDLL::set_name(SgAsmGenericString *s)
{
    if (s!=p_name) {
        if (p_name) {
            p_name->set_parent(NULL);
            delete p_name;
        }
        p_name = s;
        if (p_name)
            p_name->set_parent(this);
        set_isModified(true);
    }
}

/** Print some debugging info */
void
SgAsmGenericDLL::dump(FILE *f, const char *prefix, ssize_t idx) const
{
    char p[4096];
    if (idx>=0) {
        sprintf(p, "%sDLL[%zd].", prefix, idx);
    } else {
        sprintf(p, "%sDLL.", prefix);
    }
    const int w = std::max(1, DUMP_FIELD_WIDTH-(int)strlen(p));

    fprintf(f, "%s%-*s = \"%s\"\n", p, w, "lib_name", p_name->c_str());
    for (size_t i = 0; i < p_symbols.size(); i++)
        fprintf(f, "%s%-*s = [%zd] \"%s\"\n", p, w, "symbol_name", i, p_symbols[i].c_str());
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Symbols and symbol tables
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
SgAsmGenericSymbol::ctor()
{
    p_name = new SgAsmBasicString("");
}

/** Like ROSETTA-generated accessors, but also sets parent */
SgAsmGenericString *
SgAsmGenericSymbol::get_name() const
{
    return p_name;
}
void
SgAsmGenericSymbol::set_name(SgAsmGenericString *s)
{
    if (s!=p_name) {
        if (p_name) {
            p_name->set_parent(NULL);
            delete p_name;
        }
        p_name = s;
        if (p_name)
            p_name->set_parent(this);
        set_isModified(true);
    }
}

std::string
SgAsmGenericSymbol::stringifyDefState() const
{
#ifndef _MSC_VER
    return stringifySgAsmGenericSymbolSymbolDefState(p_def_state);
#else
	ROSE_ASSERT(false);
	return "";
#endif
}

std::string
SgAsmGenericSymbol::stringifyType() const
{
#ifndef _MSC_VER
    return stringifySgAsmGenericSymbolSymbolType(p_type);
#else
	ROSE_ASSERT(false);
	return "";
#endif
}

std::string
SgAsmGenericSymbol::stringifyBinding() const
{
#ifndef _MSC_VER
    return stringifySgAsmGenericSymbolSymbolBinding(p_binding);
#else
	ROSE_ASSERT(false);
	return "";
#endif
}

/** Print some debugging info */
void
SgAsmGenericSymbol::dump(FILE *f, const char *prefix, ssize_t idx) const
{
    char p[4096];
    if (idx>=0) {
        sprintf(p, "%sSymbol[%zd].", prefix, idx);
    } else {
        sprintf(p, "%sSymbol.", prefix);
    }
    const int w = std::max(1, DUMP_FIELD_WIDTH-(int)strlen(p));

    fprintf(f, "%s%-*s = \"%s\"\n", p, w, "name", p_name->c_str());

    const char *s_def_state = NULL;
    switch (p_def_state) {
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
        fprintf(f, "%s%-*s = [%d] \"%s\"\n", p, w, "bound", p_bound->get_id(), p_bound->get_name()->c_str());
    } else {
        fprintf(f, "%s%-*s = none\n", p, w, "bound");
    }
    
    fprintf(f, "%s%-*s = 0x%08"PRIx64, p, w, "value", p_value);
    if (p_value > 9) {
        fprintf(f, " (unsigned)%"PRIu64, p_value);
        if ((int64_t)p_value < 0) fprintf(f, " (signed)%"PRId64, (int64_t)p_value);
    }
    fputc('\n', f);

    fprintf(f, "%s%-*s = %"PRIu64" bytes\n", p, w, "size", p_size);
}
