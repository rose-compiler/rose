#ifndef CALL_GRAPH_WRITER_H
#define CALL_GRAPH_EXPORT_H 20250310

#include <nlohmann/json.h>

#include "ClassHierarchyAnalysis.h"
#include "ObjectLayoutAnalysis.h"

/*!
 * \author Peter Pirkelbauer
 * \date 2025-2025
 */


namespace CodeThorn
{
  using ClassPredicate       = std::function<bool(ClassKeyType)>;

  ClassPredicate anyClass();
  ClassPredicate anyClassWithVtable(const VTableLayoutContainer&);

  nlohmann::json
  toJson( const ObjectLayoutContainer& classLayout,
          const VTableLayoutContainer& vtableLayout,
          ClassPredicate               pred = anyClass()
        );


  /// Writes a JSON object representing object and vtable layout to stream \p os
  void
  writeJson( std::ostream&                os,
             const ObjectLayoutContainer& classLayout,
             const VTableLayoutContainer& vtableLayout,
             ClassPredicate               pred = anyClass()
           );
} // end of namespace CodeThorn

#endif /* CALL_GRAPH_WRITER_H */
