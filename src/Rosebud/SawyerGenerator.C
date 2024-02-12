#include <Rosebud/SawyerGenerator.h>

#include <Rose/Affirm.h>

#include <Sawyer/Optional.h>
#include <boost/algorithm/string/predicate.hpp>
#include <fstream>

using namespace Sawyer::Message::Common;
using Rose::notnull;

namespace Rosebud {

SawyerGenerator::Ptr
SawyerGenerator::instance() {
    return Ptr(new SawyerGenerator);
}

std::string
SawyerGenerator::name() const {
    return "sawyer";
}

std::string
SawyerGenerator::purpose() const {
    return "Experimental backend to generate Sawyer::Tree classs hierarchies.";
}

void
SawyerGenerator::adjustParser(Sawyer::CommandLine::Parser &parser) {
    using namespace Sawyer::CommandLine;

    SwitchGroup sg("Sawyer backend(--backend=" + name() + ")");
    sg.name(name());
    sg.doc("This backend generates Sawyer::Tree code. The goal is to use a more modern style of tree data structures in ROSE, such "
           "as for abstract syntax trees (ASTs)."

           "@bullet{Automatic parent pointers. It is impossible to mess up the parent/child relationships in a tree.}"
           "@bullet{Immediately enforced tree invariants. No need for a separate consistency checker.}"
           "@bullet{Reference counted vertices. The nodes of the tree have clear ownership rules that are enforced.}"
           "@bullet{Simple traversals. Only two traversals upon which all others can be built.}"
           "@bullet{Event driven. Observers can register to be notified when parent/child edges change.}"
           "@bullet{Exception safe. Tree connectivity will be consistent even when exceptions are thrown.}"
           "@bullet{Disjoint class hierarchies. Multiple tree class hierarchies are possible.}"
           "@bullet{Well tested. Sawyer trees aim for 100% testing coverage through unit tests.}"

           "This backend is not intended to generate code that is compatible with the traditional ROSE AST; it should only be "
           "used for new nodes that don't integrate into the same class hierarchy as the traditional AST node types.");

    sg.insert(Switch("generated")
              .argument("directory", anyParser(generatedDir))
              .doc("Name of the directory that will contain the generated C++ files."));

    sg.insert(Switch("header-prefix")
              .argument("directory", anyParser(headerPrefix))
              .doc("Name of the directory where generated header files will be installed. This should be relative to the "
                   "project's main header installation prefix. The default is empty, which means the header files will be "
                   "installed in the project's main header installation prefix."));

    parser.with(sg);
}

void
SawyerGenerator::generate(const Ast::Project::Ptr &project) {
    ASSERT_not_null(project);

    Hierarchy h = classHierarchy(project->allClassesFileOrder());
    const Classes classes = topDown(h);

    genNodeDeclarations(project);
    for (const auto &c: classes)
        genClass(c, h);

    const std::vector<std::string> implNames = implementationFileNames(classes);
    std::vector<std::string> headerNames = headerFileNames(classes);
    headerNames.push_back("NodeDeclarations.h");

    genTupFile(implNames, headerNames);
    genCmakeFile(implNames, headerNames);
    genMakeFile(implNames, headerNames);
}

Sawyer::Optional<std::string>
SawyerGenerator::treeVectorEdgeInnerType(const Ast::Property::Ptr &p) {
    ASSERT_not_null(p);
    static std::regex re("((Sawyer::)?Tree::)?EdgeVector<(.*)>");
    std::smatch found;
    const std::string type = propertyDataMemberType(p);
    if (std::regex_match(type, found, re)) {
        return found.str(3);
    } else {
        return Sawyer::Nothing();
    }
}

bool
SawyerGenerator::isTreeVectorEdge(const Ast::Property::Ptr &p) {
    return !treeVectorEdgeInnerType(p).orDefault().empty();
}

Sawyer::Optional<std::string>
SawyerGenerator::treeScalarEdgeInnerType(const Ast::Property::Ptr &p) {
    ASSERT_not_null(p);
    static std::regex re("((Sawyer::)?Tree::)?Edge<(.*)>");
    std::smatch found;
    const std::string type = propertyDataMemberType(p);
    if (std::regex_match(type, found, re)) {
        return found.str(3);
    } else {
        return Sawyer::Nothing();
    }
}

bool
SawyerGenerator::isTreeScalarEdge(const Ast::Property::Ptr &p) {
    return !treeScalarEdgeInnerType(p).orDefault().empty();
}

bool
SawyerGenerator::isTreeEdge(const Ast::Property::Ptr &p) {
    return isTreeScalarEdge(p) || isTreeVectorEdge(p);
}

std::string
SawyerGenerator::propertyDataMemberType(const Ast::Property::Ptr &p) {
    ASSERT_not_null(p);
    ASSERT_not_null(p->cType);
    const std::string specifiedType = p->cType->string();
    const std::string nonPointerType = removePointer(specifiedType);
    ASSERT_forbid(specifiedType.empty());

    if (p->findAttribute("Rosebud::traverse")) {
        if (!isTreeEdge(p)) {
            message(ERROR, p->findFirstAncestor<Ast::File>(), p->nameToken,
                    "property \"" + p->name + "\" type must be Edge or EdgeVector"
                    " when the \"Rosebud::traverse\" property is specified.");
        }
        return "INVALID_TYPE<" + Base::propertyDataMemberType(p) + ">";

    } else if (notnull(p->findAncestor<Ast::Project>())->findClassByName(nonPointerType)) {
        if (nonPointerType == specifiedType)
            message(ERROR, "property \"" + p->name + "\" has type \"" + specifiedType + "\" must be a pointer");
        return nonPointerType + "Ptr";

    } else {
        return Base::propertyDataMemberType(p);
    }
}

std::string
SawyerGenerator::propertyMutatorArgumentType(const Ast::Property::Ptr &p) {
    if (const auto inner = treeScalarEdgeInnerType(p)) {
        return *inner + "Ptr";
    } else if (const auto inner = treeVectorEdgeInnerType(p)) {
        return *inner + "Ptr";
    } else {
        return Base::propertyMutatorArgumentType(p);
    }
}

std::string
SawyerGenerator::valueType(const Ast::Property::Ptr &p) {
    ASSERT_not_null(p);
    static std::regex re("((Sawyer::)?Tree::)?Edge(Vector)?<(.*)>");
    std::smatch found;
    const std::string type = propertyDataMemberType(p);
    if (std::regex_match(type, found, re)) {
        return found.str(4) + "Ptr";
    } else {
        return type;
    }
}

std::string
SawyerGenerator::title(const std::string &s) {
    const size_t indented = [&s]() {
        static std::regex re("(\\s*)(.*)");
        std::smatch found;
        return std::regex_match(s, found, re) ? found.str(1).size() : 0;
    }();

    const std::string prefix(indented, ' ');
    const size_t nSlashes = indented < 128 ? 128 - indented : 0;

    return ("\n" +
            prefix + std::string(nSlashes, '/') + "\n" +
            prefix + "// " + s.substr(indented) + "\n" +
            prefix + std::string(nSlashes, '/') + "\n");
}

void
SawyerGenerator::genPropertyDataMember(std::ostream &header, const Ast::Property::Ptr &p) {
    ASSERT_not_null(p);
    header <<THIS_LOCATION <<locationDirective(p, p->startToken)
           <<"private:\n"
           <<"    // Automatically generated; do not modify!\n"
           <<"    " <<propertyDataMemberType(p) <<" " <<propertyDataMemberName(p) <<";\n";
}

void
SawyerGenerator::genPropertyAccessors(std::ostream &header, std::ostream &impl, const Ast::Property::Ptr &p) {
    ASSERT_not_null(p);

    for (const std::string &accessorName: propertyAccessorNames(p)) {
        const std::string type = propertyAccessorReturnType(p);
        const auto c = p->findAncestor<Ast::Class>();
        const bool isLarge = isTreeEdge(p) || p->findAttribute("Rosebud::large");
        ASSERT_not_null(c);

        //--------------------------------------------------------------------
        // Normal type: `const T& property() const;`
        {
            const std::string constRefType = constRef(type);
            header <<THIS_LOCATION <<locationDirective(p, p->startToken)
                   <<"public:\n"
                   <<"    // Automatically generated; do not modify!\n"
                   <<"    " <<constRefType <<" " <<accessorName <<"() const;\n";

            impl <<"\n"
                 <<THIS_LOCATION <<locationDirective(p, p->startToken)
                 <<"// Automatically generated; do not modify!\n"
                 <<"auto " <<c->name <<"::" <<accessorName <<"() const -> " <<constRefType <<" {\n";
            if (p->findAttribute("Rosebud::not_null") && !isTreeVectorEdge(p))
                impl <<"    ASSERT_not_null(" <<propertyDataMemberName(p) <<");\n";
            impl <<"    return " <<propertyDataMemberName(p) <<";\n"
                 <<"}\n";
        }

        //--------------------------------------------------------------------
        // Large type: `T& property();`
        if (isLarge) {
            const std::string refType = type + "&";
            header <<THIS_LOCATION <<locationDirective(p, p->startToken)
                   <<"public:\n"
                   <<"    // Automatically generated; do not modify!\n"
                   <<"    " <<refType <<" " <<accessorName <<"();\n";

            impl <<"\n"
                 <<THIS_LOCATION <<locationDirective(p, p->startToken)
                 <<"// Automatically generated; do not modify!\n"
                 <<"auto " <<c->name <<"::" <<accessorName <<"() -> " <<refType <<" {\n";
            if (p->findAttribute("Rosebud::not_null") && !isTreeVectorEdge(p))
                impl <<"    ASSERT_not_null(" <<propertyDataMemberName(p) <<");\n";
            impl <<"    return " <<propertyDataMemberName(p) <<";\n"
                 <<"}\n";
        }
    }
}

void
SawyerGenerator::genPropertyMutators(std::ostream &header, std::ostream &impl, const Ast::Property::Ptr &p) {
    ASSERT_not_null(p);

    // Vector of tree edges never has a mutator because changes to the vector are made through its accessor. For example, a property
    // definition like this:
    //   [[Rosebud::property]]
    //   EdgeVector<Thing> things;
    //
    // is modified by the user like this:
    //   node.things().push_back(thing);
    // or
    //   node.things().back();
    // or
    //   node.things()[i];
    // etc.
    if (isTreeVectorEdge(p))
        return;

    const std::string type = propertyMutatorArgumentType(p);
    const std::string constRefType = constRef(type);
    const auto c = p->findAncestor<Ast::Class>();
    ASSERT_not_null(c);

    // The normal mutators for all users of this class.
    for (const std::string &mutatorName: propertyMutatorNames(p)) {
        // Declaration
        header <<THIS_LOCATION <<locationDirective(p, p->startToken)
               <<"public:\n"
               <<"    // Automatically generated; do not modify!\n"
               <<"    void " <<mutatorName <<"(" <<constRefType <<");\n";

        // Implementation
        impl <<"\n"
             <<THIS_LOCATION <<locationDirective(p, p->startToken)
             <<"// Automatically generated; do not modify!\n"
             <<"void\n"
             <<c->name <<"::" <<mutatorName <<"(" <<constRefType <<" x) {\n";
        if (p->findAttribute("Rosebud::not_null"))
            impl <<"    ASSERT_not_null2(x, \"property cannot be set to null\");\n";
        impl <<"    this->" <<propertyDataMemberName(p) <<" = x;\n"
             <<"}\n";
    }

    // If no mutators are given, we still might want the class itself to be able to modify the data member, such as in hand-written
    // constructors. This is hard to do if the data member names are hidden by a random name generator. We don't want the mutation
    // mechanism to be too easy since that might encourage class authors to abuse it -- so we make it stand out a bit by modifying
    // the mutator name.
    if (propertyMutatorNames(p).empty()) {
        // Declaration
        header <<THIS_LOCATION <<locationDirective(p, p->startToken)
               <<"private:\n"
               <<"    // Automatically generated; do not modify!\n"
               <<"    void forceSet" <<pascalCase(p->name) <<"(" <<constRefType <<");\n";

        // Implementation
        impl <<"\n"
             <<THIS_LOCATION <<locationDirective(p, p->startToken)
             <<"// Automatically generated; do not modify!\n"
             <<"void "<<c->name <<"::forceSet" <<pascalCase(p->name) <<"(" <<constRefType <<" x) {\n";
        if (p->findAttribute("Rosebud::not_null"))
            impl <<"    ASSERT_not_null2(x, \"property cannot be set to null\");\n";
        impl <<"    this->" <<propertyDataMemberName(p) <<" = x;\n"
             <<"}\n";
    }
}

void
SawyerGenerator::genProperty(std::ostream &header, std::ostream &impl, const Ast::Property::Ptr &p) {
    ASSERT_not_null(p);

    // Emit text prior to the property
    if (!p->priorText.empty()) {
        header <<"\n"
               <<THIS_LOCATION <<locationDirective(p, p->priorTextToken)
               <<p->priorText <<"\n";
    }

    // Fix up the doxygen comment so it can apply to more than one class member. If the doxygen comment doesn't end with "@{" then
    // we need to add that.
    const auto doc = [&p]() -> std::pair<std::string, std::string> {
        std::string begin = p->doc;
        std::string end;
        if (!begin.empty()) {
            if (begin.find("@{") == std::string::npos) {
                begin = appendToDoxygen(begin, "\n@{");
                end = "    /** @} */\n";
            }
            if (!boost::ends_with(begin, "\n"))
                begin += "\n";
        }
        return {begin, end};
    }();

    // Emit documentation
    if (!doc.first.empty())
        header <<"\n"
               <<THIS_LOCATION <<locationDirective(p, p->docToken) <<doc.first;

    genPropertyDataMember(header, p);
    genPropertyAccessors(header, impl, p);
    genPropertyMutators(header, impl, p);

    // Close documentation
    header <<doc.second;
};

std::string
SawyerGenerator::ctorInitializerExpression(const Ast::Property::Ptr &p, const std::string &init) {
    ASSERT_not_null(p);
    const std::string checkedInit = init.empty() ? "" : (p->findAttribute("Rosebud::not_null") ? "notnull(" + init + ")" : init);

    if (isTreeEdge(p)) {
        return init.empty() ? "*this" : "*this, " + checkedInit;
    } else {
        return checkedInit;
    }
}

void
SawyerGenerator::genConstructorBody(std::ostream &impl, const Ast::Class::Ptr &c) {
    ASSERT_not_null(c);

    for (const auto &p: c->properties) {
        if (p->findAttribute("Rosebud::not_null")) {
            const std::string t = valueType(p()) + " const&";
            if (isTreeVectorEdge(p())) {
                // Whenever a new edge is added to the vector we need to make sure the child pointer is not null. Also, we need
                // to make sure that the edge cannot be set to null later.
                impl <<"\n"
                     <<THIS_LOCATION <<locationDirective(p(), p->startToken)
                     <<"    // Tree edge vector member cannot be set to null initially or later due to Rosebud::not_null attribute\n"
                     <<"    " <<propertyDataMemberName(p()) <<".beforeResize([](int delta, " <<t <<" childPtr) {\n"
                     <<"        if (1 == delta)\n"
                     <<"            ASSERT_not_null2(childPtr, \"property cannot be set to null\");\n"
                     <<"    });\n"
                     <<"    " <<propertyDataMemberName(p()) <<".afterResize([this](int delta, " <<t <<" childPtr) {\n"
                     <<"        if (1 == delta) {\n"
                     <<"            " <<propertyDataMemberName(p()) <<".back().beforeChange([](" <<t <<", " <<t <<" childPtr) {\n"
                     <<"                ASSERT_not_null2(childPtr, \"property cannot be set to null\");\n"
                     <<"            });\n"
                     <<"        }\n"
                     <<"    });\n";
            } else if (isTreeScalarEdge(p())) {
                // The new value cannot be null
                impl <<"\n"
                     <<THIS_LOCATION <<locationDirective(p(), p->startToken)
                     <<"    // Tree edge cannot be set to null due to Rosebud::not_null attribute\n"
                     <<"    " <<propertyDataMemberName(p()) <<".beforeChange([](" <<t <<", " <<t <<" childPtr) {\n"
                     <<"        ASSERT_not_null2(childPtr, \"property cannot be set to null\");\n"
                     <<"    });\n";
            }
        }
    }
}

void
SawyerGenerator::genDefaultConstructor(std::ostream &header, std::ostream &impl, const Ast::Class::Ptr &c, Access access) {
    ASSERT_not_null(c);

    // C++ constructors are always generated
    Base::genDefaultConstructor(header, impl, c, Access::PROTECTED);

    // Static `instance` allocating constructor is sometimes generated
    if (!c->findAttribute("Rosebud::abstract") && !c->findAttribute("Rosebud::no_constructors")) {
        if (header) {
            header <<"\n"
                   <<THIS_LOCATION <<locationDirective(c, c->startToken)
                   <<toString(access) <<":\n"
                   <<"    // Automatically generated; do not modify!\n"
                   <<"    /** Default allocating constructor. */\n"
                   <<"    static Ptr instance();\n";
        }
        if (impl) {
            impl <<"\n"
                 <<THIS_LOCATION <<locationDirective(c, c->startToken)
                 <<"// Automatically generated; do not modify!\n"
                 <<"auto " <<c->name <<"::instance() -> Ptr {\n"
                 <<"    return Ptr(new " <<c->name <<");\n"
                 <<"}\n";
        }
    }
}

bool
SawyerGenerator::genArgsConstructor(std::ostream &header, std::ostream &impl, const Ast::Class::Ptr &c, const Hierarchy &h,
                                    Access access) {
    ASSERT_not_null(c);

    // The C++ constructors are always generated, but the static `instance` members on not always generated.
    if (Base::genArgsConstructor(header, impl, c, h, Access::PROTECTED)) {
        if (!c->findAttribute("Rosebud::abstract") && !c->findAttribute("Rosebud::no_constructors")) {
            const std::vector<Ast::Property::Ptr> args = allConstructorArguments(c, h);
            header <<"\n"
                   <<THIS_LOCATION <<locationDirective(c, c->startToken)
                   <<toString(access) <<":\n"
                   <<"    /** Allocating constructor. */\n"
                   <<"    static Ptr instance(";
            for (size_t i = 0; i <args.size(); ++i) {
                header <<(0 == i ? "" : ",\n                        ")
                       <<constRef(propertyMutatorArgumentType(args[i])) <<" " <<args[i]->name;
            }
            header <<");\n";

            impl <<"\n"
                 <<THIS_LOCATION <<locationDirective(c, c->startToken)
                 <<c->name <<"::Ptr\n"
                 <<c->name <<"::instance(";
            const auto indent = ",\n" + std::string(c->name.size() + 11, ' ');
            for (size_t i = 0; i <args.size(); ++i) {
                impl <<(0 == i ? "" : indent)
                     <<constRef(propertyMutatorArgumentType(args[i])) <<" " <<args[i]->name;
            }
            impl <<") {\n"
                 <<"    return Ptr(new " <<c->name <<"(";
            for (size_t i = 0; i < args.size(); ++i)
                impl <<(0 == i ? "" : ", ") <<args[i]->name;
            impl <<"));\n"
                 <<"}\n";
        }
        return true;
    } else {
        return false;
    }
}

void
SawyerGenerator::genClassConstructors(std::ostream &header, std::ostream &impl, const Ast::Class::Ptr &c, const Hierarchy &h) {
    ASSERT_not_null(c);

    header <<THIS_LOCATION <<locationDirective(c, c->nameToken)
           <<"\n"
           <<title("    Generated constructors, etc.");

    if (!genArgsConstructor(header, impl, c, h, Access::PUBLIC))
        genDefaultConstructor(header, impl, c, Access::PUBLIC);

    header <<"\n"
           <<THIS_LOCATION <<locationDirective(c, c->nameToken)
           <<"public:\n"
           <<"    ~" <<c->name <<"();\n";

    impl <<"\n"
         <<THIS_LOCATION <<locationDirective(c, c->nameToken)
         <<c->name <<"::~" <<c->name <<"() {}\n";
}

void
SawyerGenerator::genClass(const Ast::Class::Ptr &c, const Hierarchy &h) {
    ASSERT_not_null(c);

    if (c->findAttribute("rosebud::suppress"))
        return;

    auto file = notnull(c->findAncestor<Ast::File>());
    const boost::filesystem::path headerName = generatedDir / toPath(c->name, ".h");
    const boost::filesystem::path implName = generatedDir / toPath(c->name, ".C");
    std::ofstream header(headerName.c_str());
    std::ofstream impl(implName.c_str());
    if (!header) {
        message(ERROR, "cannot open file for writing: \"" + headerName.string() + "\"");
        return;
    }
    if (!impl) {
        message(ERROR, "cannot open file for writing: \"" + implName.string() + "\"");
        return;
    }

    // Emit stuff before the beginning of the class
    genImplOpen(impl, c);
    genHeaderOpen(header, c);
    if (!c->priorText.empty()) {
        std::regex implRe("#\\s*(ifdef\\s+ROSE_IMPL|if\\s+defined\\s*\\(\\s*ROSE_IMPL\\s*\\))");
        const std::string cppImplSymbol = toCppSymbol(c->qualifiedNamespace + "::" + c->name) + "_IMPL";
        const std::string s = std::regex_replace(c->priorText, implRe, "#ifdef " + cppImplSymbol);

        header <<"\n"
               <<THIS_LOCATION <<locationDirective(c, c->priorTextToken)
               <<s <<"\n";
        impl <<"\n"
             <<THIS_LOCATION <<"#define " <<cppImplSymbol <<"\n"
             <<"#include <" <<toPath(c->qualifiedNamespace + "::" + c->name, ".h").string() <<">\n"
             <<locationDirective(c, c->priorTextToken)
             <<s <<"\n";
    }

    // Class documentation
    if (!c->doc.empty()) {
        header <<"\n"
               <<THIS_LOCATION <<locationDirective(c, c->docToken)
               <<c->doc;
    }

    // Class opening line: "class NAME [: BASES] {"
    header <<"\n"
           <<locationDirective(c, c->nameToken)
           <<"class " <<c->name;
    if (c->inheritance.empty()) {
        header <<": public Sawyer::Tree::Vertex<" <<c->name <<">";
    } else {
        for (auto base = c->inheritance.begin(); base != c->inheritance.end(); ++base)
            header <<(base == c->inheritance.begin() ? ": " : ", ") <<base->first <<" " <<base->second;
    }
    header <<" {\n";

    // Emit generated types at the start of the class
    header <<THIS_LOCATION <<locationDirective(c, c->startToken)
           <<"public:\n"
           <<"    // Automatically generated; do not modify!\n"
           <<"    /** Shared-ownership pointer to @ref " <<c->name <<".\n"
           <<"     *\n"
           <<"     *  @{ */\n"
           <<"    using Ptr = " <<c->name <<"Ptr;\n"
           <<"    using ConstPtr = " <<c->name <<"ConstPtr;\n"
           <<"    /** @} */\n";

    // Properties
    for (const auto &p: c->properties)
        genProperty(header, impl, p());

    // User-defined class members
    if (!c->endText.empty())
        header <<"\n"
               <<THIS_LOCATION <<locationDirective(c, c->endTextToken)
               <<c->endText;

    // Constructors and destructors
    genClassConstructors(header, impl, c, h);

    // Emit stuff after the class
    header <<"};\n";
    if (!file->endText.empty()) {
        header <<"\n"
               <<THIS_LOCATION <<locationDirective(file, file->endTextToken)
               <<file->endText <<"\n";
        impl <<"\n"
             <<THIS_LOCATION <<locationDirective(file, file->endTextToken)
             <<file->endText <<"\n";
    }
    genHeaderClose(header, c);
    genImplClose(impl, c);
}

void
SawyerGenerator::genNodeDeclarations(const Ast::Project::Ptr &project) {
    ASSERT_not_null(project);

    const auto headerName = generatedDir / "NodeDeclarations.h";
    std::ofstream header(headerName.c_str());
    if (!header) {
        message(ERROR, "cannot open file for writing: \"" + headerName.string() + "\"");
        return;
    }

    const std::string onceSymbol = "ROSE_" + toCppSymbol((relativeToRoseSource(generatedDir) / "NodeDeclarations").string()) + "_H";
    header <<THIS_LOCATION <<machineGenerated()
           <<generatedByRosebud("//")
           <<"#ifndef " <<onceSymbol <<"\n"
           <<"#define " <<onceSymbol <<"\n"
           <<"\n"
           <<"#include <Sawyer/Tree.h>\n"
           <<"\n";

    Classes classes = project->allClassesFileOrder();
    std::sort(classes.begin(), classes.end(), [](const Ast::Class::Ptr &a, const Ast::Class::Ptr &b) {
        return a->qualifiedNamespace < b->qualifiedNamespace ||
            (a->qualifiedNamespace == b->qualifiedNamespace && a->name < b->name);
    });

    std::string prevNamespace;
    for (const auto &c: classes) {
        if (!c->findAttribute("Rosebud::suppress")) {
            if (c->qualifiedNamespace != prevNamespace) {
                if (!prevNamespace.empty())
                    genNamespaceDeclarationClose(header, prevNamespace);
                if (!c->qualifiedNamespace.empty())
                    genNamespaceDeclarationOpen(header, c->qualifiedNamespace);
                prevNamespace = c->qualifiedNamespace;
            }


            header <<"\n"
                   <<THIS_LOCATION <<locationDirective(c, c->startToken)
                   <<"class " <<c->name <<";\n"
                   <<"\n"
                   <<THIS_LOCATION <<locationDirective(c, c->startToken)
                   <<"/** Shared-ownership pointer to @ref " <<c->name <<"\n"
                   <<" *\n"
                   <<" * @{ */\n"
                   <<"using " <<c->name <<"Ptr = std::shared_ptr<" <<c->name <<">;\n"
                   <<"using " <<c->name <<"ConstPtr = std::shared_ptr<const " <<c->name <<">;\n"
                   <<"/** @} */\n";
        }
    }

    if (!prevNamespace.empty())
        genNamespaceDeclarationClose(header, prevNamespace);

    header <<"\n"
           <<THIS_LOCATION <<"#endif // " <<onceSymbol <<"\n"
           <<machineGenerated();
}

void
SawyerGenerator::genNamespaceDeclarationOpen(std::ostream &out, const std::string &ns) {
    static std::regex sep("::");
    for (std::sregex_token_iterator word(ns.begin(), ns.end(), sep, -1); word != std::sregex_token_iterator(); ++word)
        out <<"namespace " <<*word <<" {\n";
}

void
SawyerGenerator::genNamespaceDeclarationClose(std::ostream &out, const std::string &ns) {
    static std::regex sep("::");
    for (std::sregex_token_iterator word(ns.begin(), ns.end(), sep, -1); word != std::sregex_token_iterator(); ++word)
        out <<"} // namespace\n";
}

void
SawyerGenerator::genImplOpen(std::ostream &impl, const Ast::Class::Ptr &c) {
    ASSERT_not_null(c);
    impl <<THIS_LOCATION <<machineGenerated()
         <<generatedByRosebud("//");

    // FIXME[Robb Matzke 2024-02-09]: we want to eventually remove this since it takes a long time to compile
    impl <<"#include <sage3basic.h>\n";
}

void
SawyerGenerator::genImplClose(std::ostream &impl, const Ast::Class::Ptr&) {
    impl <<"\n"
         <<THIS_LOCATION <<machineGenerated();
}

void
SawyerGenerator::genHeaderOpen(std::ostream &header, const Ast::Class::Ptr &c) {
    ASSERT_not_null(c);
    const std::string once = toCppSymbol(c->qualifiedNamespace + "::" + c->name) + "_H";
    header <<THIS_LOCATION <<machineGenerated()
           <<generatedByRosebud("//")
           <<"#ifndef " <<once <<"\n"
           <<"#define " <<once <<"\n";
}

void
SawyerGenerator::genHeaderClose(std::ostream &header, const Ast::Class::Ptr &c) {
    ASSERT_not_null(c);
    header <<"\n"
           <<THIS_LOCATION <<"#endif // include once\n"
           <<machineGenerated();
}

// List of file names that hold the implementations for vairous things we generated in this backend, sorted by file name.
std::vector<std::string>
SawyerGenerator::implementationFileNames(const Classes &classes) {
    std::vector<std::string> names;
    names.reserve(classes.size());
    for (const auto &c: classes) {
        if (!c->findAttribute("Rosebud::suppress"))
            names.push_back(c->name + ".C");
    }
    std::sort(names.begin(), names.end());
    return names;
}

// List of generated header files
std::vector<std::string>
SawyerGenerator::headerFileNames(const Classes &classes) {
    std::vector<std::string> names;
    names.reserve(classes.size());
    for (const auto &c: classes) {
        if (!c->findAttribute("Rosebud::suppress"))
            names.push_back(c->name + ".h");
    }
    std::sort(names.begin(), names.end());
    return names;
}

void
SawyerGenerator::genTupFile(const std::vector<std::string> &implFileNames, const std::vector<std::string> &hdrFileNames) {
    if (!generatedDir.empty()) {
        const boost::filesystem::path outputName = generatedDir / "Tupfile";
        std::ofstream out(outputName.c_str());
        if (!out) {
            message(ERROR, "cannot create file: \"" + outputName.string() + "\"");
            return;
        }

        out <<THIS_LOCATION <<machineGenerated('#')
            <<generatedByRosebud("#")
            <<"\n"
            <<"include_rules\n"
            <<"\n";

        if (implFileNames.empty()) {
            // Create at least one stub file, otherwise some problems might arise building the intermediate library.
            std::ofstream stub((generatedDir / "stub.C").c_str());
            stub <<THIS_LOCATION <<"static void stub() {}\n";
            out <<"run $(librose_compile) \\\n"
                <<" \\\n    stub.C";
        } else {
            const auto prefix = boost::filesystem::path("include") / headerPrefix;
            out <<"run $(librose_compile)";
            for (const std::string &fileName: implFileNames)
                out <<" \\\n    " <<fileName;
            out <<"\n\n"
                <<"run $(public_header) -o " <<prefix.string();
            for (const std::string &fileName: hdrFileNames)
                out <<" \\\n    " <<fileName;
        }

        out <<"\n\n"
            <<THIS_LOCATION <<machineGenerated('#');
    }
}

void
SawyerGenerator::genMakeFile(const std::vector<std::string> &implFileNames, const std::vector<std::string> &hdrFileNames) {
    if (!generatedDir.empty()) {
        const boost::filesystem::path outputName = generatedDir / "Makefile.am";
        std::ofstream out(outputName.c_str());
        if (!out) {
            message(ERROR, "cannot create file: \"" + outputName.string() + "\"");
            return;
        }

        const std::string libraryTarget = "lib" + pascalCase(toCppSymbol(relativeToRoseSource(generatedDir).string()));

        out <<THIS_LOCATION <<machineGenerated('#')
            <<generatedByRosebud("#")
            <<"\n"
            <<"include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs\n"
            <<"\n"
            <<"AM_CPPFLAGS = $(ROSE_INCLUDES)\n"
            <<"noinst_LTLIBRARIES = " <<libraryTarget <<".la\n"
            <<libraryTarget <<"_la_SOURCES =";

        if (implFileNames.empty()) {
            // Create at least one stub file, otherwise some problems might arise building the intermediate library.
            std::ofstream stub((generatedDir / "stub.C").c_str());
            stub <<THIS_LOCATION <<"static void stub() {}\n";
            out <<" \\\n    stub.C";
        } else {
            for (const std::string &fileName: implFileNames)
                out <<" \\\n        " <<fileName;
        }
        out <<"\n";


        if (!hdrFileNames.empty()) {
            const auto prefix = boost::filesystem::path("$(includedir)") / headerPrefix;
            out <<"\n"
                <<THIS_LOCATION <<libraryTarget <<"includedir = " <<prefix.string() <<"\n"
                <<libraryTarget <<"include_HEADERS =";
            for (const std::string &fileName: hdrFileNames)
                out <<" \\\n        " <<fileName;
            out <<"\n";
        }

        out <<"\n"
            <<THIS_LOCATION <<machineGenerated('#');
    }
}

void
SawyerGenerator::genCmakeFile(const std::vector<std::string> &implFileNames, const std::vector<std::string> &hdrFileNames) {
    if (!generatedDir.empty()) {
        const boost::filesystem::path outputName = generatedDir / "CMakeLists.txt";
        std::ofstream out(outputName.c_str());
        if (!out) {
            message(ERROR, "cannot create file: \"" + outputName.string() + "\"");
            return;
        }

        const std::string libraryTarget = "lib" + pascalCase(toCppSymbol(relativeToRoseSource(generatedDir).string()));

        out <<THIS_LOCATION <<machineGenerated('#')
            <<generatedByRosebud("#")
            <<"\n"
            <<"add_library(" <<libraryTarget <<" OBJECT";

        if (implFileNames.empty()) {
            // Create at least one stub file, otherwise some problems might arise building the intermediate library.
            std::ofstream stub((generatedDir / "stub.C").c_str());
            stub <<THIS_LOCATION <<"static void stub() {}\n";
            out <<"\n  stub.C";
        } else {
            for (const std::string &fileName: implFileNames)
                out <<"\n  " <<fileName;
        }
        out <<")\n";

        out <<"\n"
            <<THIS_LOCATION <<"add_dependencies(" <<libraryTarget <<" rosetta_generated)\n";

        if (!hdrFileNames.empty()) {
            const auto prefix = boost::filesystem::path("include") / headerPrefix;

            out <<"\n"
                <<THIS_LOCATION <<"install(FILES";
            for (const std::string &fileName: hdrFileNames)
                out <<"\n  " <<fileName;
            out <<"\n  DESTINATION " <<prefix.string() <<")\n";
        }

        out <<"\n"
            <<THIS_LOCATION <<machineGenerated('#');
    }
}

std::string
SawyerGenerator::propertyDataMemberName(const Ast::Property::Ptr &p) const {
    const std::string name = CxxGenerator::propertyDataMemberName(p);
    unsigned n = 0;
    for (const auto ch: name)
        n = ((n << 3) | ((n >> 29) & 0x7)) ^ static_cast<unsigned>(ch);
    return name + "P" + boost::lexical_cast<std::string>(n % 10000) + "_";
}

} // namespace
