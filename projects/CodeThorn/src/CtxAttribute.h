#ifndef CTX_ATTRIBUTE_H
#define CTX_ATTRIBUTE_H 1

/// \author Peter Pirkelbauer

#include <iosfwd>

namespace CodeThorn {

template <class CallContext>
struct CtxAttribute : DFAstAttribute
{
    explicit
    CtxAttribute(CtxLattice<CallContext>& lat)
    : lattice(lat)
    {}

    ~CtxAttribute() { delete &lattice; }

    void toStream(std::ostream& os, VariableIdMapping* vm) override
    {
      lattice.toStream(os, vm);
    }

    std::string toString() override
    {
      return lattice.toString();
    }

  private:
    CtxLattice<CallContext>&  lattice;
};

} // namespace CodeThorn

#endif /* CTX_ATTRIBUTE_H */
