#ifndef ROSE_Traits_Traversal_H
#define ROSE_Traits_Traversal_H

#include <Rose/Traits/describe/fields.h>
#include <Rose/Traits/grammar/lists.h>
#include <Rose/Traits/grammar/predicates.h>

namespace Rose {
namespace Traits {

#if 0
auto dispatch = []<typename OrigNodeT>(OrigNodeT * node, auto & F = details::nop_f) {
  using nl = Nodes; // mp::List<SgExpression>; // TODO list_concrete_subclasses<OrigNodeT>
  if (node) {
    auto const v = node->variantT(); // TODO better? variantFromMempool is slow but maybe a version using `nl` above
    nl::apply([&]<typename ConcreteNodeT>() {
      if (v == Describe<ConcreteNodeT>::variant) {
        F((ConcreteNodeT*)node);
      }
    });
  }
};
#endif

namespace traversal_details {
  //! Used for the call to pre/post functor at the root of the traversal
  struct empty_field_t {
    using parent = void;
    static constexpr size_t position{0};
    size_t index{0};
  };

  /**
   * No Operation lambda and type
   */
  auto nop_noargs_f = []<typename NodeT, typename FieldT>(NodeT * , FieldT) { /* NOP */ };

//  auto nop_pre_f = []<typename SynthT, typename NodeT, typename FieldT, typename InhT>(NodeT * node, FieldT pfld, InhT & inh) { return InhT{}; };
//  auto nop_post_f = []<typename SynthT, typename NodeT, typename FieldT, typename InhT>(NodeT * node, FieldT pfld, InhT const & inh, SynthContT const & synths) { return SynthT{}; };

  template <typename OrigNodeT, typename PreFuncT, typename PostFuncT, typename FieldT>
  struct Dispatcher;

  template <typename ParentNodeT, typename PreFuncT, typename PostFuncT>
  struct FieldTreeTraversal {
    template <typename ChildNodeT>
    static constexpr bool is_match = std::is_same<ParentNodeT, ChildNodeT>::value;

    template <typename F, typename N>
    static constexpr bool do_nothing = !is_match<N> || !F::traverse;

    template <typename F, typename N>
    using enable_nop = std::enable_if_t<do_nothing<F, N>>;

    template <typename F, typename N>
    using enable_call = std::enable_if_t<!do_nothing<F, N> && !F::iterable>;

    template <typename F, typename N>
    using enable_iterate = std::enable_if_t<!do_nothing<F, N> && F::iterable>;

    template <typename FieldT, typename ChildNodeT, enable_nop<FieldT, ChildNodeT> * = nullptr>
    inline void operator() (ChildNodeT *, PreFuncT &, PostFuncT &) const { /* NOP */ }

    template <typename FieldT, typename ChildNodeT, enable_call<FieldT, ChildNodeT> * = nullptr>
    inline void operator() (ChildNodeT * node, PreFuncT & pre, PostFuncT & post) const {
      using node_type = std::remove_pointer_t<typename FieldT::type>;
      using FieldDispatcher = Dispatcher<node_type, PreFuncT, PostFuncT, FieldT>;
      if (node->*(FieldT::mbr_ptr)) {
        FieldDispatcher::dispatch(node->*(FieldT::mbr_ptr), pre, post, FieldT{FieldT::position});
      }
    }

    template <typename FieldT, typename ChildNodeT, enable_iterate<FieldT, ChildNodeT> * = nullptr>
    inline void operator() (ChildNodeT * node, PreFuncT & pre, PostFuncT & post) const {
      using node_type = std::remove_pointer_t<typename FieldT::type>;
      using FieldDispatcher = Dispatcher<node_type, PreFuncT, PostFuncT, FieldT>;

      size_t i = 0;
      for (auto ptr: node->*(FieldT::mbr_ptr)) {
        if (ptr) {
          FieldDispatcher::dispatch(ptr, pre, post, FieldT{i});
        }
        i++;
      }
    }
  };
  
  template <typename FieldT>
  struct is_traversable_t {
    static constexpr bool value = FieldT::traverse;
  };

  template <typename NodeT>
  struct get_traversable_list_t {
    using fields = typename generated::describe_node_t<NodeT>::fields_t;
    using trav_fields = typename fields::template filter<is_traversable_t>;
    using list = typename trav_fields::template map_t<details::transcribe_field_desc_t>;
  };

  template <typename NodeT>
  struct get_inherited_traversable_list_t {
    using parents = list_base_classes<NodeT>;
    using list = typename parents::template map_ravel_t<get_traversable_list_t>;
  };

  template <typename OrigNodeT, typename PreFuncT, typename PostFuncT, typename FieldT>
  struct Dispatcher {
    using subclasses = list_concrete_subclasses<OrigNodeT>;

    static void dispatch(OrigNodeT * node, PreFuncT & pre, PostFuncT & post, FieldT field) {
      if (!node) return;
      auto const v = node->variantT(); // TODO better? variantFromMempool is slow but maybe a version using `subclasses` above
      subclasses::apply([&]<typename ConcreteNodeT>() {
        if (v == generated::describe_node_t<ConcreteNodeT>::variant) {
          using fields = typename get_inherited_traversable_list_t<ConcreteNodeT>::list;

          ConcreteNodeT * cnode = (ConcreteNodeT*)node;
          FieldTreeTraversal<ConcreteNodeT, PreFuncT, PostFuncT> ftt;

          pre.PreFuncT::template operator()(cnode, field);
          fields::apply(ftt, cnode, pre, post);
          post.PostFuncT::template operator()(cnode, field);
        }
      });
    }
  };

}

template < typename NodeT, typename PreFuncT, typename PostFuncT>
void traverse( NodeT * node_, PreFuncT & pre = traversal_details::nop_noargs_f, PostFuncT & post = traversal_details::nop_noargs_f) {
  traversal_details::Dispatcher<NodeT, PreFuncT, PostFuncT, traversal_details::empty_field_t>::dispatch(node_, pre, post, traversal_details::empty_field_t{});
}

} }

#endif
