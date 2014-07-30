/** 
 * \file MFB/include/MFB.hpp
 *
 * \author Tristan Vanderbruggen
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

namespace MFB {

/*!
 * \addtogroup grp_mfb_driver
 * @{
*/

template <template <typename T> class Model>
class Driver {
  public:
    /** Indicate to the Driver that a symbol is in use in a scope. Add any needed include directives.
     *    This version is never used as additionnal information are needed: scope, usage, ...
     *  \param symbol             the symbol we want to use
     *  \return a replacement symbol if needed
     */
    template <typename Object>
    typename Model<Object>::symbol_t useSymbol(typename Model<Object>::symbol_t symbol);

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

/** @} */

}

#endif /* __MULTI_FILE_BUILDER_HPP__ */

