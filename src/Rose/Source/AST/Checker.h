#ifndef ROSE_Source_AST_Checker_H
#define ROSE_Source_AST_Checker_H

namespace Rose { namespace Source { namespace AST {

/**
 * \brief Checking function that apply to the whole AST
 *
 * Two categories of checker: integrity and consistency
 * If integrity checker reports defects then the consistency checker might *fail*.
 * Similarly, if the edges integrity checker reports defects then the other integrity checkers might *fail*.
 *
 *  
 */
namespace Checker {

/**
 * \brief Apply all existing Checkers
 * @param project
 * @return true if no defect was found
 *
 * Run all integrity checkers followed by the consistency checkers if the former does not find any defects. 
 *
 * Any defect found is saved in Rose::Source::AST::Defect::all (aka Rose::Source::AST::Defects::defect_t<Defects::Kind::any>::all).
 *
 */
bool all(SgProject * project);

/**
 * \brief check multiple low-level properties of the AST's graph.
 * @param project
 * @return true if no defect was found
 *
 * Run all integrity checkers starting by the edge integrity checker then the other integrity checkers if the edges have no defect.
 * 
 * Any defect found is saved in Rose::Source::AST::Defect::all (aka Rose::Source::AST::Defects::defect_t<Defects::Kind::any>::all).
 */
bool integrity(SgProject * project);

/**
 * \brief for all valid node in the AST's graph check that the target of all edges are valid.
 * @param project
 * @return true if no defect was found
 * 
 * Check the edges out of all allocated nodes in the memory pool.
 * For each (non-null) edge, it looks for the pointer in all memory pools.
 * If not found, an IntegrityEdgeDefect is produced with Reason::invalid.
 * If found in an unexpected memory pool, an IntegrityEdgeDefect is produced with Reason::incompatible.
 * If found but not marked as allocated, an IntegrityEdgeDefect is produced with Reason::unallocated.
 * A single edge can yield two defects one with Reason::incompatible and the other with Reason::unallocated.
 *
 * Any defect found is saved in Rose::Source::AST::Defect::all (aka Rose::Source::AST::Defects::defect_t<Defects::Kind::any>::all).
 */
bool integrity_edges(SgProject * project);

/**
 * \brief check all declaration subgraphs (1st-nondef / defn)
 * @param project
 * @return true if no defect was found
 * 
 * Check ...
 * 
 * Any defect found is saved in Rose::Source::AST::Defect::all (aka Rose::Source::AST::Defects::defect_t<Defects::Kind::any>::all).
 */
bool integrity_declarations(SgProject * project);

/**
 * \brief check all symbol subgraphs.
 * @param project
 * @return true if no defect was found
 * 
 * Check ...
 * 
 * Any defect found is saved in Rose::Source::AST::Defect::all (aka Rose::Source::AST::Defects::defect_t<Defects::Kind::any>::all).
 */
bool integrity_symbols(SgProject * project);

/**
 * \brief check all type subgraphs.
 * @param project
 * @return true if no defect was found
 * 
 * Check ...
 * 
 * Any defect found is saved in Rose::Source::AST::Defect::all (aka Rose::Source::AST::Defects::defect_t<Defects::Kind::any>::all).
 */
bool integrity_types(SgProject * project);

/**
 * \brief check (potentially complex) rules
 *
 * @param project
 * @return true if no defect was found
 * 
 * Check ...
 * 
 * Any defect found is saved in Rose::Source::AST::Defect::all (aka Rose::Source::AST::Defects::defect_t<Defects::Kind::any>::all).
 * 
 * @{
 */
bool consistency(SgProject * project);
/** @} */

} } } }

#endif /* ROSE_Source_AST_Checker_H */
