#ifndef ROSE_RTIHELPERS_H
#define ROSE_RTIHELPERS_H

#include <string>
#include <vector>
#include <list>
#include <set>
#include <sstream>
#include <iomanip>
#include <boost/lexical_cast.hpp>
#include "rose.h"

// Helpful functions for Cxx_GrammarRTI.C
// Probably should not be included anywhere else

#if ROSE_USE_VALGRIND
#include <valgrind/valgrind.h>
#include <valgrind/memcheck.h>
#include <stdio.h>
static void doUninitializedFieldCheck(const char* fieldName, void* fieldPointer, size_t fieldSize, void* wholeObject, const char* className) {
  if (VALGRIND_CHECK_READABLE(fieldPointer, fieldSize)) {
    fprintf(stderr, "Warning: uninitialized field p_%s of object %p of class %s\n", fieldName, wholeObject, className);
  }
}
#endif

template <typename T>
static std::string toStringForRTI(const T& x) {
  std::ostringstream ss;
  ss << x;
  return ss.str();
}

template <typename T>
static std::string toStringForRTI(const std::vector<T>& x) {
  std::ostringstream ss;
  ss << "[";
  for (typename std::vector<T>::const_iterator i = x.begin(); i != x.end(); ++i) {if (i != x.begin()) ss << ", "; ss << (*i);}
  ss << "]";
  return ss.str();
}

// DQ (8/8/2008): Added support for type used in binary file format support.
template <typename T>
static std::string toStringForRTI(const std::vector<std::pair<T,T> >& x) {
  std::ostringstream ss;
  ss << "[";
  for (typename std::vector<std::pair<T,T> >::const_iterator i = x.begin(); i != x.end(); ++i) {if (i != x.begin()) ss << ", "; ss << i->first << "->" << i->second;}
  ss << "]";
  return ss.str();
}


static std::string toStringForRTI(const std::vector<bool>& x) {
  std::ostringstream ss;
  ss << "[";
  for (std::vector<bool>::const_iterator i = x.begin(); i != x.end(); ++i) {if (i != x.begin()) ss << ", "; ss << (*i ? "T" : "F");}
  ss << "]";
  return ss.str();
}

template <typename T>
static std::string toStringForRTI(const std::list<T>& x) {
  std::ostringstream ss;
  ss << "[";
  for (typename std::list<T>::const_iterator i = x.begin(); i != x.end(); ++i) {if (i != x.begin()) ss << ", "; ss << (*i);}
  ss << "]";
  return ss.str();
}

template <typename T>
static std::string toStringForRTI(const std::set<T>& x) {
  std::ostringstream ss;
  ss << "[";
  for (typename std::set<T>::const_iterator i = x.begin(); i != x.end(); ++i) {if (i != x.begin()) ss << ", "; ss << (*i);}
  ss << "]";
  return ss.str();
}

template <typename K, typename V>
static std::string toStringForRTI(const std::map<K, V>& x) {
  std::ostringstream ss;
  ss << "[";
  for (typename std::map<K, V>::const_iterator i = x.begin(); i != x.end(); ++i) {if (i != x.begin()) ss << ", "; ss << i->first << "->" << i->second;}
  ss << "]";
  return ss.str();
}


#if !OLD_GRAPH_NODES
// DQ (8/18/2008): Added support for new Graph IR node.
static std::string toStringForRTI(const SgGraphNodeDirectedGraphEdgeMultimapPtrList & x)
{
  std::ostringstream ss;
  ss << "[";
// for (SgGraphNodeDirectedGraphEdgeMultimapPtrList::const_iterator i = x.begin(); i != x.end(); ++i) {if (i != x.begin()) ss << ", "; ss << i->first << "->" << i->second;}
  ss << "]";
  return ss.str();
}

// DQ (8/18/2008): Added support for new Graph IR node.
static std::string toStringForRTI(const SgStringGraphNodeMapPtrList & x)
{
  std::ostringstream ss;
  ss << "[";
// for (SgStringGraphNodeMapPtrList::const_iterator i = x.begin(); i != x.end(); ++i) {if (i != x.begin()) ss << ", "; ss << i->first << "->" << i->second;}
  ss << "]";
  return ss.str();
}


// DQ (8/18/2008): Added support for new Graph IR node.
static std::string toStringForRTI(const SgIntegerGraphNodeMapPtrList & x)
{
  std::ostringstream ss;
  ss << "[";
  for (SgIntegerGraphNodeMapPtrList::const_iterator i = x.begin(); i != x.end(); ++i) {if (i != x.begin()) ss << ", "; ss << i->first << "->" << i->second;}
  ss << "]";
  return ss.str();
}

// DQ (8/18/2008): Added support for new Graph IR node.
static std::string toStringForRTI(const SgGraphNodeUndirectedGraphEdgeMultimapPtrList & x)
{
  std::ostringstream ss;
  ss << "[";
// for (SgGraphNodeUndirectedGraphEdgeMultimapPtrList::const_iterator i = x.begin(); i != x.end(); ++i) {if (i != x.begin()) ss << ", "; ss << i->first << "->" << i->second;}
  ss << "]";
  return ss.str();
}
#endif


static std::string toStringForRTI(const SgAccessModifier& m) {
  return m.displayString();
}

static std::string toStringForRTI(const SgUPC_AccessModifier& m) {
  return m.displayString();
}

static std::string toStringForRTI(const SgConstVolatileModifier& m) {
  return m.displayString();
}

static std::string toStringForRTI(const SgElaboratedTypeModifier& m) {
  return m.displayString();
}

static std::string toStringForRTI(const SgTypeModifier& m) {
  return m.displayString();
}

static std::string toStringForRTI(const SgStorageModifier& m) {
  return m.displayString();
}

static std::string toStringForRTI(const SgDeclarationModifier& m) {
  return m.displayString();
}

static std::string toStringForRTI(const SgFunctionModifier& m) {
  return m.displayString();
}

static std::string toStringForRTI(const SgSpecialFunctionModifier& m) {
  return m.displayString();
}

static std::string toStringForRTI(const SgName& n) {
  return n.getString();
}

#if 0
// None of these seem to be used

template <typename Sym>
static std::string toStringForRTISymbol(Sym* sym) {
  std::ostringstream ss;
  ss << sym;
  if (sym) {
    ss << ": varsym " << sym->get_name().str() << " declared at 0x" << std::hex << (sym->get_declaration());
  }
  return ss.str();
}

static std::string toStringForRTI(SgVariableSymbol* sym) {return toStringForRTISymbol(sym);}
static std::string toStringForRTI(SgFunctionSymbol* sym) {return toStringForRTISymbol(sym);}
static std::string toStringForRTI(SgMemberFunctionSymbol* sym) {return toStringForRTISymbol(sym);}

static std::string toStringForRTI(const SgSymbolTable&) {return "<no output operator defined for this type>";}
static std::string toStringForRTI(const SgSymbolHashBase::iterator&) {return "<no output operator defined for this type>";}
#endif

#endif // ROSE_RTIHELPERS_H
