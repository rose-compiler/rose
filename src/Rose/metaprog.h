#ifndef Rose_metaprog_h
#define Rose_metaprog_h

namespace Rose {

//! Base algorithms for meta-programming
namespace metaprog {

namespace details {

template <typename L, typename R, bool = L::value>
struct and_t {
  static constexpr bool value = false;
};

template <typename L, typename R>
struct and_t<L,R,true> {
  static constexpr bool value = R::value;
};

template <typename L>
struct and_t<L,void,true> {
  static constexpr bool value = true;
};

template <typename L, typename R, bool = L::value>
struct or_t {
  static constexpr bool value = true;
};

template <typename L, typename R>
struct or_t<L,R,false> {
  static constexpr bool value = R::value;
};

template <typename L>
struct or_t<L,void,false> {
  static constexpr bool value = false;
};

}

template <typename L, typename R> struct and_t : details::and_t<L,R> {};
template <typename L, typename R> struct or_t : details::or_t<L,R> {};

/**
 * A template list implementing a `apply` pattern (functor return void but does not have to be pure)
 *
 * \tparam Elements variadic to capture the list of type
 *
 */
template <typename... Elements>
struct List;

template <typename T>
struct is_list_t {
  static constexpr bool value = false;
};

template <typename... Ts>
struct is_list_t<List<Ts...>> {
  static constexpr bool value = true;
};

template <typename T> using enable_list = typename std::enable_if<is_list_t<T>::value>::type;
template <typename T> using disable_list = typename std::enable_if<!is_list_t<T>::value>::type;

//! helper base class for metaprogramming list
template <typename... Elements>
struct list_base_t {
  template <typename L> using append_ravel  = typename L::template prepend<Elements...>;
  template <typename L> using prepend_ravel = typename L::template append<Elements...>;

  template <typename... Es> using append  = List<Elements..., Es...>;
  template <typename... Es> using prepend = List<Es..., Elements...>;
};

template <typename... Elements>
struct List : list_base_t<Elements...> {
  /**
   * Apply the given instance of Functor to each element of the list (in order)
   *
   * \tparam Functor Do not use! deducted from first argument
   * \tparam Args Do not use! forwarded types of functor argments
   *
   * \param functor an instance of Functor 
   * \param args forwarded values of functor argments
   */
  template <typename Functor, typename... Args>
  static inline void apply(Functor &&, Args&&...) {}

  /**
   * Map Functor to each element of the list and return a list. Functor return a list element.
   *
   * \tparam Functor a template of one type (list element) assign the result to `type`
   */
  template <template <typename> class Functor>
  using map_t = List<>;

  /**
   * Reduce ...
   *
   * \tparam Reducer
   */
  template <template <typename,typename> class Reducer>
  using reduce_t = void;

  /**
   * Map Functor to each element of the list and return a list. Functor return a list.
   *
   * \tparam Functor a template of one type (list element) assign the result to `list`
   */
  template <template <typename> class Functor>
  using map_ravel_t = List<>;

  /**
   * Return the list of element for which Pred<E>::value == true
   *
   * \tparam Predicate a template of one type (list element) assign the result to `value`
   */
  template <template <typename> class Predicate>
  using filter = List<>;

  //! Length of the list
  static constexpr size_t length{0};

  /**
   * Return the element at the given position (void if out-of-bound)
   *
   * \tparam position of the element to access
   */
  template <size_t position>
  using at = void;
};

namespace details {
  template <size_t position, typename Element, typename... Elements>
  struct list_at_t {
    using type = typename List<Elements...>::template at<position-1>;
  };

  template <typename Element, typename... Elements>
  struct list_at_t<0, Element, Elements...> {
    using type = Element;
  };

  template <template <typename> class Predicate, bool, typename Element, typename... Elements>
  struct list_filter_t {
    using list = typename List<Elements...>::template filter<Predicate>;
  };

  template <template <typename> class Predicate, typename Element, typename... Elements>
  struct list_filter_t<Predicate, true, Element, Elements...> {
    using list = typename List<Elements...>::template filter<Predicate>::template prepend<Element>;
  };
}

template <typename Element, typename... Elements>
struct List<Element, Elements...> : list_base_t<Element, Elements...> {
  template <typename Functor, typename... Args>
  static inline void apply(Functor && functor, Args&&... args) {
    functor.template operator()<Element>(std::forward<Args>(args)...);
    List<Elements...>::template apply<Functor>(std::forward<Functor>(functor), std::forward<Args>(args)...);
  }

  template <template <typename> class Functor>
  using map_t = typename List<Elements...>::template map_t<Functor>::template prepend< typename Functor<Element>::type >;

  template <template <typename, typename> class Reducer>
  using reduce_t = Reducer< Element, typename List<Elements...>::template reduce_t<Reducer> >;

  template <template <typename> class Functor>
  using map_ravel_t = typename List<Elements...>::template map_ravel_t<Functor>::template prepend_ravel<typename Functor<Element>::list>;

  template <template <typename> class Predicate>
  using filter = typename details::list_filter_t<Predicate, Predicate<Element>::value, Element, Elements...>::list;

  static constexpr size_t length{ List<Elements...>::length + 1 };

  template <size_t position>
  using at = typename details::list_at_t<position, Element, Elements...>::type;
};

/**
 * Build traversals for meta-tree
 *
 * \tparam Predicate
 * \tparam Extract
 * \tparam Children
 *
 */
template <template<typename> class Predicate, template<typename> class Extract, template<typename> class Children>
struct tree_traversal_factory_t {
  
  /**
   * Traverse a tree (pre-order, depth-first) and build a list of values
   *
   * \tparam RootTag
   */
  template <typename RootTag>
  struct preorder_depthfirst_t {
    //! pseudo: head = Predicate<RootTag> ? List<Extract<RootTag>> : List<>;
    using head = std::conditional_t<Predicate<RootTag>::value, List<typename Extract<RootTag>::type>, List<> >;
    //! pseudo: for (ChildTag: Children<RootTag>) tail += preorder_depthfirst_t<ChildTag>;
    using tail = typename Children<RootTag>::list::template map_ravel_t<preorder_depthfirst_t>;
    //! pseudo: return head+tail;
    using list = typename head::template append_ravel<tail>;
  };
};

namespace details {

  template<typename... Ts> struct make_void { typedef void type;};
  template<typename... Ts> using void_t = typename make_void<Ts...>::type;

  template <typename T, typename = void>
  struct is_iterable_t : std::false_type {};

  template <typename T>
  struct is_iterable_t<T, void_t<
                            decltype(std::begin(std::declval<T>())),
                            decltype(std::end(std::declval<T>())) //! \todo check for value_type! 
                         > > : std::true_type {};
}

//! Detect STL-like containers
template <class C> constexpr bool is_iterable = details::is_iterable_t<C>::value;

}

namespace mp = metaprog;

}

#endif /* Rose_metaprog_h */
