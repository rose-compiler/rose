#ifndef Rosebud_BoostSerializer_H
#define Rosebud_BoostSerializer_H
#include <Rosebud/Serializer.h>

namespace Rosebud {

/** Class serializer using Boost Serialization.
 *
 *  This serializer generates code for boost::serialization. */
class BoostSerializer: public Serializer {
public:
    using Ptr = std::shared_ptr<BoostSerializer>;

protected:
    BoostSerializer() {}

public:
    static Ptr instance();
    virtual std::string name() const override;
    virtual std::string purpose() const override;
    virtual bool isSerializable(const Ast::ClassPtr&) const override;
    virtual void generate(std::ostream &header, std::ostream &impl, const Ast::ClassPtr&, const Generator&) const override;
};

} // namespace
#endif
