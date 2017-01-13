// The example demonstrates an error that was a 
// problem because the symbols are commonly associated with
// declarations and put into the scope of the declaration,
// however a post-processing step on namespaces resets
// the scopes of declarations in  namespaces to be
// consistant, so this step that resets the scope of
// the declarations to be consistant, now unloads and
// reloads any associated symbols from the old scope
// to the new scope.  Note that for declarations in 
// SgNamespaceDefinitionStatement, the parent does not
// match the scope where the scopes are reset. In general
// the parent is about structure while the scope is about
// semantics.
namespace X {
  template<typename _Iterator, typename _Container> class __normal_iterator;
 }

namespace X {

  template<typename _Iterator, typename _Container> class __normal_iterator {};
 }

