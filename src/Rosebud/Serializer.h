#ifndef Rosebud_Serializer_H
#define Rosebud_Serializer_H
#include <Rosebud/Ast.h>

#include <Rosebud/Utility.h>

namespace Rosebud {

/** Base class for serialization generators.
 *
 *  A serialization generator is responsible for generating C++ source code to serialize and deserialize instances of an node
 *  class. */
class Serializer {
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Nested types
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Shared-ownership pointer. */
    using Ptr = SerializerPtr;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Data members
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
private:
    static std::vector<Ptr> registry_;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Constructors, etc.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    virtual ~Serializer() {}
protected:
    Serializer() {}                                     // use derived class `instance` static member functions instea

public:
    /** Register a serializer for use later. */
    static void registerSerializer(const Ptr&);

    /** Return all registered serializers. */
    static const std::vector<Ptr>& registeredSerializers();

    /** Return the registered serializer with the specified name. */
    static Ptr lookup(const std::string&);

    /** Returns the serializers registered with the specified names. */
    static std::vector<Ptr> lookup(const std::vector<std::string>&);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Properties
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Every serializer has a unique name. */
    virtual std::string name() const = 0;

    /** Single-line description for the serializer. */
    virtual std::string purpose() const = 0;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Code generation
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Determines if a class should be serialized.
     *
     *  Returns true if this class should have serilization and deserialization functions. */
    virtual bool isSerializable(const Ast::ClassPtr&) const = 0;

    /** Generate prologue code.
     *
     *  In the header file, the prologue goes immediately after the generated include-once protection and before any user-written
     *  pre-class-definition code. */
    virtual void genPrologue(std::ostream &header, std::ostream &impl, const Ast::ClassPtr&, const Hierarchy&,
                             const Generator&) const = 0;

    /** Generate main serialization code for the specified class.
     *
     *  This code emitted by this function will appear inside the class definition in the header, and near the end of the
     *  implementation file.
     *
     *  The @p header and @p impl streams are output streams for the C++ header file and implementation file, respectively. The
     *  generator is from whence this serializer was called. */
    virtual void genBody(std::ostream &header, std::ostream &impl, const Ast::ClassPtr&, const Hierarchy&,
                         const Generator&) const = 0;

    /** Generate epilogue code.
     *
     *  In the header file, this goes at the end of the file, after the class definition and after the namespace (if any) but
     *  before the closing compile-once directives. */
    virtual void genEpilogue(std::ostream &header, std::ostream &impl, const Ast::ClassPtr&, const Hierarchy&,
                             const Generator&) const = 0;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Support
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
private:
    static void initRegistry();
};

} // namespace
#endif
