
#ifndef _SAGEFUNCTORS_H
#define _SAGEFUNCTORS_H

/// \file sageFunctors.h
///       This file implements utility functors for using sage containers
///       with STL functions:
///       - ScopeSetter, VarRefBuilder, InitNameCloner, and SageInserter
///         (a generic inserter for sage containers).
/// \email peter.pirkelbauer@llnl.gov

#include "sageInterface.h"
#include "sageBuilder.h"

namespace sg
{
  /// \brief   returns a deep copy of a sage node
  /// \details allows NULL input nodes (in contrast to SageInterface::deepCopy)
  template <class SageNode>
  static inline
  SageNode* cloneNode(const SageNode* n)
  {
    if (!n) return 0;

    return SageInterface::deepCopy(n);
  }

  /// \brief unified interface for storing an element in a sage container
  /// \note  internal use
  static inline
  void _append(SgExprListExp& container, SgExpression* elem)
  {
    SageInterface::appendExpression(&container, elem);
  }

  /// \overload
  static inline
  void _append(SgFunctionParameterList& container, SgInitializedName* elem)
  {
    SageInterface::appendArg(&container, elem);
  }

  /// \brief Functor setting the scope of a sage node to a specified (at Functor construction time) scope
  struct ScopeSetter
  {
      explicit
      ScopeSetter(SgScopeStatement& the_scope)
      : scope(the_scope)
      {}

      template <class ScopedSageNode>
      void handle(ScopedSageNode* scopeElem) const
      {
        ROSE_ASSERT(scopeElem);

        scopeElem->set_scope(&scope);
      }

      void operator()(SgStatement* scopeElem)       const { handle(scopeElem); }
      void operator()(SgInitializedName* scopeElem) const { handle(scopeElem); }

    private:
      SgScopeStatement& scope;
  };

  /// \brief Functor building a variable reference from an initialized name
  struct VarRefBuilder
  {
      explicit
      VarRefBuilder(SgScopeStatement& the_scope)
      : scope(the_scope)
      {}

      SgVarRefExp* operator()(SgInitializedName* initName) const
      {
        return SageBuilder::buildVarRefExp(initName, &scope);
      }

    private:
      SgScopeStatement& scope;
  };

  /// \brief Functor copying an initialized name into a different scope
  struct InitNameCloner
  {
      InitNameCloner(SgDeclarationStatement& declaration, SgScopeStatement* enclosing_scope = 0)
   // DQ (3/25/2017): Remove to avoid Clang warning about unused private variable.
   // : decl(declaration), 
      : scope(enclosing_scope)
      {}

      SgInitializedName* operator()(const SgInitializedName* orig) const
      {
        SgInitializer*     copy_init = cloneNode(orig->get_initializer());
        SgInitializedName* res = SageBuilder::buildInitializedName(orig->get_name(), orig->get_type(), copy_init);

        res->set_scope(scope);

        return res;
      }

    private:
   // DQ (3/25/2017): Remove to avoid Clang warning about unused private variable.
   // SgDeclarationStatement& decl;
      SgScopeStatement*       scope;
  };

  /// \brief   Generic inserter for sage containers
  /// \tparam  SageSequenceContainer, a sage container that supports appending an element
  /// \details forwards actual insert to function family _append
  template <class SageSequenceContainer>
  struct SageInserter : std::iterator<std::output_iterator_tag, void, void, void, void>
  {
    typedef SageSequenceContainer Container;

    Container& container;

    explicit
    SageInserter(Container& cont)
    : container(cont)
    {}

    // \todo SageElem should be derived form the container type
    template <class SageElem>
    SageInserter& operator=(SageElem* elem)
    {
      _append(container, elem);
      return *this;
    }

    SageInserter& operator*()     { return *this; }
    SageInserter& operator++()    { return *this; }
    SageInserter& operator++(int) { return *this; }
  };

  /// \brief   generates a SageInserter, adding elements at the end of a sequence
  /// \tparam  SageSequenceContainer, a sage container that supports appending an element
  template <class SageSequenceContainer>
  SageInserter<SageSequenceContainer>
  sage_inserter(SageSequenceContainer& cont)
  {
    return SageInserter<SageSequenceContainer>(cont);
  }
}

#endif /* _SAGEFUNCTORS_H */
