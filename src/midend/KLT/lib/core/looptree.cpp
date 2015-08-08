
#include "sage3basic.h"

#include "KLT/Core/looptree.hpp"
#include "KLT/Core/descriptor.hpp"

#include <iostream>

namespace KLT {

namespace LoopTree {

extraction_context_t::extraction_context_t(loop_map_t & loop_map_, const vsym_set_t & data_) :
  loop_map(loop_map_), data(data_), loop_cnt(0), parameters(), iterators(), locals() {}

void extraction_context_t::addParameter(vsym_t * vsym) {
  if (iterators.find(vsym) == iterators.end() && locals.find(vsym) == locals.end() && data.find(vsym) == data.end())
    parameters.insert(vsym);
}

void extraction_context_t::addIterator(vsym_t * vsym) {
  iterators.insert(vsym);
}

void extraction_context_t::addLocal(vsym_t * vsym) {
  locals.insert(vsym);
}

void extraction_context_t::processVarRefs(SgNode * node) {
  std::vector<SgVarRefExp *> var_refs = SageInterface::querySubTree<SgVarRefExp>(node);
  std::vector<SgVarRefExp *>::const_iterator it_var_ref;
  for (it_var_ref = var_refs.begin(); it_var_ref != var_refs.end(); it_var_ref++) {
    SgVariableSymbol * sym = (*it_var_ref)->get_symbol();
    assert(sym->get_declaration() != NULL);
    assert(sym->get_declaration()->get_parent() != NULL);
    assert(sym->get_declaration()->get_parent()->get_parent() != NULL);

    if (isSgClassDefinition(sym->get_declaration()->get_parent()->get_parent())) continue; // reference to a field ('x' in 'a.x')

    addParameter(sym);
  }
}

size_t extraction_context_t::nextLoopID() {
  return loop_cnt++;
}

void extraction_context_t::registerLoop(SgForStatement * for_stmt, loop_t * loop) {
  loop_map.insert(std::pair<SgForStatement *, size_t>(for_stmt, loop->id));
}

const extraction_context_t::vsym_set_t & extraction_context_t::getParameters() const {
  return parameters;
}

////////////////////////////////////////////////

kind_e kindOf(SgStatement * stmt) {
       if ( isSgBasicBlock          (stmt)    ) return e_block;
  else if ( isSgForStatement        (stmt)    ) return e_loop;
  else if ( isSgIfStmt              (stmt)    ) return e_cond;
  else if ( isSgExprStatement       (stmt) ||
            isSgVariableDeclaration (stmt)    ) return e_stmt;
  else if ( isSgPragmaDeclaration   (stmt)    ) return e_ignored;
  else                                         return e_unknown;
}

// Constructors

node_t::node_t(kind_e kind_) :
  kind(kind_), parent(NULL) {}

node_t::~node_t() {}

block_t::block_t() :
  node_t(e_block), children() {}

block_t::~block_t() {}

cond_t::cond_t(SgExpression * cond) :
  node_t(e_cond), condition(cond), branch_true(NULL), branch_false(NULL) {}

cond_t::~cond_t() {}

loop_t::loop_t(size_t id_, SgVariableSymbol * it, SgExpression * lb, SgExpression * ub, SgExpression * stride_) :
  node_t(e_loop), id(id_), iterator(it), lower_bound(lb), upper_bound(ub), stride(stride_), body(NULL) {}

loop_t::~loop_t() {}

stmt_t::stmt_t(SgStatement * stmt) :
  node_t(e_stmt), statement(stmt) {}

stmt_t::~stmt_t() {}

tile_t::tile_t(size_t id_, unsigned long kind_, size_t order_, SgExpression * param_, loop_t * loop_, size_t tile_id_) :
  node_t(e_tile), id(id_), kind(kind_), order(order_), param(param_), loop(loop_), tile_id(tile_id_), next_tile(NULL), next_node(NULL) {}

tile_t::~tile_t() {}

// Extract Methods

node_t * node_t::extract(SgStatement * stmt, extraction_context_t & ctx) {
  node_t * res = NULL;

  switch (kindOf(stmt)) {
    case e_block: res = block_t::extract(stmt, ctx); break;
    case e_cond:  res = cond_t::extract (stmt, ctx); break;
    case e_loop:  res = loop_t::extract (stmt, ctx); break;
    case e_stmt:  res = stmt_t::extract (stmt, ctx); break;

    case e_ignored:
      std::cerr << "[Warning] (KLT::LoopTrees::node_t::extract) Ignored statement " << stmt << " (type: " << stmt->class_name() << ")." << std::endl;
      break;

    case e_unknown:
    default: 
      std::cerr << "[Error] (KLT::LoopTrees::node_t::extract) Uknown statement " << stmt << " (type: " << stmt->class_name() << ")." << std::endl;
      assert(false);
  }

  if (res == NULL) {
    std::cerr << "[Warning] (KLT::LoopTrees::node_t::extract) Statement " << stmt << " (type: " << stmt->class_name() << ") cannot be translated." << std::endl;
  }

  return res;
}

block_t * block_t::extract(SgStatement * stmt, extraction_context_t & ctx) {
  SgBasicBlock * bb = isSgBasicBlock(stmt);
  assert(bb != NULL);

  block_t * block = new block_t();

  std::vector<SgStatement *>::const_iterator it_stmt;
  for (it_stmt = bb->get_statements().begin(); it_stmt != bb->get_statements().end(); it_stmt++) {
    node_t * node = node_t::extract(*it_stmt, ctx);
    if (node != NULL)
      block->children.push_back(node);
  }

  return block;
}

cond_t * cond_t::extract(SgStatement * stmt, extraction_context_t & ctx) {
  SgIfStmt * if_stmt = isSgIfStmt(stmt);
  assert(if_stmt != NULL);

  SgExprStatement * cond_stmt = isSgExprStatement(if_stmt->get_conditional());
  assert(cond_stmt != NULL);
  SgExpression * cond_expr = cond_stmt->get_expression();
  assert(cond_expr != NULL);

  ctx.processVarRefs(cond_expr);

  cond_t * cond = new cond_t(cond_expr);
      
  cond->branch_true = node_t::extract(if_stmt->get_true_body(), ctx);
  cond->branch_false = node_t::extract(if_stmt->get_false_body(), ctx);
      
  return cond;
}

loop_t * loop_t::extract(SgStatement * stmt, extraction_context_t & ctx) {
  SgForStatement * for_stmt = isSgForStatement(stmt);
  assert(for_stmt != NULL);

  SgVariableSymbol * iterator = NULL;
  SgExpression * lower_bound = NULL;
  SgExpression * upper_bound = NULL;
  SgExpression * stride = NULL;

  bool canon_loop = SageInterface::getForLoopInformations(for_stmt, iterator, lower_bound, upper_bound, stride);
  assert(canon_loop == true);

  ctx.addIterator(iterator);

  ctx.processVarRefs(for_stmt->get_for_init_stmt());
  ctx.processVarRefs(for_stmt->get_test());
  ctx.processVarRefs(for_stmt->get_increment());

  loop_t * loop = new loop_t(ctx.nextLoopID(), iterator, lower_bound, upper_bound, stride);

  ctx.registerLoop(for_stmt, loop);

  loop->body = node_t::extract(for_stmt->get_loop_body(), ctx);

  return loop;
}

stmt_t * stmt_t::extract(SgStatement * stmt, extraction_context_t & ctx) {
  if (isSgExprStatement(stmt)) {
    SgExprStatement * expr_stmt = (SgExprStatement *)stmt;
    SgExpression * expr = expr_stmt->get_expression();
    assert(expr != NULL);

    ctx.processVarRefs(expr);

    return new stmt_t(stmt);
  }
  else if (isSgVariableDeclaration(stmt)) {
    SgVariableDeclaration * var_decl = isSgVariableDeclaration(stmt);
    assert(var_decl != NULL);
    SgScopeStatement * scope = var_decl->get_scope();
    assert(scope != NULL);

    const std::vector<SgInitializedName *> & decls = var_decl->get_variables();
    std::vector<SgInitializedName *>::const_iterator it_decl;
    for (it_decl = decls.begin(); it_decl != decls.end(); it_decl++) {
      SgVariableSymbol * var_sym = scope->lookup_variable_symbol((*it_decl)->get_name());
      assert(var_sym != NULL);
      ctx.addLocal(var_sym);
    }

    ctx.processVarRefs(stmt);

    return new stmt_t(stmt);
  }
  else assert(false);
}

std::string node_t::getGraphVizLabel() const {
  std::ostringstream oss; oss << "node_" << this;
  return oss.str();
}

void block_t::toGraphViz(std::ostream & out, std::string indent) const {
  out << indent << getGraphVizLabel() << " [label=\"block\"]" << std::endl;
  std::vector<node_t *>::const_iterator it;
  for (it = children.begin(); it != children.end(); it++) {
    (*it)->toGraphViz(out, indent + "  ");
    out << indent + "  " << getGraphVizLabel() << " -> " << (*it)->getGraphVizLabel() << std::endl;
  }

  if (parent != NULL) {
    out << indent + "  " << parent->getGraphVizLabel() << " -> " << getGraphVizLabel() << " [label=\"P\", contraint=false]" << std::endl;
  }
  else {
    out << indent + "  " << parent->getGraphVizLabel() << " -> " << "node_null" << " [label=\"P\", contraint=false, color=red]" << std::endl;
  }
}

void cond_t::toGraphViz(std::ostream & out, std::string indent) const {
  out << indent << getGraphVizLabel() << " [label=\"If (" << condition->unparseToString() << ")\"]" << std::endl;
  branch_true->toGraphViz(out, indent + "  ");
  out << indent + "  " << getGraphVizLabel() << " -> " << branch_true->getGraphVizLabel() << " [label=\"T\"]" << std::endl;
  branch_false->toGraphViz(out, indent + "  ");
  out << indent + "  " << getGraphVizLabel() << " -> " << branch_false->getGraphVizLabel() << " [label=\"F\"]" << std::endl;

  if (parent != NULL) {
    out << indent + "  " << parent->getGraphVizLabel() << " -> " << getGraphVizLabel() << " [label=\"P\", contraint=false]" << std::endl;
  }
  else {
    out << indent + "  " << parent->getGraphVizLabel() << " -> " << "node_null" << " [label=\"P\", contraint=false, color=red]" << std::endl;
  }
}

void loop_t::toGraphViz(std::ostream & out, std::string indent) const {
  out << indent << getGraphVizLabel() << " [label=\"Loop#" << id << "\\niterator=" << iterator->get_name().getString() << "\\nlower=" << lower_bound->unparseToString() << "\\nupper=" << upper_bound->unparseToString() << "\\nstride=" << stride->unparseToString() << "\\n\"]" << std::endl;
  body->toGraphViz(out, indent + "  ");
  out << indent + "  " << getGraphVizLabel() << " -> " << body->getGraphVizLabel() << std::endl;

  if (parent != NULL) {
    out << indent + "  " << parent->getGraphVizLabel() << " -> " << getGraphVizLabel() << " [label=\"P\", contraint=false]" << std::endl;
  }
  else {
    out << indent + "  " << parent->getGraphVizLabel() << " -> " << "node_null" << " [label=\"P\", contraint=false, color=red]" << std::endl;
  }
}

void tile_t::toGraphViz(std::ostream & out, std::string indent) const {
  out << indent << getGraphVizLabel() << " [label=\"Tile #" << id << "\\nkind=" << kind << "\\norder=" << order << "\\nloop_id=" << loop->id << "\\ntile_id=" << tile_id << "\\n";
  if (param != NULL) out << "param=" << param->unparseToString() << "\\n";
  out << "\"]" << std::endl;

  if (next_tile != NULL) {
    next_tile->toGraphViz(out, indent + "  ");
    out << indent + "  " << getGraphVizLabel() << " -> " << next_tile->getGraphVizLabel() << std::endl;
  }
  if (next_node != NULL) {
    next_node->toGraphViz(out, indent + "  ");
    out << indent + "  " << getGraphVizLabel() << " -> " << next_node->getGraphVizLabel() << std::endl;
  }

  if (parent != NULL) {
    out << indent + "  " << parent->getGraphVizLabel() << " -> " << getGraphVizLabel() << " [label=\"P\", contraint=false]" << std::endl;
  }
  else {
    out << indent + "  " << parent->getGraphVizLabel() << " -> " << "node_null" << " [label=\"P\", contraint=false, color=red]" << std::endl;
  }
}

void stmt_t::toGraphViz(std::ostream & out, std::string indent) const {
  out << indent << getGraphVizLabel() << " [label=\"" << statement->unparseToString() << "\"]" << std::endl;

  if (parent != NULL) {
    out << indent + "  " << parent->getGraphVizLabel() << " -> " << getGraphVizLabel() << " [label=\"P\", contraint=false]" << std::endl;
  }
  else {
    out << indent + "  " << parent->getGraphVizLabel() << " -> " << "node_null" << " [label=\"P\", contraint=false, color=red]" << std::endl;
  }
}

void block_t::collectLoops(std::vector<Descriptor::loop_t *> & loops, std::map<const loop_t *, Descriptor::loop_t *> & loop_translation_map) const {
  std::vector<node_t *>::const_iterator it;
  for (it = children.begin(); it != children.end(); it++)
    (*it)->collectLoops(loops, loop_translation_map);
}

void block_t::collectTiles(std::vector<Descriptor::tile_t *> & tiles, const std::map<const loop_t *, Descriptor::loop_t *> & loop_translation_map) const {
  std::vector<node_t *>::const_iterator it;
  for (it = children.begin(); it != children.end(); it++)
    (*it)->collectTiles(tiles, loop_translation_map);
}

void cond_t::collectLoops(std::vector<Descriptor::loop_t *> & loops, std::map<const loop_t *, Descriptor::loop_t *> & loop_translation_map) const {
  branch_true->collectLoops(loops, loop_translation_map);
  branch_false->collectLoops(loops, loop_translation_map);
}

void cond_t::collectTiles(std::vector<Descriptor::tile_t *> & tiles, const std::map<const loop_t *, Descriptor::loop_t *> & loop_translation_map) const {
  branch_true->collectTiles(tiles, loop_translation_map);
  branch_false->collectTiles(tiles, loop_translation_map);
}

template <class T>
bool cmp_id(size_t val, T * elem) {
  return val < elem->id;
}

template <class T>
void insert(T * t, std::vector<T *> & Ts) {
  typename std::vector<T *>::iterator it = Ts.begin();
  while (it != Ts.end() && t->id < (*it)->id) it++;
  Ts.insert(it, t);
}

void loop_t::collectLoops(std::vector<Descriptor::loop_t *> & loops, std::map<const loop_t *, Descriptor::loop_t *> & loop_translation_map) const {
  Descriptor::loop_t * loop_desc = new Descriptor::loop_t(id, lower_bound, upper_bound, stride, iterator);

  loop_translation_map.insert(std::pair<const loop_t *, Descriptor::loop_t *>(this, loop_desc));

  std::vector<Descriptor::loop_t *>::iterator pos = std::upper_bound(loops.begin(), loops.end(), id, cmp_id<Descriptor::loop_t>);
  assert(pos == loops.end() || (*pos)->id != id);
  loops.insert(pos, loop_desc);

  body->collectLoops(loops, loop_translation_map);
}

void loop_t::collectTiles(std::vector<Descriptor::tile_t *> & tiles, const std::map<const loop_t *, Descriptor::loop_t *> & loop_translation_map) const {
  body->collectTiles(tiles, loop_translation_map);
}

void tile_t::collectLoops(std::vector<Descriptor::loop_t *> & loops, std::map<const loop_t *, Descriptor::loop_t *> & loop_translation_map) const {
  Descriptor::loop_t * loop_desc = new Descriptor::loop_t(loop->id, loop->lower_bound, loop->upper_bound, loop->stride, loop->iterator);

  loop_translation_map.insert(std::pair<const loop_t *, Descriptor::loop_t *>(loop, loop_desc));

  std::vector<Descriptor::loop_t *>::iterator pos = std::upper_bound(loops.begin(), loops.end(), loop->id, cmp_id<Descriptor::loop_t>);
  if (pos == loops.end() || (*pos)->id != loop->id)
    loops.insert(pos, loop_desc);

  if (next_tile != NULL) next_tile->collectLoops(loops, loop_translation_map);
  if (next_node != NULL) next_node->collectLoops(loops, loop_translation_map);
}

void tile_t::collectTiles(std::vector<Descriptor::tile_t *> & tiles, const std::map<const loop_t *, Descriptor::loop_t *> & loop_translation_map) const {
  Descriptor::tile_t * tile_desc = new Descriptor::tile_t(id, (Descriptor::tile_kind_e)kind, order, param);
  {
    std::vector<Descriptor::tile_t *>::iterator pos = std::upper_bound(tiles.begin(), tiles.end(), id, cmp_id<Descriptor::tile_t>);
    tiles.insert(pos, tile_desc);
  }
  std::map<const loop_t *, Descriptor::loop_t *>::const_iterator it = loop_translation_map.find(loop);
  assert(it != loop_translation_map.end());
  {
    std::vector<Descriptor::tile_t *>::iterator pos = std::upper_bound(it->second->tiles.begin(), it->second->tiles.end(), id, cmp_id<Descriptor::tile_t>);
    it->second->tiles.insert(pos, tile_desc);
  }

  if (next_tile != NULL) next_tile->collectTiles(tiles, loop_translation_map);
  if (next_node != NULL) next_node->collectTiles(tiles, loop_translation_map);
}

void stmt_t::collectLoops(std::vector<Descriptor::loop_t *> & loops, std::map<const loop_t *, Descriptor::loop_t *> & loop_translation_map) const {}
void stmt_t::collectTiles(std::vector<Descriptor::tile_t *> & tiles, const std::map<const loop_t *, Descriptor::loop_t *> & loop_translation_map) const {}

node_t * block_t::finalize() {
  std::cerr << "[Info] (KLT::LoopTree::block_t::finalize)" << std::endl;
  if (children.size() == 1) {
    node_t * res = children[0]->finalize();
    res->parent = parent;
    delete this;
    return res;
  }
  else {
    std::vector<node_t *>::iterator it;
    for (it = children.begin(); it != children.end(); it++)
      *it = (*it)->finalize();
    return this;
  }
}

node_t * cond_t::finalize() {
  std::cerr << "[Info] (KLT::LoopTree::cond_t::finalize)" << std::endl;
  branch_true  = branch_true->finalize();
  branch_false = branch_false->finalize();
  return this;
}

node_t * loop_t::finalize() {
  std::cerr << "[Info] (KLT::LoopTree::loop_t::finalize) Loop #" << id << std::endl;
  body = body->finalize();
  return this;
}

node_t * tile_t::finalize() {
  std::cerr << "[Info] (KLT::LoopTree::tile_t::finalize) Loop #" << loop->id << ", Tile #" << tile_id << std::endl;

  if (next_node != NULL) { // it is the last tile in a chain
    assert(next_tile == NULL);

    node_t * node = next_node->finalize();  // recursive call on the next node

    if (node->kind == e_tile) { // 'next_node' was a block with only one children which is a tile
      next_tile = (tile_t *)node;
      next_node = NULL;
    }
    else {
      next_tile = NULL;
      next_node = node;
    }
  }
  else if (next_tile != NULL) { // in a chain of tiles
    assert(next_node == NULL);
    node_t * node = next_tile->finalize();
    assert(node->kind == e_tile);
    next_tile = (tile_t *)node;
  }
  else assert(false);

  if (next_tile != NULL) { // in a chain of tiles and 'next_tile' should be the first of a **ordered-linked-list** of tile_t
    tile_t * curr = this;
    tile_t * prev = NULL;
    do {
      prev = curr; curr = prev->next_tile;
      if (curr == NULL) break;

      if (curr->order == order) {
        std::cerr << "[Warning] (KLT::LoopTree::tile_t::finalize) Tile #" << tile_id << " of loop #" << loop->id << " and tile #" << curr->tile_id << " of loop #" << curr->loop->id << " have the same ordering index = " << order << "." << std::endl;
        std::cerr << "[Warning] (KLT::LoopTree::tile_t::finalize) Resulting tile order is undefined." << std::endl; // should be increasing order of the pair loop ID and tile ID. See sort
      }
    } while (order > curr->order);
    if (curr == next_tile) {
      // insert first (nothing to do)
    }
    else if (curr != NULL) {
      // insert between 'prev' and 'curr'

      tile_t * res = next_tile;
        res->parent = parent;

      prev->next_tile = this;
      parent = prev;
      next_tile = curr;
      curr->parent = this;

      return res;
    }
    else {
      // insert last (after 'prev')

      tile_t * res = next_tile;
        res->parent = parent;

      next_tile = NULL;
      next_node = prev->next_node;
      parent = prev;
      
      prev->next_tile = this;
      prev->next_node = NULL;

      next_node->parent = this;
      
      return res;
    }
    
  }

  return this;
}

node_t * stmt_t::finalize() { return this; }

}

}

