#ifndef _MEMOIZER_H
#define _MEMOIZER_H

namespace
{
  //
  // functor/function decorator to memoize results


  /// \private
  template <class Fn>
  struct FnTraits : FnTraits<decltype(&Fn::operator())> { /* use overloads */ };

  /// \private
  /// for const member operator() and non-mutable lambda's
  template <class R, class C, class... Args>
  struct FnTraits<R (C::*) (Args...) const>
  {
    typedef std::tuple<Args...> arguments_t;
    typedef R                   result_t;
  };

  /// \private
  /// for non-const member operator() and mutable lambda's
  template <class R, class C, class... Args>
  struct FnTraits<R (C::*) (Args...)>
  {
    typedef std::tuple<Args...> arguments_t;
    typedef R                   result_t;
  };

  /// \private
  /// for freestanding functions
  template <class R, class... Args>
  struct FnTraits<R (*) (Args...)>
  {
    typedef std::tuple<Args...> arguments_t;
    typedef R                   result_t;
  };

  /// \brief   decorator on functions to cache and reuse results
  /// \details On the first invocation with a set of arguments, the result
  ///          is computed and memoized. On later invocations, the memoized
  ///          result is returned.
  /// \tparam Fn the type of the function or functor. The arguments MUST
  ///          have a strict weak ordering and must be copyable.
  ///          References to objects MAY NOT work as intended.
  /// \todo
  template <class Fn>
  struct Memoizer
  {
      using func_t         = Fn;
      using result_t       = typename FnTraits<func_t>::result_t;
      using arguments_t    = typename FnTraits<func_t>::arguments_t;
      using result_cache_t = std::map<arguments_t, result_t>;

      explicit
      Memoizer(Fn f)
      : func(std::move(f))
      {}

      Memoizer()                           = default;
      Memoizer(const Memoizer&)            = default;
      Memoizer(Memoizer&&)                 = default;
      Memoizer& operator=(Memoizer&&)      = default;
      Memoizer& operator=(const Memoizer&) = default;

      /// \tparam Args an argument pack consisting of less-than comparable components
      /// \param  args the arguments to func
      /// \return the result of calling func(args...)
      template <class... Args>
      const result_t& operator()(Args... args)
      {
        arguments_t argPack{args...};
        auto        pos = cache.find(argPack);

        if (pos != cache.end())
        {
          ++num_hits;
          return pos->second;
        }

        result_t    res = func(std::forward<Args>(args)...);

        return cache.emplace(std::move(argPack), std::move(res)).first->second;
      }

      void clear() { cache.clear(); }

      size_t size() const { return cache.size(); }
      size_t hits() const { return num_hits; }

    private:
      size_t         num_hits = 0;
      func_t         func;
      result_cache_t cache;
  };

  template <class Fn>
  std::ostream& operator<<(std::ostream& os, const Memoizer<Fn>& memo)
  {
    return os << memo.hits()
              << " <hits -- size> "
              << memo.size();
  }

  template <class Fn>
  inline
  Memoizer<Fn> memoizer(Fn fn)
  {
    return Memoizer<Fn>(fn);
  }
}
#endif /* _MEMOIZER_H */
