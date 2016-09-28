template<typename _StateT>
class fpos
    {
    };
     
  // The standard only requires that operator== must be an
  // equivalence relation. In this implementation two fpos<StateT>
  // objects belong to the same equivalence class if the contained
  // offsets compare equal.
  /// Test if equivalent to another position.
template<typename _StateT>
inline bool operator==(const fpos<_StateT>& __lhs, const fpos<_StateT>& __rhs)
   { return true; /* streamoff(__lhs) == streamoff(__rhs); */ }

template<typename _StateT>
inline bool operator!=(const fpos<_StateT>& __lhs, const fpos<_StateT>& __rhs)
   { return false; /* streamoff(__lhs) != streamoff(__rhs); */ }

