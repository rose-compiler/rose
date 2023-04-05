#ifndef Rosebud_RoseGenerator_H
#define Rosebud_RoseGenerator_H
#include <Rosebud/CxxGenerator.h>

namespace Rosebud {

/** Generator that produces ROSE code. */
class RoseGenerator: public CxxGenerator {
    using Base = CxxGenerator;

    boost::filesystem::path generatedDir;               // directory where generated files are written

public:
    virtual void adjustParser(Sawyer::CommandLine::Parser&) override;
    virtual void generate(const Ast::ProjectPtr&) override;

protected:
    virtual std::string ctorInitializerExpression(const Ast::PropertyPtr&, const std::string &expr) override;
    virtual std::string valueType(const Ast::PropertyPtr&) override;

private:
    void adjustAst(const Classes&);
    bool isTreeEdge(const Ast::PropertyPtr&);
    void genImplOpen(std::ostream &impl, const Ast::ClassPtr&);
    void genImplClose(std::ostream &impl, const Ast::ClassPtr&);
    void genHeaderOpen(std::ostream &header, const Ast::ClassPtr&);
    void genHeaderClose(std::ostream &header, const Ast::ClassPtr&);
    void genHeaderIncludes(std::ostream &header, const Ast::ClassPtr&, const Hierarchy&);
    void genPropertyDataMember(std::ostream &header, const Ast::PropertyPtr&);
    void genPropertyAccessors(std::ostream &header, std::ostream &impl, const Ast::PropertyPtr&);
    void genPropertyMutators(std::ostream &header, std::ostream &impl, const Ast::PropertyPtr&);
    void genProperty(std::ostream &header, std::ostream &impl, const Ast::PropertyPtr&);
    void genDefaultConstructor(std::ostream &header, std::ostream &impl, const Ast::ClassPtr&);
    void genArgsConstructor(std::ostream &header, std::ostream &impl, const Ast::ClassPtr&, const Hierarchy&);
    void genTraversals(std::ostream &header, std::ostream &impl, const Ast::ClassPtr&, const Hierarchy&);
    void genClass(const Ast::ClassPtr&, const Hierarchy&);
    void genBasicTypes(const Ast::ProjectPtr&);
};

} // namespace
#endif
