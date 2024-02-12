#ifndef Rosebud_SawyerGenerator_H
#define Rosebud_SawyerGenerator_H
#include <Rosebud/CxxGenerator.h>

namespace Rosebud {

/** Generator that produces Sawyer::Tree class hierarchies. */
class SawyerGenerator: public CxxGenerator {
    using Base = CxxGenerator;

public:
    using Ptr = std::shared_ptr<SawyerGenerator>;

private:
    boost::filesystem::path generatedDir;               // directory where generated files are written
    boost::filesystem::path headerPrefix;               // installation directory for the header files

protected:
    SawyerGenerator() {}

public:
    static Ptr instance();

public:
    std::string name() const override;
    std::string purpose() const override;
    void adjustParser(Sawyer::CommandLine::Parser&) override;
    void generate(const Ast::ProjectPtr&) override;
    std::string valueType(const Ast::PropertyPtr&) override;
    void genDefaultConstructor(std::ostream&, std::ostream&, const Ast::ClassPtr&, Access) override;
    bool genArgsConstructor(std::ostream&, std::ostream&, const Ast::ClassPtr&, const Hierarchy&, Access) override;
    void genConstructorBody(std::ostream&, const Ast::ClassPtr&) override;
    std::string ctorInitializerExpression(const Ast::PropertyPtr&, const std::string&) override;
    std::string propertyDataMemberName(const Ast::PropertyPtr&) const override;
    std::string propertyDataMemberType(const Ast::PropertyPtr&) override;
    std::string propertyMutatorArgumentType(const Ast::PropertyPtr&) override;

private:
    void genNodeDeclarations(const Ast::ProjectPtr&);
    void genClass(const Ast::ClassPtr&, const Hierarchy&);
    void genClassConstructors(std::ostream&, std::ostream&, const Ast::ClassPtr&, const Hierarchy&);
    void genHeaderClose(std::ostream&, const Ast::ClassPtr&);
    void genHeaderOpen(std::ostream&, const Ast::ClassPtr&);
    void genImplClose(std::ostream&, const Ast::ClassPtr&);
    void genImplOpen(std::ostream&, const Ast::ClassPtr&);
    void genProperty(std::ostream&, std::ostream&, const Ast::PropertyPtr&);
    void genPropertyAccessors(std::ostream&, std::ostream&, const Ast::PropertyPtr&);
    void genPropertyDataMember(std::ostream&, const Ast::PropertyPtr&);
    void genPropertyMutators(std::ostream&, std::ostream&, const Ast::PropertyPtr&);
    void genTupFile(const std::vector<std::string> &implNames, const std::vector<std::string> &hdrNames);
    void genMakeFile(const std::vector<std::string> &implNames, const std::vector<std::string> &hdrNames);
    void genCmakeFile(const std::vector<std::string> &implNames, const std::vector<std::string> &hdrNames);
    void genNamespaceDeclarationOpen(std::ostream&, const std::string&);
    void genNamespaceDeclarationClose(std::ostream&, const std::string&);

    // A mutli-line title comment. The comment is indented according to the leading space in the argument.
    static std::string title(const std::string&);

    // Returns T if property's type is Edge<T> or EdgeVector<T>
    Sawyer::Optional<std::string> treeScalarEdgeInnerType(const Ast::PropertyPtr&);
    Sawyer::Optional<std::string> treeVectorEdgeInnerType(const Ast::PropertyPtr&);

    // True if property's type is Edge<T> or EdgeVector<T>
    bool isTreeScalarEdge(const Ast::PropertyPtr&);
    bool isTreeVectorEdge(const Ast::PropertyPtr&);
    bool isTreeEdge(const Ast::PropertyPtr&);

    std::vector<std::string> implementationFileNames(const Classes&);
    std::vector<std::string> headerFileNames(const Classes&);
};

} // namespace

#endif
