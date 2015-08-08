
#ifndef __KLT_LOOPTREE_HPP__
#define __KLT_LOOPTREE_HPP__

#include <vector>
#include <map>
#include <set>
#include <iostream>
#include <string>

class SgNode;
class SgProject;
class SgExpression;
class SgStatement;
class SgForStatement;
class SgVariableSymbol;

namespace KLT {

namespace Descriptor {
  struct loop_t;
  struct tile_t;
}

namespace LoopTree {

struct loop_t;

enum kind_e {
  e_block,
  e_cond,
  e_loop,
  e_tile,
  e_stmt,
  e_ignored,
  e_unknown
};

class extraction_context_t {
  public:
    typedef ::KLT::LoopTree::loop_t loop_t;
    typedef SgVariableSymbol vsym_t;
    typedef std::vector<vsym_t *> vsym_list_t;
    typedef std::set<vsym_t *> vsym_set_t;
    typedef std::map<SgForStatement *, size_t> loop_map_t;

  private:
    loop_map_t & loop_map;
    const vsym_set_t & data;
    size_t loop_cnt;

  private:
    vsym_set_t parameters;
    vsym_set_t iterators;
    vsym_set_t locals;

  public:
    extraction_context_t(loop_map_t & loop_map_, const vsym_set_t & data_);

    void addParameter(vsym_t * vsym);
    void addIterator (vsym_t * vsym);
    void addLocal    (vsym_t * vsym);

    void processVarRefs(SgNode * node);

    size_t nextLoopID();
    void registerLoop(SgForStatement *, loop_t *);

    const vsym_set_t & getParameters() const;
};

struct loop_t;
struct tile_t;

struct node_t {
  kind_e kind;
  node_t * parent;

  node_t(kind_e kind_);
  virtual ~node_t();

  static node_t * extract(SgStatement * stmt, extraction_context_t & ctx);

  virtual node_t * finalize() = 0;

  std::string getGraphVizLabel() const;
  virtual void toGraphViz(std::ostream & out, std::string indent) const = 0;

  virtual void collectLoops(std::vector<Descriptor::loop_t *> & loops, std::map<const loop_t *, Descriptor::loop_t *> & loop_translation_map) const = 0;
  virtual void collectTiles(std::vector<Descriptor::tile_t *> & tiles, const std::map<const loop_t *, Descriptor::loop_t *> & loop_translation_map) const = 0;
};

struct block_t : public node_t {
  std::vector<node_t *> children;

  block_t();
  virtual ~block_t();

  static block_t * extract(SgStatement * stmt, extraction_context_t & ctx);

  virtual node_t * finalize();

  virtual void toGraphViz(std::ostream & out, std::string indent) const;

  virtual void collectLoops(std::vector<Descriptor::loop_t *> & loops, std::map<const loop_t *, Descriptor::loop_t *> & loop_translation_map) const;
  virtual void collectTiles(std::vector<Descriptor::tile_t *> & tiles, const std::map<const loop_t *, Descriptor::loop_t *> & loop_translation_map) const;
};

struct cond_t : public node_t {
  SgExpression * condition;

  node_t * branch_true;
  node_t * branch_false;

  cond_t(SgExpression * cond = NULL);
  virtual ~cond_t();

  static cond_t * extract(SgStatement * stmt, extraction_context_t & ctx);

  virtual node_t * finalize();

  virtual void toGraphViz(std::ostream & out, std::string indent) const;

  virtual void collectLoops(std::vector<Descriptor::loop_t *> & loops, std::map<const loop_t *, Descriptor::loop_t *> & loop_translation_map) const;
  virtual void collectTiles(std::vector<Descriptor::tile_t *> & tiles, const std::map<const loop_t *, Descriptor::loop_t *> & loop_translation_map) const;
};

struct loop_t : public node_t {
  size_t id;

  // for ('iterator' = 'lower_bound'; 'iterator' <= 'upper_bound'; 'iterator' += 'stride')
  SgVariableSymbol * iterator;
  SgExpression * lower_bound;
  SgExpression * upper_bound;
  SgExpression * stride;

  node_t * body;

  loop_t(
    size_t id_, SgVariableSymbol * it = NULL,
    SgExpression * lb = NULL, SgExpression * ub = NULL, SgExpression * stride_ = NULL
  );
  virtual ~loop_t();

  static loop_t * extract(SgStatement * stmt, extraction_context_t & ctx);

  virtual node_t * finalize();

  virtual void toGraphViz(std::ostream & out, std::string indent) const;

  virtual void collectLoops(std::vector<Descriptor::loop_t *> & loops, std::map<const loop_t *, Descriptor::loop_t *> & loop_translation_map) const;
  virtual void collectTiles(std::vector<Descriptor::tile_t *> & tiles, const std::map<const loop_t *, Descriptor::loop_t *> & loop_translation_map) const;
};

struct stmt_t : public node_t {
  SgStatement * statement;

  stmt_t(SgStatement * stmt = NULL);
  virtual ~stmt_t();

  static stmt_t * extract(SgStatement * stmt, extraction_context_t & ctx);

  virtual node_t * finalize();

  virtual void toGraphViz(std::ostream & out, std::string indent) const;

  virtual void collectLoops(std::vector<Descriptor::loop_t *> & loops, std::map<const loop_t *, Descriptor::loop_t *> & loop_translation_map) const;
  virtual void collectTiles(std::vector<Descriptor::tile_t *> & tiles, const std::map<const loop_t *, Descriptor::loop_t *> & loop_translation_map) const;
};

struct tile_t : public node_t {
  size_t id;

  unsigned long kind;
  size_t order;
  SgExpression * param;

  loop_t * loop;
  size_t tile_id; // ID in the loop

  tile_t * next_tile;
  node_t * next_node;

  tile_t(size_t id_, unsigned long kind_, size_t order_, SgExpression * param_, loop_t * loop_, size_t tile_id_);
  virtual ~tile_t();

  virtual node_t * finalize();

  virtual void toGraphViz(std::ostream & out, std::string indent) const;

  virtual void collectLoops(std::vector<Descriptor::loop_t *> & loops, std::map<const loop_t *, Descriptor::loop_t *> & loop_translation_map) const;
  virtual void collectTiles(std::vector<Descriptor::tile_t *> & tiles, const std::map<const loop_t *, Descriptor::loop_t *> & loop_translation_map) const;
};

}

}

#endif /* __KLT_LOOPTREE_HPP__ */

