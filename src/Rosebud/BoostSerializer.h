#ifndef Rosebud_BoostSerializer_H
#define Rosebud_BoostSerializer_H
#include <Rosebud/Serializer.h>

namespace Rosebud {

/** Class serializer using Boost Serialization.
 *
 *  This serializer generates code for boost::serialization. */
class BoostSerializer: public Serializer {
public:
    virtual void generate(std::ostream &header, std::ostream &impl, const Ast::ClassPtr&, const Generator&) const override;
};

} // namespace
#endif
