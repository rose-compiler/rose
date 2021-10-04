
template <bool c, typename T = void>
struct enableIf {};

template <typename T>
struct enableIf<true, T> {
  using type = T;
};

template <typename BaseT, long lbT, long ubT>
struct static_range {
  static constexpr bool is_static = true;
};

template <typename BaseT>
struct dynamic_range {
  static constexpr bool is_static = false;

  template <typename RangeT, typename enableIf<RangeT::is_static>::type * = nullptr >
  constexpr dynamic_range(RangeT &) {}
};

template <typename BaseT, long... boundT>
struct Range;

template <typename BaseT>
struct Range<BaseT> : public dynamic_range<BaseT> {
  constexpr Range(BaseT const lb_, BaseT const ub_) : dynamic_range<BaseT>(lb_,ub_) {}
  template <typename SrangeT>
  constexpr Range(SrangeT & r) : dynamic_range<BaseT>(r) {}
};

template <typename BaseT, long lbT, long ubT>
struct Range<BaseT, lbT, ubT> : public static_range<BaseT, lbT, ubT> {};

template <typename... Ranges>
struct Domain;

template <>
struct Domain<> {
  Domain() {}
};

template <typename RangeT, typename... Ranges>
struct Domain<RangeT, Ranges...> {
  using Range = RangeT;
  using Next = Domain<Ranges...>;

  Range range;
  Next next;

  template <typename RangeT_, typename... Ranges_>
  Domain(RangeT_ range_, Ranges_... ranges) :
    range(range_),
    next(ranges...)
  {}
};

template <typename BaseT, typename DomainT >
struct Array {
  using Base = BaseT;
  using Domain = DomainT;

  Domain domain;

  template <typename... RangeT>
  Array(RangeT... ranges) :
    domain(ranges...)
  {}
};

void foo(Array<double, Range<int> > const &);

typedef Range<int,1,10> Index_Range_1;

void bar() {
  Array<double, Range<int> > arr_1((Index_Range_1()));
  foo(arr_1);
}

