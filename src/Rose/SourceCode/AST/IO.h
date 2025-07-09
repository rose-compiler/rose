#ifndef ROSE_SourceCode_AST_IO_H
#define ROSE_SourceCode_AST_IO_H
#include <RoseFirst.h>

#include <list>
#include <string>
#include <iostream>

#include "sage3basic.hhh"

class SgProject;

namespace Rose { namespace SourceCode { namespace AST {

//! Function used to load, merge, prune, link, delete
namespace IO {

/**
 * \brief Load ASTs that have been saved to files.
 *
 * @param project the project to which the files will be happened
 * @param filepaths a std::list of path to AST files
 * 
 * This function happens the content of each AST file to the given project.
 * It is used by the command line option: -rose:ast:read.
 * This function will leave the AST in an inconsistent state and Rose::SourceCode::AST::merge must be run to fix it.
 */
ROSE_DLL_API void load(SgProject * project, std::list<std::string> const & filepaths);

/**
 * \brief Performs sharing of AST nodes followed by linking accross translation units.
 *
 * @param project
 * 
 * This function is mainly used after loading ASTs from files.
 * It is used by the command line option: -rose:ast:read and -rose:ast:merge.
 * It simply calls three functions in sequence: Rose::SourceCode::AST::share, Rose::SourceCode::AST::prune, and Rose::SourceCode::AST::link.
 * It also provides statistics if Rose is in verbose > 0.
 *
 */
ROSE_DLL_API void merge(SgProject * project);

/**
 * \brief Eliminates duplicated nodes from the AST.
 *
 * @param project
 * 
 * This function is mainly used when two or more translation units are merged together.
 * Particularly, the ASTs of header files is duplicated when included from different translation-units.
 */
ROSE_DLL_API void share(SgProject * project);

/**
 * \brief Removes nodes that are not reachable from the project node.
 *
 * @param project
 * 
 * This function is mainly used after sharing redundant nodes.
 * It can also be useful to clean-up after transfortions.
 */
ROSE_DLL_API void prune(SgProject * project);

/**
 * \brief Links declarations from multiple translation units.
 *
 * @param project
 * 
 * This function provide functionalities similar to a linker in a compiler toolchain.
 * It links forward declarations and definitions of variables, functions, classes, etc.
 */
ROSE_DLL_API void link(SgProject * project);

/**
 * \brief Empties the memory pool of all nodes.
 */
ROSE_DLL_API void clear();

/**
 * \brief Delete all memory allocated by ROSE.
 */
ROSE_DLL_API void free();

} } } }

#endif /* ROSE_SourceCode_AST_IO_H */

