/**
 * Multi-Files Builder
 *
 * A template for AST builder over multiple compilation units.
 * Functionnalities identified:
 *   - building an AST node
 *   - scope management:
 *       + Which file?
 *       + Where in this file?
 *   - Headers management
 *   - 
 *
 */

#ifndef __MULTI_FILE_BUILDER_HPP__
#define __MULTI_FILE_BUILDER_HPP__

#include <set>
#include <utility>
#include <string>

class SgSymbol;
class SgScopeStatement;
class SgSourceFile;
class SgProject;

namespace MultiFileBuilder {

template <template <typename T> class Model>
class Driver {
  private:
    std::set<SgSourceFile *> source_files;
    std::set<SgSourceFile *> header_files;

    SgProject * project;

  public:
    Driver(const std::string & filename_, SgProject * project_);

    /**  Build the main declaration for an object
     *    \param desc a model specific object descritor
     *    \return the symbol of the main declaration and its (empty) definition
     */
    template <typename Object>
    typename Model<Object>::build_result_t build(const typename Model<Object>::object_desc_t & desc);

    /** Indicate to the 
     *
     */
    template <typename Object>
    typename Model<Object>::symbol_t useSymbol(typename Model<Object>::symbol_t symbol, Model::scope_t scope);

  protected:
    template <typename Object>
    typename Model<Object>::build_scopes_t getBuildScopes(const typename Model<Object>::object_desc_t & desc);
};

}

#endif /* __MULTI_FILE_BUILDER_HPP__ */

