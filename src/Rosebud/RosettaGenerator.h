#ifndef Rosebud_RosettaGenerator_H
#define Rosebud_RosettaGenerator_H
#include <Rosebud/CxxGenerator.h>

namespace Rosebud {

/** Generator that produces ROSETTA output. */
class RosettaGenerator: public CxxGenerator {
public:
    using Ptr = std::shared_ptr<RosettaGenerator>;

private:
    boost::filesystem::path rosettaFileName;            // optional name of file into which ROSETTA source code is written
    boost::filesystem::path implDirectoryName;          // optional name of directory where node implementation files are created
    boost::filesystem::path nodeListFileName;           // optional name of file that contains list of all node types
    std::string grammarFunctionName = "Grammar::no_name"; // name of ROSETTA function being generated
    std::string cppProtection;                          // conditional compilation expression protecting all generated code
    bool strictParents = false;                         // generate extra parent pointer checks and adjustments

protected:
    RosettaGenerator() {}

public:
    static Ptr instance();
    virtual std::string name() const override;
    virtual std::string purpose() const override;
    virtual void adjustParser(Sawyer::CommandLine::Parser&) override;
    virtual void generate(const Ast::ProjectPtr&) override;
    virtual std::string propertyDataMemberName(const Ast::PropertyPtr&p) const override;
    virtual std::vector<std::string> propertyAccessorNames(const Ast::PropertyPtr&) const override;
    virtual std::vector<std::string> propertyMutatorNames(const Ast::PropertyPtr&) const override;

private:
    boost::filesystem::path fileWrtRose(const boost::filesystem::path&);
    std::string shortName(const Ast::ClassPtr&);
    std::string rosettaBaseClass(const Ast::ClassPtr&);
    bool isBaseClass(const Ast::ClassPtr&, const Hierarchy&);
    void genRosettaFileBegin(std::ostream &rosetta);
    void genRosettaFileEnd(std::ostream &rosetta);
    void genRosettaFunctionBegin(std::ostream &rosetta);
    void genRosettaFunctionEnd(std::ostream &rosetta);
    void genImplFileBegin(std::ostream &impl, const Ast::ClassPtr&);
    void genImplFileEnd(std::ostream &impl, const Ast::ClassPtr&);
    void genRosettaPragmas(std::ostream &rosetta, const std::vector<std::string>& pragmas, const Ast::ClassPtr&);
    size_t genRosettaPragmas(std::ostream &rosetta, const Ast::ClassPtr&, const Ast::PropertyPtr&);
    size_t genRosettaPragmas(std::ostream &rosetta, const Ast::ClassPtr&);
    void genClassDefinition(std::ostream&, const Ast::ClassPtr&, const Hierarchy&);
    void genNonterminalMacros(std::ostream &rosetta, const Ast::ClassPtr&, const Hierarchy&);
    void genNewNonterminalMacro(std::ostream &rosetta, const Ast::ClassPtr&, const Hierarchy&);
    void genLeafMacros(std::ostream &rosetta, const Ast::ClassPtr&);
    void genClassBegin(std::ostream &rosetta, const Ast::ClassPtr&);
    void genClassEnd(std::ostream &rosetta, const Ast::ClassPtr&);
    void genClassDeclarations(std::ostream &rosetta, const Classes&);
    void genPropertyDataMember(std::ostream &rosetta, std::ostream &header, const Ast::PropertyPtr&);
    void genPropertyAccessors(std::ostream &header, std::ostream &impl, const Ast::PropertyPtr&);
    void genPropertyMutators(std::ostream &header, std::ostream &impl, const Ast::PropertyPtr&);
    void genProperty(std::ostream &rosetta, std::ostream &header, std::ostream &impl, const Ast::PropertyPtr&);
    void genCppConditions(std::ostream &out, const Ast::FilePtr&, const std::string &text);
    void genClassDestructor(std::ostream &header, std::ostream &impl, const Ast::ClassPtr&);
    void genClassConstructors(std::ostream &header, std::ostream &impl, const Ast::ClassPtr&, const Hierarchy&);
    void genOtherContent(std::ostream &rosetta, const Ast::ClassPtr&, const Hierarchy&, const std::string &content);
    std::vector<std::string> implementationFileNames(const Classes&);
    void adjustNodeList(const std::shared_ptr<Ast::Project>&);
    void genTupFile(const std::vector<std::string> &implFileNames);
    void genMakeFile(const std::vector<std::string> &implFileNames);
    void genCmakeFile(const std::vector<std::string> &implFileNames);
};

} // namespace
#endif
