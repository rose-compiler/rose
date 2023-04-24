#ifndef Rosebud_NoneGenerator_H
#define Rosebud_NoneGenerator_H
#include <Rosebud/Generator.h>

#include <Sawyer/CommandLine.h>

namespace Rosebud {

/** Generator that produces a YAML description of the input. */
class NoneGenerator: public Generator {
public:
    using Ptr = std::shared_ptr<NoneGenerator>;

protected:
    NoneGenerator() {}

public:
    static Ptr instance();
    virtual std::string name() const override;
    virtual std::string purpose() const override;
    virtual void generate(const Ast::ProjectPtr&) override;
};

} // namespace
#endif
