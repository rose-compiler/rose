// Use of template variable.

// math constant with precision dictated by actual type
template<typename T> constexpr T pi = T(3.14159265358979323846);

// Example use:
template<class T> T area_of_circle_with_radius(T r) { return pi<T> * r * r; }

// Same use, in a  more C++14-stylish way:
// auto area_of_circle_with_radius = [](auto r) { return pi<decltype(r)> * r * r; };
