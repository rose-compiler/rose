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
#include <map>
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
    unsigned long file_id_counter;

    std::map<unsigned long, std::string> id_to_name_map;

    std::map<unsigned long, std::pair<SgSourceFile *, SgSourceFile *> > file_pair_map;
    std::map<std::string, unsigned long> file_pair_name_map;

    std::map<unsigned long, SgSourceFile *> standalone_source_file_map;
    std::map<std::string, unsigned long> standalone_source_file_name_map;

    std::map<SgSourceFile *, unsigned long> file_to_id_map;

    std::map<SgSourceFile *, std::set<unsigned long> > file_id_to_accessible_file_id_map;

    std::map<SgSymbol *, unsigned long> symbol_to_file_id_map;

    std::map<SgSymbol *, SgSymbol *> parent_map;

  private:
    void addIncludeDirectives(SgSourceFile * target_file, unsigned long to_be_included_file_id);

    template <typename Object>
    void createForwardDeclaration(typename Model<Object>::symbol_t symbol, SgSourceFile * target_file);

  public:
    SgProject * project;

  public:
    Driver(SgProject * project_ = NULL);

    unsigned long createPairOfFiles(const std::string & name);

    unsigned long createStandaloneSourceFile(const std::string & name);
    unsigned long addStandaloneSourceFile(SgSourceFile * source_file);

    /** Indicate to the Driver that a symbol is in use in a scope. Add any needed include directives.
     *  \param symbol             the symbol we wantr to use
     *  \param scope              the scope in which we want to use it
     *  \param need_forward_only  the usage of this symbol only requires a forward declaration
     *  \return a replacement symbol if needed
     */
    template <typename Object>
    typename Model<Object>::symbol_t useSymbol(typename Model<Object>::symbol_t symbol, SgScopeStatement * scope, bool need_forward_only = false);

    /** Indicate to the Driver that a symbol is in use in a source file. Add any needed include directives.
     *  \param symbol             the symbol we wantr to use
     *  \param file               the file in which we want to use it
     *  \param need_forward_only  the usage of this symbol only requires a forward declaration
     *  \return a replacement symbol if needed
     */
    template <typename Object>
    typename Model<Object>::symbol_t useSymbol(typename Model<Object>::symbol_t symbol, SgSourceFile * file, bool need_forward_only = false);

    /** Indicate to the Driver that a symbol is in use by a file_id (if pair of file assume the source file if not otherwise precised). Add any needed include directives.
     *  \param symbol             the symbol we wantr to use
     *  \param file_id            the file_id in which we want to use it
     *  \param needed_in_header   if refering to a pair of file this flag will signal that the symbol is used by both headers and include file
     *  \return a replacement symbol if needed
     */
    template <typename Object>
    typename Model<Object>::symbol_t useSymbol(typename Model<Object>::symbol_t symbol, unsigned long file_id, bool needed_in_header = false, bool need_forward_only = false);

    /**  Build the main declaration for an object
     *    \param desc a model specific object descritor
     *    \return the symbol of the main declaration and its (empty) definition
     */
    template <typename Object>
    typename Model<Object>::build_result_t build(const typename Model<Object>::object_desc_t & desc);

  protected:
    /** Provide the scopes need to build the object
     * \return a structure containing all necessary scope information
     */
    template <typename Object>
    typename Model<Object>::build_scopes_t getBuildScopes(const typename Model<Object>::object_desc_t & desc);
};

}

#ifndef NO_TEMPLATE_DEFINITION
#include "MFB/multi-file-builder.tpp"
#endif

#endif /* __MULTI_FILE_BUILDER_HPP__ */

