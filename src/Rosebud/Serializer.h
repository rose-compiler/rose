#ifndef Rosebud_Serializer_H
#define Rosebud_Serializer_H
#include <Rosebud/Ast.h>

namespace Rosebud {

/** Base class for serialization generators.
 *
 *  A serialization generator is responsible for generating C++ source code to serialize and deserialize instances of an node
 *  class. */
class Serializer {
public:
    virtual ~Serializer() {}

    /** Generate code for the specified class.
     *
     *  The @p header and @p impl streams are output streams for the C++ header file and implementation file, respectively. The
     *  generator is from whence this serializer was called. */
    virtual void generate(std::ostream &header, std::ostream &impl, const Ast::ClassPtr&, const Generator&) const = 0;
};

} // namespace
#endif
