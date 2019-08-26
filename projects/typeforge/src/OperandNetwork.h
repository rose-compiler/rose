
#ifndef TYPEFORGE_OPERAND_NETWORK_H
#  define TYPEFORGE_OPERAND_NETWORK_H

#include <string>
#include <ostream>
#include <map>
#include <set>

class SgLocatedNode;
class SgProject;
class SgType;

namespace Typeforge {

typedef SgLocatedNode * lnode_ptr;
  
enum class OperandKind {
  base,

// declarations
  variable,
  function,
  parameter,

// references & values
  varref,
  fref,
  thisref,
  value,

// operations:
  assign,
  unary_arithmetic,
  binary_arithmetic,
  call,
  array_access,
  address_of,
  dereference,
  member_access,

  unknown
};
  
enum class EdgeKind {
  value,
  deref,
  address,
  traversal,
  unknown
};

EdgeKind operator| (const EdgeKind & a, const EdgeKind & b);
EdgeKind operator& (const EdgeKind & a, const EdgeKind & b);

namespace CollapseRules {

enum class Pattern {
  binary_operator,
  unary_operator,
  call_operator,
  reference,
  unknown
};

}

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

namespace CollapseRules {

struct AssignOperator {
  static constexpr OperandKind opkind = OperandKind::assign;
  static constexpr Pattern pattern = Pattern::binary_operator;
  static const std::string lhs;
  static const std::string rhs;

  static std::pair<EdgeKind,bool> link() { return std::pair<EdgeKind,bool>(EdgeKind::value, true); }

  static lnode_ptr predeccessor(lnode_ptr lhs, lnode_ptr rhs) { return lhs; }
  static lnode_ptr successor(lnode_ptr lhs, lnode_ptr rhs) { return lhs; }

  static std::pair<EdgeKind,bool> predeccessor(EdgeKind ekind) { return std::pair<EdgeKind,bool>(ekind | EdgeKind::value, true); }
  static std::pair<EdgeKind,bool> successor(EdgeKind ekind) { return std::pair<EdgeKind,bool>(EdgeKind::value | ekind, true); }
};

struct MemberAccess {
  static constexpr OperandKind opkind = OperandKind::member_access;
  static constexpr Pattern pattern = Pattern::binary_operator;
  static const std::string lhs;
  static const std::string rhs;

  static std::pair<EdgeKind,bool> link() { return std::pair<EdgeKind,bool>(EdgeKind::unknown, false); }

  static lnode_ptr predeccessor(lnode_ptr lhs, lnode_ptr rhs) { return rhs; }
  static lnode_ptr successor(lnode_ptr lhs, lnode_ptr rhs) { return rhs; }

  static std::pair<EdgeKind,bool> predeccessor(EdgeKind ekind) { return std::pair<EdgeKind,bool>(ekind | EdgeKind::value, true); }
  static std::pair<EdgeKind,bool> successor(EdgeKind ekind) { return std::pair<EdgeKind,bool>(EdgeKind::value | ekind, true); }
};

struct ArrayAccess {
  static constexpr OperandKind opkind = OperandKind::array_access;
  static constexpr Pattern pattern = Pattern::binary_operator;
  static const std::string lhs;
  static const std::string rhs;

  static std::pair<EdgeKind,bool> link() { return std::pair<EdgeKind,bool>(EdgeKind::unknown, false); }

  static lnode_ptr predeccessor(lnode_ptr lhs, lnode_ptr rhs) { return lhs; }
  static lnode_ptr successor(lnode_ptr lhs, lnode_ptr rhs) { return lhs; }

  static std::pair<EdgeKind,bool> predeccessor(EdgeKind ekind) { return std::pair<EdgeKind,bool>(ekind | EdgeKind::deref, true); }
  static std::pair<EdgeKind,bool> successor(EdgeKind ekind) { return std::pair<EdgeKind,bool>(EdgeKind::deref | ekind, true); }
};

struct Dereference {
  static constexpr OperandKind opkind = OperandKind::dereference;
  static constexpr Pattern pattern = Pattern::unary_operator;
  static const std::string operand;

  static lnode_ptr predeccessor(lnode_ptr operand) { return operand; }
  static lnode_ptr successor(lnode_ptr operand) { return operand; }

  static std::pair<EdgeKind,bool> predeccessor(EdgeKind ekind) { return std::pair<EdgeKind,bool>(ekind | EdgeKind::deref, true); }
  static std::pair<EdgeKind,bool> successor(EdgeKind ekind) { return std::pair<EdgeKind,bool>(EdgeKind::deref | ekind, true); }
};

struct AddressOf {
  static constexpr OperandKind opkind = OperandKind::address_of;
  static constexpr Pattern pattern = Pattern::unary_operator;
  static const std::string operand;

  static lnode_ptr predeccessor(lnode_ptr operand) { return operand; }
  static lnode_ptr successor(lnode_ptr operand) { return operand; }

  static std::pair<EdgeKind,bool> predeccessor(EdgeKind ekind) { return std::pair<EdgeKind,bool>(ekind | EdgeKind::address, true); }
  static std::pair<EdgeKind,bool> successor(EdgeKind ekind) { return std::pair<EdgeKind,bool>(EdgeKind::address | ekind, true); }
};

struct VarRef {
  static constexpr OperandKind opkind = OperandKind::varref;
  static constexpr Pattern pattern = Pattern::reference;

  static lnode_ptr predeccessor(lnode_ptr decl) { return decl; }
  static lnode_ptr successor(lnode_ptr decl) { return decl; }

  static std::pair<EdgeKind,bool> predeccessor(EdgeKind ekind) { return std::pair<EdgeKind,bool>(ekind | EdgeKind::value, true); }
  static std::pair<EdgeKind,bool> successor(EdgeKind ekind) { return std::pair<EdgeKind,bool>(EdgeKind::value | ekind, true); }
};

struct FRef {
  static constexpr OperandKind opkind = OperandKind::fref;
  static constexpr Pattern pattern = Pattern::reference;

  static lnode_ptr predeccessor(lnode_ptr decl) { return decl; }
  static lnode_ptr successor(lnode_ptr decl) { return decl; }

  static std::pair<EdgeKind,bool> predeccessor(EdgeKind ekind) { return std::pair<EdgeKind,bool>(ekind | EdgeKind::value, true); }
  static std::pair<EdgeKind,bool> successor(EdgeKind ekind) { return std::pair<EdgeKind,bool>(EdgeKind::value | ekind, true); }
};

struct ThisRef {
  static constexpr OperandKind opkind = OperandKind::thisref;
  static constexpr Pattern pattern = Pattern::reference;

  static lnode_ptr predeccessor(lnode_ptr decl) { return nullptr; }
  static lnode_ptr successor(lnode_ptr decl) { return nullptr; }

  static std::pair<EdgeKind,bool> predeccessor(EdgeKind ekind) { return std::pair<EdgeKind,bool>(EdgeKind::unknown, true); }
  static std::pair<EdgeKind,bool> successor(EdgeKind ekind) { return std::pair<EdgeKind,bool>(EdgeKind::unknown, true); }
};

struct Call {
  static constexpr OperandKind opkind = OperandKind::call;
  static constexpr Pattern pattern = Pattern::call_operator;

  static std::pair<EdgeKind,bool> link() { return std::pair<EdgeKind,bool>(EdgeKind::unknown, false); }

  static lnode_ptr predeccessor(lnode_ptr fnc) { return fnc; }
  static lnode_ptr successor(lnode_ptr fnc) { return fnc; }

  static std::pair<EdgeKind,bool> predeccessor(EdgeKind ekind) { return std::pair<EdgeKind,bool>(ekind | EdgeKind::value, true); }
  static std::pair<EdgeKind,bool> successor(EdgeKind ekind) { return std::pair<EdgeKind,bool>(EdgeKind::value | ekind, true); }
};

}

}

///////////////////////////////////
// Start of Template Definitions //
///////////////////////////////////

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

bool is_template_with_dependent_return_type(lnode_ptr fnc);

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

    if (is_template_with_dependent_return_type(fnc)) {
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

