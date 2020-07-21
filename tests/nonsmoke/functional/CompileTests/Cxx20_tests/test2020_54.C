template <class T = void>
    requires EqualityComparable<T> || Same<T, void>
struct equal_to;

