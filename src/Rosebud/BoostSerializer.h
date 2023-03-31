#ifndef Rosebud_BoostSerializer_H
#define Rosebud_BoostSerializer_H
#include <Rosebud/Serializer.h>

namespace Rosebud {

class BoostSerializer: public Serializer {
public:
    virtual void generate(std::ostream &header, std::ostream &impl, const Ast::ClassPtr&, const Generator&) const override;
};

} // namespace
#endif
