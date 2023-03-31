#ifndef Rosebud_Generator_H
#define Rosebud_Generator_H
#include <Rosebud/Utility.h>

#include <Sawyer/CommandLine.h>

namespace Rosebud {

/** Base class for generators. */
class Generator {
public:
    virtual void adjustParser(Sawyer::CommandLine::Parser&) {}
    virtual void generate(const Ast::ProjectPtr&) = 0;
    virtual std::string propertyDataMemberName(const Ast::PropertyPtr&) const;
    virtual std::vector<std::string> propertyAccessorNames(const Ast::PropertyPtr&) const;
    virtual std::vector<std::string> propertyMutatorNames(const Ast::PropertyPtr&) const;
};

} // namespace
#endif
