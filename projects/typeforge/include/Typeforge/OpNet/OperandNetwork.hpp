
#ifndef TYPEFORGE_OPERAND_NETWORK_H
#  define TYPEFORGE_OPERAND_NETWORK_H

#include "Typeforge/OpNet/commons.hpp"

#include <string>
#include <ostream>
#include <map>
#include <set>

namespace Typeforge {

template <OperandKind opkind>
struct OperandData;

typedef std::map<lnode_ptr, EdgeKind> edges_map_t;
typedef std::map<lnode_ptr, std::map<lnode_ptr, EdgeKind> > edges_map_map_t;

class OperandNetwork {
  private:
    std::map<lnode_ptr const, OperandData<OperandKind::base> *> node_to_data;
    std::map<std::string const, OperandData<OperandKind::base> *> handle_to_data;

    std::map<OperandKind const, std::set<lnode_ptr> > kind_to_nodes{
      { OperandKind::variable          , {} },
      { OperandKind::function          , {} },
      { OperandKind::parameter         , {} },
      { OperandKind::varref            , {} },
      { OperandKind::fref              , {} },
      { OperandKind::thisref           , {} },
      { OperandKind::value             , {} },
      { OperandKind::assign            , {} },
      { OperandKind::unary_arithmetic  , {} },
      { OperandKind::binary_arithmetic , {} },
      { OperandKind::call              , {} },
      { OperandKind::array_access      , {} },
      { OperandKind::address_of        , {} },
      { OperandKind::dereference       , {} },
      { OperandKind::member_access     , {} },
    };

    edges_map_map_t predeccessors;
    edges_map_map_t successors;

  private:
    OperandData<OperandKind::base> * addNode(lnode_ptr node);
    void addEdge(lnode_ptr const source, lnode_ptr const target, EdgeKind const ek);

    void rmNode(lnode_ptr const node);
    void rmEdge(lnode_ptr const source, lnode_ptr const target);

    void updateLabels(lnode_ptr pred_node, lnode_ptr old_node, lnode_ptr new_node);

    template <OperandKind ok>
    std::set<OperandData<ok> *> collectNodes() const;

    template <typename CollapseRule>
    void collapsePredeccessors(OperandData<CollapseRule::opkind> * n, lnode_ptr pred_rnode);

    template <typename CollapseRule>
    void collapseSuccessors(OperandData<CollapseRule::opkind> * n, lnode_ptr succ_rnode);

  public:
    void initialize(SgProject * p = nullptr);

    template <
      typename CollapseRule,
      typename std::enable_if< CollapseRule::pattern == CollapseRules::Pattern::unary_operator >::type * = nullptr
    >
    void collapse();

    template <
      typename CollapseRule,
      typename std::enable_if< CollapseRule::pattern == CollapseRules::Pattern::binary_operator >::type * = nullptr
    >
    void collapse();

    template <
      typename CollapseRule,
      typename std::enable_if< CollapseRule::pattern == CollapseRules::Pattern::reference >::type * = nullptr
    >
    void collapse();

    template <
      typename CollapseRule,
      typename std::enable_if< CollapseRule::pattern == CollapseRules::Pattern::call_operator >::type * = nullptr
    >
    void collapse();

  public:
    std::string     const getHandle ( lnode_ptr const) const;
    lnode_ptr const getNode   ( std::string const & ) const;

    SgType * const getOriginalType ( lnode_ptr const ) const;
    SgType * const getOriginalType ( std::string   const & ) const;

    std::string const getFilename ( lnode_ptr const ) const;
    std::string const getFilename ( std::string   const & ) const;

    size_t const getLineStart ( lnode_ptr const ) const;
    size_t const getLineStart ( std::string   const & ) const;

    size_t const getLineEnd ( lnode_ptr const ) const;
    size_t const getLineEnd ( std::string   const & ) const;

    size_t const getColumnStart ( lnode_ptr const ) const;
    size_t const getColumnStart ( std::string   const & ) const;

    size_t const getColumnEnd ( lnode_ptr const ) const;
    size_t const getColumnEnd ( std::string   const & ) const;

  public:
    std::set<lnode_ptr> const & getAll(const OperandKind opkind) const;

  public:
    void toDot(std::string const & fname) const;
    void toDot(std::ostream & out) const;
};

extern OperandNetwork opnet;

}

///////////////////////////////////
// Start of Template Definitions //
///////////////////////////////////

#include <iostream>
#include <sstream>
#include <cassert>

namespace Typeforge {

template <OperandKind ok>
std::set<OperandData<ok> *> OperandNetwork::collectNodes() const {
  std::set<OperandData<ok> *> res;

  for (auto n : getAll(ok)) {
    auto it = node_to_data.find(n);

    assert(it != node_to_data.end());
    assert(it->second != nullptr);
    assert(it->second->opkind == ok);

    res.insert((OperandData<ok> *)it->second);
  }

  return res;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Predeccessors collapsing

template <typename CollapseRule>
void OperandNetwork::collapsePredeccessors(OperandData<CollapseRule::opkind> * n, lnode_ptr pred_rnode) {
  for (auto pred : edges_map_t(predeccessors[n->lnode])) {
    assert(pred.first != nullptr);

    rmEdge(pred.first, n->lnode);

    if (pred_rnode != nullptr && pred.first != pred_rnode) {
      auto action = CollapseRule::predeccessor(pred.second);
      if (action.first != EdgeKind::unknown) {
        if (action.second) {
          addEdge(pred.first, pred_rnode, action.first);
        } else {
          addEdge(pred_rnode, pred.first, action.first);
        }
      }
    }

    updateLabels(pred.first, n->lnode, pred_rnode);
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Successors collapsing

template <typename CollapseRule>
void OperandNetwork::collapseSuccessors(OperandData<CollapseRule::opkind> * n, lnode_ptr succ_rnode) {
  for (auto succ : edges_map_t(successors[n->lnode])) {
    assert(succ.first != nullptr);

    rmEdge(n->lnode, succ.first);

    if (succ_rnode != nullptr && succ.first != succ_rnode) {
      auto action = CollapseRule::successor(succ.second);
      if (action.first != EdgeKind::unknown) {
        if (action.second) {
          addEdge(succ_rnode, succ.first, action.first);
        } else {
          addEdge(succ.first, succ_rnode, action.first);
        }
      }
    }
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Collapse operation for unary operators

template <
  typename CollapseRule,
  typename std::enable_if< CollapseRule::pattern == CollapseRules::Pattern::unary_operator >::type * = nullptr
>
void OperandNetwork::collapse() {
  std::set<lnode_ptr> to_be_erased;
  for (auto n : collectNodes<CollapseRule::opkind>()) {
    lnode_ptr operand = n->edge_labels[CollapseRule::operand];
    assert(operand != nullptr);

    rmEdge(n->lnode, operand);

    collapsePredeccessors<CollapseRule>(n, CollapseRule::predeccessor(operand));
    collapseSuccessors<CollapseRule>(n, CollapseRule::successor(operand));

    to_be_erased.insert(n->lnode);
  }
  for (auto n : to_be_erased) {
    rmNode(n);
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Collapse operation for binary operators

template <
  typename CollapseRule,
  typename std::enable_if< CollapseRule::pattern == CollapseRules::Pattern::binary_operator >::type * = nullptr
>
void OperandNetwork::collapse() {
  std::set<lnode_ptr> to_be_erased;
  for (auto n : collectNodes<CollapseRule::opkind>()) {
    lnode_ptr lhs = n->edge_labels[CollapseRule::lhs];
    assert(lhs != nullptr);

    lnode_ptr rhs = n->edge_labels[CollapseRule::rhs];
    assert(rhs != nullptr);

    rmEdge(n->lnode, lhs);
    rmEdge(n->lnode, rhs);

    auto link = CollapseRule::link();
    if (link.first != EdgeKind::unknown) {
      if (link.second) {
        addEdge(lhs, rhs, link.first);
      } else {
        addEdge(rhs, lhs, link.first);
      }
    }

    collapsePredeccessors<CollapseRule>(n, CollapseRule::predeccessor(lhs, rhs));
    collapseSuccessors<CollapseRule>(n, CollapseRule::successor(lhs, rhs));

    to_be_erased.insert(n->lnode);
  }
  for (auto n : to_be_erased) {
    rmNode(n);
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Collapse operation for reference operators ( VarRef / FunctionRef : has `decl` field )

template <
  typename CollapseRule,
  typename std::enable_if< CollapseRule::pattern == CollapseRules::Pattern::reference >::type * = nullptr
>
void OperandNetwork::collapse() {
  std::set<lnode_ptr> to_be_erased;
  for (auto n : collectNodes<CollapseRule::opkind>()) {
    lnode_ptr decl = n->edge_labels["decl"];
    if (decl != nullptr) {
      rmEdge(n->lnode, decl);
    }

    collapsePredeccessors<CollapseRule>(n, CollapseRule::predeccessor(decl));
    collapseSuccessors<CollapseRule>(n, CollapseRule::successor(decl));

    to_be_erased.insert(n->lnode);
  }
  for (auto n : to_be_erased) {
    rmNode(n);
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Collapse operation for call operators ( CallExp : has `fnc` field )

template <
  typename CollapseRule,
  typename std::enable_if< CollapseRule::pattern == CollapseRules::Pattern::call_operator >::type * = nullptr
>
void OperandNetwork::collapse() {
  std::set<lnode_ptr> to_be_erased;
  for (auto n : collectNodes<CollapseRule::opkind>()) {
    lnode_ptr fnc = n->edge_labels["fnc"];
    assert(fnc != nullptr);

    auto fnc_handle = getHandle(fnc);
    fnc_handle = fnc_handle.substr(9, fnc_handle.size() - 10);

    std::set<lnode_ptr> rm_edges;
    std::set<std::string> rm_labels;
    for (auto p : n->edge_labels) {
      if (p.first.find("args[") == 0) {
        auto pos__ = p.first.substr(5, p.first.size() - 6);
        size_t pos = std::stoul(pos__);
        std::ostringstream oss;
        oss << "PARAMETER[" << fnc_handle << "::" << pos << "]";
        auto param = getNode(oss.str());

        rm_edges.insert(p.second);
        rm_labels.insert(p.first);
        addEdge(param, p.second, EdgeKind::value);
      }
    }

//  rm_labels.insert("fnc");
    rm_edges.insert(fnc);

    for (auto n_ : rm_edges) {
      rmEdge(n->lnode, n_);
    }
    for (auto l : rm_labels) {
      n->edge_labels.erase(l);
    }

    if (::Typeforge::is_template_with_dependent_return_type(fnc)) {
      // TODO ?
    } else {
      collapsePredeccessors<CollapseRule>(n, CollapseRule::predeccessor(fnc));
      collapseSuccessors<CollapseRule>(n, CollapseRule::successor(fnc));

      to_be_erased.insert(n->lnode);
    }
  }
  for (auto n : to_be_erased) {
    rmNode(n);
  }
}

}

#endif /* TYPEFORGE_OPERAND_NETWORK_H */

