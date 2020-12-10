

#ifndef CTXPROPERTYSTATEFACTORY_H
#define CTXPROPERTYSTATEFACTORY_H 1

#include "PropertyStateFactory.h"
#include "CtxLattice.h"

namespace CodeThorn
{

/// \brief implements the Decorator pattern to enhance the
///   PropertyStateFactory with context specific functionality
template <class CallContext>
struct CtxPropertyStateFactory : PropertyStateFactory
{
    typedef CallContext           context_t;
    typedef CtxLattice<context_t> context_lattice_t;

    explicit
    CtxPropertyStateFactory(PropertyStateFactory& compFac)
    : compFactory(compFac)
    {}

    context_lattice_t* create() ROSE_OVERRIDE
    {
      return new context_lattice_t(compFactory);
    }

    PropertyStateFactory& componentFactory()
    {
      return compFactory;
    }

  private:
    PropertyStateFactory& compFactory;
};


} // end CodeThorn

#endif /* CTXPROPERTYSTATEFACTORY_H */
