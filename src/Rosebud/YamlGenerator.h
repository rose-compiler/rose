#ifndef Rosebud_YamlGenerator_H
#define Rosebud_YamlGenerator_H
#include <Rosebud/Generator.h>

#include <Sawyer/Yaml.h>

namespace Rosebud {

/** Generator that produces a YAML description of the input. */
class YamlGenerator: public Generator {
    bool showingLocations = false;                      // show input location information
public:
    virtual void adjustParser(Sawyer::CommandLine::Parser&) override;
    virtual void generate(const Ast::ProjectPtr&) override;
private:
    void genLocation(Sawyer::Yaml::Node&, const Ast::NodePtr&, const Token&);
    void genLocation(Sawyer::Yaml::Node&, const Ast::NodePtr&, const std::vector<Token>&);
    void genClass(Sawyer::Yaml::Node&, const Ast::ClassPtr&, const Hierarchy&);
    void genDefinition(Sawyer::Yaml::Node&, const Ast::DefinitionPtr&);
    void genCppStack(Sawyer::Yaml::Node&, const Ast::CppStackPtr&);
    void genProperty(Sawyer::Yaml::Node&, const Ast::PropertyPtr&);
    void genAttribute(Sawyer::Yaml::Node&, const Ast::AttributePtr&);
};

} // namespace
#endif
