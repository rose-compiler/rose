#ifndef CTX_ATTRIBUTE_H
#define CTX_ATTRIBUTE_H 1

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

    void toStream(std::ostream& os, VariableIdMapping* vm) ROSE_OVERRIDE
    {
      lattice.toStream(os, vm);
    }

    std::string toString() ROSE_OVERRIDE
    {
      return lattice.toString();
    }

  private:
    CtxLattice<CallContext>&  lattice;
};

} // namespace CodeThorn

#endif /* CTX_ATTRIBUTE_H */
