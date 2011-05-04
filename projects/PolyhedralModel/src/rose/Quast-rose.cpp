
#include "rose/rose-utils.hpp"
#include "common/PolyhedricDependency.hpp"

template <>
bool isBottom<std::pair<SgExprStatement *, size_t> >(std::pair<SgExprStatement *, size_t> p) {
	return p.first == NULL;
}

template <>
std::pair<SgExprStatement *, size_t> getBottom<std::pair<SgExprStatement *, size_t> >() {
	return std::pair<SgExprStatement *, size_t>(NULL, 0);
}

template <>
std::ostream & operator << (std::ostream & out, const std::pair<SgExprStatement *, size_t> & p) {
	out << "< " << *p.first << ", " << p.second << " >";
	return out;
}
