#ifndef Rosebud_Serializer_H
#define Rosebud_Serializer_H
#include <Rosebud/Ast.h>

namespace Rosebud {

class Serializer {
public:
    virtual ~Serializer() {}
    virtual void generate(std::ostream &header, std::ostream &impl, const Ast::ClassPtr&, const Generator&) const = 0;
};

} // namespace
#endif
