#include <Rosebud/YamlGenerator.h>

#include <boost/lexical_cast.hpp>

#include <iostream>

namespace Rosebud {

YamlGenerator::Ptr
YamlGenerator::instance() {
    return Ptr(new YamlGenerator);
}

std::string
YamlGenerator::name() const {
    return "yaml";
}

std::string
YamlGenerator::purpose() const {
    return "Generate YAML output that can be parsed by standalone backends.";
}

void
YamlGenerator::adjustParser(Sawyer::CommandLine::Parser &parser) {
    using namespace Sawyer::CommandLine;

    SwitchGroup sg("YAML backend (--backend=" + name() + ")");
    sg.name(name());
    sg.doc("This backend produces a YAML file that describes the input. Its purpose is to make it easier to write tools that "
           "generate code since these tools do not need to be able to parse the Rosebud C++-like input language, but rather "
           "the easily parsed YAML representation of that input. Most mainstream source languages have good YAML parsing "
           "capabilities. The ROSE library itself can use either `Sawyer::Yaml` or `Rose::Yaml` (they're aliases, but the "
           "`Rose::Yaml` is present only in librose.\n\n"

           "The YAML that this backend generates should be fairly easy to understand. It's mainly a list of classes which "
           "each contain a list of properties, which each contain a list of attributes. Each class, property, and attribute "
           "has a name. If source location output is enabled, then the YAML document will also describe where each entity "
           "occurred in the Rosebud input.\n\n"

           "Some of the not-so self-documenting parts of the YAML are described here.\n\n"

           "The \"prior_text\" values are the non-C++ tokens that appeared before a definition. These usually consist of "
           "white space, comments, and C preprocessor directives. The closely related class \"conditions\" list is a list "
           "of C preprocessor conditional compilation directives that were not closed prior to the beginning of a class or "
           "property. Each element of this list will be one or more related directives (such as \"#if X\" and \"#else\" when "
           "the input had a an \"#if X\" that was later matched to an \"#else\" which was not terminated with an \"#endif\" "
           "prior to the start of the definition).\n\n"

           "Each class is usually derived from a base class whose name and visibility will appear in the class \"inheritance\" "
           "list.\n\n"

           "Each property has a name and a C type (\"c_type\"), and an optional C initialization expression (\"c_init\"). Any "
           "names specified by Rosebud::data, Rosebud::accessors, or Rosebud::mutators will be shown under the YAML keys "
           "\"data_member_name\", \"accessor_names\", and \"mutator_names\", respectively.\n\n"

           "Property attributes are listed in the order they were parsed from the input. Each attribute has a fully qualified "
           "name, a boolean to indicate whether an argument list was present (even if the argument list was empty), and a list "
           "of zero or more arguments.\n\n"

           "Locations for many of these things are output as \"location\" objects. Each location has a file name, a starting line "
           "number, and a starting column number. Lines and columns are numbered starting at one.\n\n"

           "The following command-line switches are understood by this backend:");

    // We need at least one switch here in order for the above documentation to appear in the man page.

    sg.insert(Switch("indentation")
              .argument("n", nonNegativeIntegerParser(indentationAmount))
              .doc("Number of columns to indent each level of YAML output. The default is " +
                   boost::lexical_cast<std::string>(indentationAmount) + "."));

    parser.with(sg);
}

void
YamlGenerator::genLocation(Sawyer::Yaml::Node &root, const Ast::Node::Ptr &node, const Token &token) {
    ASSERT_not_null(node);

    if (token) {
        if (auto file = node->findAncestor<Ast::File>()) {
            auto lc = file->tokenStream().location(token);
            root["file"] = file->name();
            root["line"] = lc.first + 1;
            root["column"] = lc.second + 1;
            root["length"] = token.size();
        }
    }
}

void
YamlGenerator::genLocation(Sawyer::Yaml::Node &root, const Ast::Node::Ptr &node, const std::vector<Token> &tokens) {
    ASSERT_not_null(node);

    if (!tokens.empty())
        genLocation(root, node, tokens.front());
}

void
YamlGenerator::genCppStack(Sawyer::Yaml::Node &root, const Ast::CppStack::Ptr &cppStack) {
    if (cppStack && !cppStack->stack.empty()) {
        for (const Ast::CppStack::Level &level: cppStack->stack) {
            auto &levelNode = root["cpp_stack"].pushBack();
            for (const Ast::CppStack::Directive &directive: level) {
                auto &dirNode = levelNode.pushBack();
                dirNode["directive"] = prefixLines(directive.lexeme, "|");
                if (settings.showingLocations)
                    genLocation(dirNode["directive_location"], cppStack, directive.token);
            }
        }
    }
}

void
YamlGenerator::genAttribute(Sawyer::Yaml::Node &root, const Ast::Attribute::Ptr &attribute) {
    ASSERT_not_null(attribute);
    root["name"] = attribute->fqName;
    if (settings.showingLocations)
        genLocation(root["name_location"], attribute, attribute->nameTokens);
    if (attribute->arguments) {
        for (const auto &arg: attribute->arguments->elmts) {
            auto &argNode = root["arguments"].pushBack();
            argNode["argument"] = arg->string();
            if (settings.showingLocations)
                genLocation(argNode["argument_location"], arg(), arg->tokens);
        }
    }
}

void
YamlGenerator::genDefinition(Sawyer::Yaml::Node &root, const Ast::Definition::Ptr &defn) {
    ASSERT_not_null(defn);

    if (settings.showingLocations)
        genLocation(root["starting_location"], defn, defn->startToken);

    root["name"] = defn->name;
    if (settings.showingLocations)
        genLocation(root["name_location"], defn, defn->nameToken);

    if (!defn->doc.empty()) {
        root["doc"] = prefixLines(defn->doc, "|");
        if (settings.showingLocations)
            genLocation(root["doc_location"], defn, defn->docToken);
    }

    genCppStack(root, defn->cppStack());

    if (!defn->priorText.empty()) {
        root["prior_text"] = prefixLines(defn->priorText, "|");
        if (settings.showingLocations)
            genLocation(root["prior_text_location"], defn, defn->priorTextToken);
    }

    for (const auto &attribute: defn->attributes)
        genAttribute(root["attributes"].pushBack(), attribute());
}

void
YamlGenerator::genProperty(Sawyer::Yaml::Node &root, const Ast::Property::Ptr &property) {
    ASSERT_not_null(property);
    genDefinition(root, property);
    if (property->cType) {
        root["type"]["c_code"] = prefixLines(property->cType->string(), "|");
        if (settings.showingLocations)
            genLocation(root["type"]["location"], property, property->cType->tokens);
    }
    if (property->cInit) {
        root["init"]["c_code"] = prefixLines(property->cInit->string(), "|");
        if (settings.showingLocations)
            genLocation(root["init"]["location"], property, property->cInit->tokens);
    }

    if (property->dataMemberName)
        root["data_member"] = property->dataMemberName;

    if (property->accessorNames) {
        if (property->accessorNames->empty()) {
            root["accessor_names"] = "none";
        } else {
            for (const std::string &s: *property->accessorNames)
                root["accessor_names"].pushBack() = s;
        }
    }

    if (property->mutatorNames) {
        if (property->mutatorNames->empty()) {
            root["mutator_names"] = "none";
        } else {
            for (const std::string &s: *property->mutatorNames)
                root["mutator_names"].pushBack() = s;
        }
    }
}

void
YamlGenerator::genClass(Sawyer::Yaml::Node &root, const Ast::Class::Ptr &c, const Hierarchy &h) {
    ASSERT_not_null(c);
    genDefinition(root, c);

    for (const auto &pair: c->inheritance) {
        auto &node = root["inheritance"].pushBack();
        node["visibility"] = pair.first;
        node["super_class"] = pair.second;
    }

    for (const auto &property: c->properties) {
        auto &node = root["properties"].pushBack();
        genProperty(node, property());
    }

    if (!c->endText.empty()) {
        root["end_text"] = prefixLines(c->endText, "|");
        if (settings.showingLocations)
            genLocation(root["end_text_location"], c, c->endTextToken);
    }

    if (!c->qualifiedNamespace.empty())
        root["namespace"] = c->qualifiedNamespace;

    const Classes dcs = derivedClasses(c, h);
    if (!dcs.empty()) {
        for (const auto &dc: dcs)
            root["derived_classes"].pushBack() = dc->name;
    }
}

void
YamlGenerator::generate(const Ast::Project::Ptr &project) {
    ASSERT_not_null(project);

    Sawyer::Yaml::Node root;
    Hierarchy h = classHierarchy(project->allClassesFileOrder());
    checkClassHierarchy(h);

    // Information about files
    for (const auto &file: project->files) {
        auto &fileNode = root["files"].pushBack();
        fileNode["name"] = file->tokenStream().fileName();
        fileNode["lines"] = file->tokenStream().content().nLines();
        fileNode["size"] = file->tokenStream().content().nCharacters();
        if (!file->endText.empty()) {
            fileNode["end_text"] = prefixLines(file->endText, "|");
            if (settings.showingLocations)
                genLocation(fileNode["end_text_location"], file(), file->endTextToken);
        }
    }

    // Information about classes
    for (const auto &file: project->files) {
        for (const auto &c: file->classes) {
            auto &classNode = root["classes"].pushBack();
            genClass(classNode, c(), h);
        }
    }

    const size_t indent = std::max(1, indentationAmount);
    Sawyer::Yaml::Serialize(root, std::cout, Sawyer::Yaml::SerializeConfig{indent, 10*1024*1024, false, false});
}

} // namespace
