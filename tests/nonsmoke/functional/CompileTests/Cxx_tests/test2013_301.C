    template <class T> struct Predicate1 { enum { value = 0 }; };
 // template <class T> void foo (T x) { ... }

    template <class T> struct Predicate2 { enum { value = 0 }; };
 // template <class T> void foo (T x) { ... }


    template <bool condition> struct Constraint;
    template <> struct Constraint<true> { typedef bool boolAlias; };

    template <class T>
    typename Constraint<Predicate1<T>::value>::boolAlias
    operator== (const T& x, const T& y) { return true; }

    template <class T>
    typename Constraint<Predicate2<T>::value>::boolAlias
    operator== (const T& x, const T& y) { return true; }
