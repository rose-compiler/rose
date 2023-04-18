#include <Rosebud/RosettaGenerator.h>

#include <Rosebud/BoostSerializer.h>
#include <Rosebud/Utility.h>

#include <Sawyer/StaticBuffer.h>

#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>

#include <iostream>
#include <fstream>

using namespace Sawyer::Message::Common;

namespace Rosebud {

void
RosettaGenerator::adjustParser(Sawyer::CommandLine::Parser &parser) {
    using namespace Sawyer::CommandLine;

    SwitchGroup sg("ROSETTA backend for IR/AST nodes (--backend=rosetta)");
    sg.name("rosetta");
    sg.doc("The ultimate goal is to remove the legacy ROSETTA system from ROSE and replace its monolithic features with "
           "small, simple, specialized code generators each serving a very specific and well defined purpose, and each having "
           "a dedicated ROSE team member as its responsible maintainer. However, since ROSETTA is large (more than 100k LOC), "
           "we cannot convert everything all at once. Also, since rosebud is still experimental, this backend will generate a "
           "number of things, including:"

           "@bullet{The C++ source file(s) that goes into building ROSETTA's CxxGrammarMetaProgram code generator. This backend "
           "uses formatting developed for binary analysis that causes the ROSETTA source code, the Doxygen documentation, and "
           "the user-defined class members to be emitted all to a single C++ file.}"

           "@bullet{The C++ implementation files for Rosebud-generated functons whose declarations are passed through ROSETTA "
           "unchanged. This backend emits one file per class because we've found that having a half-million line C++ source "
           "file makes certain things challenging, such as debugging with GDB.}"

           "@bullet{Files for build systems. For now, we are running Rosebud off-line outside the build system. This means "
           "that the Rosebud-generated files are actually checked into the source tree and thus we need the build system "
           "configuration files that go along with them. We plan to change this in the future.}"

           "The following command-line switches are understood by this backend:");

    sg.insert(Switch("output")
              .argument("file_name", anyParser(rosettaFileName))
              .doc("Name of the C++ file that is part of the source code for ROSETTA's CxxGrammarMetaProgram tool. If this switch "
                   "is not specified, then the ROSETTA output is not produced."));

    sg.insert(Switch("impl")
              .argument("directory", anyParser(implDirectoryName))
              .doc("Name of an existing directory into which the C++ implementation files are written. If this switch is not "
                   "specified, then the implementation files are not produced."));

    sg.insert(Switch("node-list")
              .argument("file_name", anyParser(nodeListFileName))
              .doc("Name of the ROSETTA input file that contains the names of all the node types, one per line. This file will "
                   "be modified in place by appending the name of any node type that doesn't already exist in that file."));

    sg.insert(Switch("function")
              .argument("name", anyParser(grammarFunctionName))
              .doc("Name of the ROSETTA function being generated. The default is \"" + grammarFunctionName + "\"."));

    sg.insert(Switch("cpp-protection")
              .argument("expression", anyParser(cppProtection))
              .doc("C preprocessor conditional compilation expression that will appear in an \"#if\" directive that protects "
                   "almost all generated code. The default is " +
                   std::string(cppProtection.empty() ? "nothing" : ("\"" + cppProtection + "\"")) + "."));

    sg.insert(Switch("strict-parents")
              .intrinsicValue(true, strictParents)
              .doc("When inserting a child into the tree, check that the child's parent is not already set to some other node "
                   "and then set the child's parent pointer to point to the new parent. The @s{no-strict-parents} switch turns "
                   "this off. The default is to " + std::string(strictParents ? "" : "not ") + "perform these checks and "
                   "adjustments."));
    sg.insert(Switch("no-strict-parents")
              .intrinsicValue(false, strictParents)
              .key("strict-parents")
              .hidden(true));

    parser.with(sg);
}

// Short name for a class
std::string
RosettaGenerator::shortName(const Ast::Class::Ptr &c) {
    ASSERT_not_null(c);

    if (c->name.size() >= 3 && boost::starts_with(c->name, "Sg")) {
        return c->name.substr(2);
    } else {
        static std::set<std::string> seen;
        if (seen.insert(c->name).second) {
            message(ERROR, c->findAncestor<Ast::File>(), c->nameToken,
                    "ROSETTA expects all AST type names to start with \"Sg\" and contain at least three characters");
        }
        return "invalid_short_name_for_" + c->name;
    }
}

// Given the name of a file, return a name that's relative to the top of the ROSE source tree. If not possible, then emit an error
// and return an error file name.
boost::filesystem::path
RosettaGenerator::fileWrtRose(const boost::filesystem::path &fileName) {
    boost::filesystem::path retval = relativeToRoseSource(fileName);
    if (retval.empty()) {
        static size_t nCalls;
        if (1 == ++nCalls)
            message(ERROR, "cannot find root of ROSE source tree from \"" + fileName.string() + "\"");
        return "ERROR_NOT_IN_ROSE_" + fileName.string();
    } else {
        return retval;
    }
}

// Output the beginning of the binaryInstruction.C file.
void
RosettaGenerator::genRosettaFileBegin(std::ostream &rosetta) {
    const std::string codeFile = fileWrtRose(rosettaFileName).string();
    rosetta <<THIS_LOCATION <<machineGenerated()
            << "//\n"
            <<"// This file was generated with ROSE's \"rosebud\" tool by reading node definitions written in a\n"
            <<"// C++-like language and emitting this ROSETTA input.\n"
            <<"//\n"
            <<makeTitleComment("DO NOT MODIFY THIS FILE MANUALLY!", "", '/', outputWidth)
            <<"\n"
            <<"\n"
            <<THIS_LOCATION <<"#include <featureTests.h>\n";
    if (!cppProtection.empty())
        rosetta <<"#if " <<cppProtection <<"\n";
    rosetta <<THIS_LOCATION <<"#include \"ROSETTA_macros.h\"\n"
            <<"#include \"grammar.h\"\n"
            <<"#include \"AstNodeClass.h\"\n"
            <<"\n"
            <<"//#undef DOCUMENTATION -- commented out so IDEs can't figure it out\n"
            <<"#ifdef DOCUMENTATION\n"
            <<"DOCUMENTATION_should_never_be_defined;\n"
            <<"#endif\n"
            <<"\n"
            <<"#ifdef DOCUMENTATION\n"
            <<"#define DECLARE_LEAF_CLASS(CLASS_WITHOUT_Sg) /*void*/\n"
            <<"#else\n"
            <<"#define DECLARE_LEAF_CLASS(CLASS_WITHOUT_Sg) \\\n"
            <<"    NEW_TERMINAL_MACRO(CLASS_WITHOUT_Sg, #CLASS_WITHOUT_Sg, #CLASS_WITHOUT_Sg \"Tag\"); \\\n"
            <<"    CLASS_WITHOUT_Sg.setCppCondition(\"!defined(DOCUMENTATION)\");\\\n"
            <<"    CLASS_WITHOUT_Sg.setAutomaticGenerationOfConstructor(false);\\\n"
            <<"    CLASS_WITHOUT_Sg.setAutomaticGenerationOfDestructor(false)\n"
            <<"#endif\n"
            <<"\n"
            <<"#ifdef DOCUMENTATION\n"
            <<"#define DECLARE_HEADERS(CLASS_WITHOUT_Sg) /*void*/\n"
            <<"#else\n"
            <<"#define DECLARE_HEADERS(CLASS_WITHOUT_Sg) \\\n"
            <<"    CLASS_WITHOUT_Sg.setPredeclarationString(\"Sg\" #CLASS_WITHOUT_Sg \"_HEADERS\", \\\n"
            <<"                          ROSE_AUTOMAKE_ABSOLUTE_PATH_TOP_SRCDIR + \"/" + codeFile + "\")\n"
            <<"#endif\n"
            <<"\n"
            <<"#ifdef DOCUMENTATION\n"
            <<"#define DECLARE_OTHERS(CLASS_WITHOUT_Sg) /*void*/\n"
            <<"#else\n"
            <<"#define DECLARE_OTHERS(CLASS_WITHOUT_Sg) \\\n"
            <<"    CLASS_WITHOUT_Sg.setFunctionPrototype(\"Sg\" #CLASS_WITHOUT_Sg \"_OTHERS\", \\\n"
            <<"                          ROSE_AUTOMAKE_ABSOLUTE_PATH_TOP_SRCDIR + \"/" + codeFile + "\")\n"
            <<"#endif\n"
            <<"\n"
            <<"#ifdef DOCUMENTATION\n"
            <<"#define IS_SERIALIZABLE() /*void*/\n"
            <<"#else\n"
            <<"#define IS_SERIALIZABLE(CLASS_WITHOUT_Sg) \\\n"
            <<"    CLASS_WITHOUT_Sg.isBoostSerializable(true)\n"
            <<"#endif\n"
            <<"\n";
}

void
RosettaGenerator::genRosettaFileEnd(std::ostream &rosetta) {
    if (!cppProtection.empty())
        rosetta <<THIS_LOCATION <<"#endif // " <<cppProtection <<"\n";
}

void
RosettaGenerator::genRosettaFunctionBegin(std::ostream &rosetta) {
    rosetta <<"\n"
            <<THIS_LOCATION <<"#ifndef DOCUMENTATION\n"
            <<"void " <<grammarFunctionName <<"() {\n"
            <<"#endif // !DOCUMENTATION\n";
}

void
RosettaGenerator::genRosettaFunctionEnd(std::ostream &rosetta) {
    rosetta <<"\n"
            <<THIS_LOCATION <<"#ifndef DOCUMENTATION\n"
            <<"} // Grammar::setUpBinaryInstruction\n"
            <<"#endif // !DOCUMENTATION\n";
}

void
RosettaGenerator::genImplFileBegin(std::ostream &impl, const Ast::Class::Ptr &c) {
    ASSERT_not_null(c);

    impl <<THIS_LOCATION <<makeTitleComment("Implementation for " + c->name + "            -- MACHINE GENERATED; DO NOT MODIFY --",
                            "", '/', outputWidth)
         <<"\n"
         <<THIS_LOCATION <<"#include <featureTests.h>\n";
    if (!cppProtection.empty())
        impl <<"#if " <<cppProtection <<"\n";

    // The CPP conditional compilation directives that appeared before the class definition in the Rosebud input need to also be in
    // effect in this implementation file. However, we don't need to include any files or define any macros because those would have
    // been emitted in the header file for this node (actually, in the monstrous Cxx_Grammar.h file, but we'll fix that later and
    // for now we'll include it by including <sage3basic.h> just like we do per policy in all other librose source files).
    c->cppStack->emitOpen(impl);
    impl <<THIS_LOCATION <<"#include <sage3basic.h>\n";
}

void
RosettaGenerator::genImplFileEnd(std::ostream &impl, const Ast::Class::Ptr &c) {
    ASSERT_not_null(c);
    impl <<"\n";
    c->cppStack->emitClose(impl);
    if (!cppProtection.empty())
        impl <<THIS_LOCATION <<"#endif // " <<cppProtection <<"\n";
}

// Output declarations for all classes used as base classes so Doxygen is sure to see them.
void
RosettaGenerator::genClassDeclarations(std::ostream &rosetta, const Classes &classes) {
    std::set<std::string> bases;
    for (const auto &c: classes) {
        for (const auto &super: c->inheritance)
            bases.insert(super.second);
    }

    rosetta <<"\n"
            <<THIS_LOCATION <<"// Since ROSETTA builds classes from the leaves up to the base, and C++ builds classes from the\n"
            <<"// base down to the leaves, we need to make sure that doxygen sees the base classes before the derived classes. So\n"
            <<"// just list all the non-leaf classes here.\n"
            <<"#ifdef DOCUMENTATION\n";
    for (const std::string &name: bases)
        rosetta <<THIS_LOCATION <<"class " <<name <<";\n";
    rosetta <<THIS_LOCATION <<"#endif // DOCUMENTATION\n";
}

// Output the "class NAME: SUPER {" line
void
RosettaGenerator::genClassBegin(std::ostream &rosetta, const Ast::Class::Ptr &c) {
    ASSERT_not_null(c);

    rosetta <<"\n"
            <<THIS_LOCATION <<"#ifdef DOCUMENTATION\n";

    // Documentation
    if (!c->doc.empty())
        rosetta <<locationDirective(c, c->docToken) <<c->doc <<"\n";

    // Emit 'class' NAME (':' VISIBILITY SUPERNAME (',' VISIBILITY SUPERNAME)*)?
    rosetta <<locationDirective(c, c->startToken) <<"class " <<c->name;
    for (size_t i = 0; i < c->inheritance.size(); ++i)
        rosetta <<(0 == i ? ": " : ", ") <<c->inheritance[i].first <<" " <<c->inheritance[i].second;

    rosetta <<" {\n"
            <<"#endif // DOCUMENTATION\n";
}

void
RosettaGenerator::genClassEnd(std::ostream &rosetta, const Ast::Class::Ptr&) {
    rosetta <<THIS_LOCATION <<"#ifdef DOCUMENTATION\n"
            <<"};\n"
            <<"#endif // DOCUMENTATION\n";
}

// Return the first super class whose name starts with "Sg" and contains at least three characters.
std::string
RosettaGenerator::rosettaBaseClass(const Ast::Class::Ptr &c) {
    ASSERT_not_null(c);
    for (const auto &super: c->inheritance) {
        if (super.second.size() >= 3 && boost::starts_with(super.second, "Sg"))
            return super.second;
    }
    return "";
}

// Emit the constructors for the class.
void
RosettaGenerator::genClassConstructors(std::ostream &header, std::ostream &impl, const Ast::Class::Ptr &c, const Hierarchy &h) {
    ASSERT_not_null(c);
    Access access = c->findAttribute("Rosebud::abstract") ? Access::PROTECTED : Access::PUBLIC;
    genDefaultConstructor(header, impl, c, access);
    genArgsConstructor(header, impl, c, h, access);
}

// Generate property data member
void
RosettaGenerator::genPropertyDataMember(std::ostream &rosetta, std::ostream &header, const Ast::Property::Ptr &p) {
    ASSERT_not_null(p);
    auto c = p->findAncestor<Ast::Class>();
    ASSERT_not_null(c);

    // Data memeber.
    //   NO_CONSTRUCTOR_PARAMETER.
    //     This is not used because we've turned off ROSETTA's automatic generation of constructors and destructors.
    //   NO_ACCESS_FUNCTIONS | BUILD_ACCESS_FUNCTIONS
    //     We disable ROSETTA's automatic generation of data member access and mutator functions because we can easily generate our
    //     own and have more control over them. For instance, ROSETTA distinguishes between passing arguments and return values by
    //     value (BUILD_ACCESS_FUNCTIONS) or reference (BUILD_LIST_ACCESS_FUNCTIONS), which is unecessary since passing by reference
    //     is more general than passing by value (therefore Rosebud always uses references). Rosebud also has more flexibility in
    //     choosing the accessor and mutator names, using either ROSETTA's "get_"/"set_" prefixes or just the property name by
    //     itself.
    //   NO_TRAVERSAL | DEF_TRAVERSAL
    //     The common case is NO_TRAVERSAL; the uncommon case is caused when the Rosebud::traverse attribute is present.
    //   NO_DELETE
    //     This controls whether ROSETTA's generated destructors delete data members. However, it's always possible to define
    //     data members in such a way that they get deleted automatically by the default destructor, so Rosetta always uses
    //     NO_DELETE, does not generate `delete` calls in the destructur, and leaves it up to the node authors. For instance,
    //     if a node points to something that should be deleted, then the type for that member should be a smart pointer that
    //     indicates that the node owns the pointed-to data. Furthermore, we've disabled ROSETTA's generation of destructors.
    //   COPY_DATA
    //     This controls whether ROSETTA's generated copy mechanism (not copy constructors) copy the data member's value from
    //     the source node to the destination node. We assume that all data members should be copied.
    if (p->findAttribute("Rosebud::rosetta")) {
        rosetta <<"\n"
                <<THIS_LOCATION <<"#ifndef DOCUMENTATION\n";
        rosetta <<locationDirective(p, p->startToken) <<"    " <<shortName(c) <<".setDataPrototype(\n"
                <<"        \"" <<dataMemberType(p) <<"\", \"" <<p->name <<"\", \"";
        const std::string initExpr = ctorInitializerExpression(p, initialValue(p));
        if (!initExpr.empty())
            rosetta <<"= " <<initExpr;          // rosetta wants contructor initializer expressions even though we have an "=" here
        rosetta <<"\",\n"
                <<"        NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, "
                <<(p->findAttribute("Rosebud::traverse") ? "DEF_TRAVERSAL" : "NO_TRAVERSAL") <<", "
                <<"NO_DELETE, COPY_DATA);\n"
                <<THIS_LOCATION <<"#endif // !DOCUMENTATION\n";
    } else {
        header <<"\n"
               <<THIS_LOCATION <<"private:\n"
               <<"    " <<dataMemberType(p) <<" " <<propertyDataMemberName(p) <<";\n";
    }
}

void
RosettaGenerator::genPropertyAccessors(std::ostream &header, std::ostream &impl, const Ast::Property::Ptr &p) {
    ASSERT_not_null(p);
    auto c = p->findAncestor<Ast::Class>();
    ASSERT_not_null(c);

    // Accessor functions declarations
    for (const std::string &accessorName: propertyAccessorNames(p)) {
        header <<THIS_LOCATION <<"    " <<constRef(removeVolatileMutable(dataMemberType(p))) <<" " <<accessorName <<"() const;\n";
        if (p->findAttribute("Rosebud::large"))
            header <<THIS_LOCATION <<"    " <<removeVolatileMutable(dataMemberType(p)) <<"& " <<accessorName <<"();\n";
    }

    // Accessor function implementations
    for (const std::string &accessorName: propertyAccessorNames(p)) {
        impl <<"\n"
             <<THIS_LOCATION <<constRef(removeVolatileMutable(dataMemberType(p))) <<"\n"
             <<c->name <<"::" <<accessorName <<"() const {\n"
             <<"    return " <<propertyDataMemberName(p) <<";\n"
             <<"}\n";
        if (p->findAttribute("Rosebud::large")) {
            impl <<"\n"
                 <<THIS_LOCATION <<removeVolatileMutable(dataMemberType(p)) <<"&\n"
                 <<c->name <<"::" <<accessorName <<"() {\n"
                 <<"    return " <<propertyDataMemberName(p) <<";\n"
                 <<"}\n";
        }
    }
}

void
RosettaGenerator::genPropertyMutators(std::ostream &header, std::ostream &impl, const Ast::Property::Ptr &p) {
    ASSERT_not_null(p);
    auto c = p->findAncestor<Ast::Class>();
    ASSERT_not_null(c);

    // Mutator function declarations
    for (const std::string &mutatorName: propertyMutatorNames(p))
        header <<THIS_LOCATION <<"    void " <<mutatorName <<"(" <<constRef(valueType(p)) <<");\n";

    // Mutator function implementations
    for (const std::string &mutatorName: propertyMutatorNames(p)) {
        impl <<"\n"
             <<THIS_LOCATION <<"void\n"
             <<c->name <<"::" <<mutatorName <<"(" <<constRef(valueType(p)) <<" x) {\n";

        if (strictParents && p->findAttribute("Rosebud::traverse")) {
            impl <<"    changeChildPointer(this->" <<propertyDataMemberName(p) <<", const_cast<" <<valueType(p) <<"&>(x));\n";
        } else {
            impl <<"    this->" <<propertyDataMemberName(p) <<" = x;\n";
        }

        impl <<"    set_isModified(true);\n"
             <<"}\n";
    }
}

// Generate code for a property. The output streams are:
//   * rosetta: the stuff that will be compiled into ROSETTA's CxxGrammarMetaProgram.
//   * header:  the stuff to pass through ROSETTA directly into the class definition header file.
//   * impl:    the C++ implementation file for one or more nodes.
void
RosettaGenerator::genProperty(std::ostream &rosetta, std::ostream &header, std::ostream &impl, const Ast::Property::Ptr &p) {
    ASSERT_not_null(p);
    auto c = p->findAncestor<Ast::Class>();
    ASSERT_not_null(c);

    p->cppStack->emitOpen(rosetta);

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

    genPropertyDataMember(rosetta, header, p);

    // Accessors and mutators
    header <<"\n"
           <<THIS_LOCATION <<"public:\n"
           <<locationDirective(p, p->docToken) <<doc.first;
    genPropertyAccessors(header, impl, p);
    genPropertyMutators(header, impl, p);
    header <<doc.second;

    p->cppStack->emitClose(rosetta);

}

void
RosettaGenerator::genOtherContent(std::ostream &rosetta, const Ast::Class::Ptr &c, const std::string &content) {
    ASSERT_not_null(c);

    if (!content.empty()) {
        rosetta <<"\n"
                <<THIS_LOCATION <<"    DECLARE_OTHERS(" <<shortName(c) <<");\n"
                <<"#if defined(" <<c->name <<"_OTHERS) || defined(DOCUMENTATION)\n";
        BoostSerializer().generate(rosetta, rosetta, c, *this);
        rosetta <<content
                <<"#endif // " <<c->name <<"_OTHERS\n";
    }
}

// Emit call to NEW_NONTERMINAL_MACRO macro
void
RosettaGenerator::genNewNonterminalMacro(std::ostream &rosetta, const Ast::Class::Ptr &c, const Hierarchy &h) {
    ASSERT_not_null(c);

    // We would normally use the NEW_NONTERMINAL_MACRO macro defined in ROSETTA_macros.h here, but that macro makes it difficult to
    // handle a list of subclass names where some of the subclasses are conditionally compiled. Especially when the first one is
    // conditionally compiled.
    rosetta <<THIS_LOCATION <<"AstNodeClass& " <<shortName(c) <<" = nonTerminalConstructor(\n"
            <<"    \"" <<shortName(c) <<"\",\n"
            <<"    *this,\n"
            <<"    \"" <<shortName(c) <<"\",\n"
            <<"    \"" <<shortName(c) <<"Tag\",\n"
            <<"    SubclassListBuilder()\n";

    auto vertex = h.findVertexKey(c);
    ASSERT_require(vertex != h.vertices().end());
    for (auto subclassEdge: vertex->outEdges()) {
        Ast::Class::Ptr subclass = subclassEdge.target()->value();
        subclass->cppStack->emitOpen(rosetta);
        rosetta <<"        | " <<shortName(subclass) <<"\n";
        subclass->cppStack->emitClose(rosetta);
    }
    if (c->findAttribute("Rosebud::abstract")) {
        rosetta <<"    , false);\n";
    } else {
        rosetta <<"    , true);\n";
    }

    rosetta <<THIS_LOCATION <<"assert(" <<shortName(c) <<".associatedGrammar != nullptr);\n";
}

void
RosettaGenerator::genNonterminalMacros(std::ostream &rosetta, const Ast::Class::Ptr &c, const Hierarchy &h) {
    ASSERT_not_null(c);

    rosetta <<THIS_LOCATION <<"#ifndef DOCUMENTATION\n";
    genNewNonterminalMacro(rosetta, c, h);
    rosetta <<THIS_LOCATION <<shortName(c) <<".setCppCondition(\"!defined(DOCUMENTATION)\");\n"
            <<shortName(c) <<".isBoostSerializable(true);\n"
            <<shortName(c) <<".setAutomaticGenerationOfConstructor(false);\n"
            <<shortName(c) <<".setAutomaticGenerationOfDestructor(false);\n"
            <<"#endif // !DOCUMENTATION\n";
}

void
RosettaGenerator::genLeafMacros(std::ostream &rosetta, const Ast::Class::Ptr &c) {
    ASSERT_not_null(c);

    rosetta <<THIS_LOCATION <<"DECLARE_LEAF_CLASS(" <<shortName(c) <<");\n"
            <<"IS_SERIALIZABLE(" <<shortName(c) <<");\n";
}

void
RosettaGenerator::genClassDefinition(std::ostream &rosetta, const Ast::Class::Ptr &c, const Hierarchy &h) {
    ASSERT_not_null(c);

    std::ostringstream header;                          // Non-ROSETTA class definition stuff

    // Open the output stream for the generated function implementations
    const std::string implFileName = implDirectoryName.empty() ?
                                     "/dev/null" :
                                     (implDirectoryName / (c->name + ".C")).string();
    ASSERT_forbid(implFileName.empty());
    std::ofstream impl(implFileName, std::ios_base::trunc);
    if (!impl) {
        message(ERROR, "unable to open implementation output file \"" + std::string(implFileName) + "\"");
        return;
    }
    genImplFileBegin(impl, c);

    // Title comment
    rosetta <<"\n\n"
            <<THIS_LOCATION <<makeTitleComment(c->name + "           -- MACHINE GENERATED; DO NOT MODIFY --", "", '/', outputWidth)
            <<"\n";

    // Emit the conditional compilation for the class as a whole
    c->cppStack->emitOpen(rosetta);

    // Emit the ROSETTA-specific macros for the class
    if (isBaseClass(c, h)) {
        genNonterminalMacros(rosetta, c, h);
    } else {
        genLeafMacros(rosetta, c);
    }

    // ROSETTA "headers" for the class. This is usually just #include statements, but it can be anything
    // needed before the ROSETTA-generated class definition.
    if (!c->priorText.empty()) {
        rosetta <<"\n"
                <<THIS_LOCATION <<"DECLARE_HEADERS(" <<shortName(c) <<");\n"
                <<"#if defined(" <<c->name <<"_HEADERS) || defined(DOCUMENTATION)\n"
                <<locationDirective(c, c->priorTextToken) <<c->priorText <<"\n";
        c->cppStack->emitClose(rosetta);
        rosetta <<"#endif // " <<c->name <<"_HEADERS\n";
    }

    genClassBegin(rosetta, c);

    for (const auto &p: c->properties) {
        // Stuff in the input that's prior to the property definition should go in the "other" section of output
        header <<locationDirective(p(), p->priorTextToken) <<p->priorText;

        // Emit the property definition, and accumulate anything that should go in the "other" section of output.
        genProperty(rosetta, header, impl, p());
    }

    // If anything else is in the input class definition after the last property definition, append it to the "other" section of
    // output
    header <<locationDirective(c, c->endTextToken) <<c->endText;

    // Class constructors and destructures emitted quite late in the class definition so we're sure that all the types needed by
    // their arguments are already emitted.
    genDestructor(header, impl, c);
    genClassConstructors(header, impl, c, h);
    genInitProperties(header, impl, c);

    // ROSETTA "others" is everything else in the class that's not generated by making ROSETTA function calls.
    genOtherContent(rosetta, c, header.str());
    genClassEnd(rosetta, c);

    c->cppStack->emitClose(rosetta);
    genImplFileEnd(impl, c);
}

std::string
RosettaGenerator::propertyDataMemberName(const Ast::Property::Ptr &p) const {
    ASSERT_not_null(p);
    if (p->findAttribute("Rosebud::rosetta")) {
        return "p_" + p->name;
    } else {
        return Generator::propertyDataMemberName(p);
    }
}

std::vector<std::string>
RosettaGenerator::propertyAccessorNames(const Ast::Property::Ptr &p) const {
    ASSERT_not_null(p);
    if (!p->accessorNames && p->findAttribute("Rosebud::rosetta")) {
        return {"get_" + p->name};
    } else {
        return Generator::propertyAccessorNames(p);
    }
}

std::vector<std::string>
RosettaGenerator::propertyMutatorNames(const Ast::Property::Ptr &p) const {
    ASSERT_not_null(p);
    if (!p->mutatorNames && p->findAttribute("Rosebud::rosetta")) {
        return {"set_" + p->name};
    } else {
        return Generator::propertyMutatorNames(p);
    }
}

// List of file names that hold the implementations for vairous things we generated in this backend, sorted by file name.
std::vector<std::string>
RosettaGenerator::implementationFileNames(const Classes &classes) {
    std::vector<std::string> names;
    names.reserve(classes.size());
    for (const auto &c: classes)
        names.push_back(c->name + ".C");
    std::sort(names.begin(), names.end());
    return names;
}

void
RosettaGenerator::genTupFile(const std::vector<std::string> &implFileNames) {
    if (!implDirectoryName.empty()) {
        const boost::filesystem::path tupFileName = implDirectoryName / "Tupfile";
        std::ofstream out(tupFileName.c_str());
        if (!out) {
            message(ERROR, "cannot create file: \"" + tupFileName.string() + "\"");
            return;
        }

        out <<THIS_LOCATION <<machineGenerated('#')
            <<"#\n"
            <<"# This file was generated by Rosebud\n"
            <<"#\n"
            <<"\n"
            <<"include_rules\n"
            <<"ifeq (@(ENABLE_BINARY_ANALYSIS),yes)\n"
            <<"run $(librose_compile)";

        for (const std::string &fileName: implFileNames)
            out <<" \\\n    " <<fileName;

        out <<"\n"
            <<"endif\n";
    }
}

void
RosettaGenerator::genMakeFile(const std::vector<std::string> &implFileNames) {
    if (!implDirectoryName.empty()) {
        const boost::filesystem::path tupFileName = implDirectoryName / "Makefile.am";
        std::ofstream out(tupFileName.c_str());
        if (!out) {
            message(ERROR, "cannot create file: \"" + tupFileName.string() + "\"");
            return;
        }

        out <<THIS_LOCATION <<machineGenerated('#')
            <<"#\n"
            <<"# This file was generated by Rosebud\n"
            <<"#\n"
            <<"\n"
            <<"include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs\n"
            <<"\n"
            <<"AM_CPPFLAGS = $(ROSE_INCLUDES)\n"
            <<"noinst_LTLIBRARIES = libroseGenerated.la\n"
            <<"libroseGenerated_la_SOURCES =";

        for (const std::string &fileName: implFileNames)
            out <<" \\\n        " <<fileName;
        out <<"\n";
    }
}

void
RosettaGenerator::genCmakeFile(const std::vector<std::string> &implFileNames) {
    if (!implDirectoryName.empty()) {
        const boost::filesystem::path cmakeFileName = implDirectoryName / "CMakeLists.txt";
        std::ofstream out(cmakeFileName.c_str());
        if (!out) {
            message(ERROR, "cannot create file: \"" + cmakeFileName.string() + "\"");
            return;
        }

        out <<THIS_LOCATION <<machineGenerated('#')
            <<"#\n"
            <<"# This file was generated by Rosebud\n"
            <<"#\n"
            <<"\n"
            <<"add_library(roseGenerated OBJECT";
        for (const std::string &fileName: implFileNames)
            out <<"\n  " <<fileName;
        out <<")\n"
            <<"\n"
            <<"add_dependencies(roseGenerated rosetta_generated)\n";
    }
}

void
RosettaGenerator::adjustNodeList(const std::shared_ptr<Ast::Project> &project) {
    ASSERT_not_null(project);

    if (!nodeListFileName.empty()) {
        // Read the file. Each line is a class name.
        std::vector<std::string> names;
        {
            std::ifstream in(nodeListFileName.c_str());
            if (!in) {
                message(ERROR, "cannot read AST node type list from file \"" + nodeListFileName.string() + "\"");
                return;
            }

            std::string line;
            while (std::getline(in, line)) {
                boost::trim(line);
                names.push_back(line);
            }
        }

        // Append our own classes to the end if they don't exist yet
        bool changed = false;
        const Classes classes = project->allClassesFileOrder();
        for (const auto &c: classes) {
            if (std::find(names.begin(), names.end(), c->name) == names.end()) {
                names.push_back(c->name);
                changed = true;
            }
        }

        // Write the new stuff back to the file
        if (changed) {
            std::ofstream out(nodeListFileName.c_str());
            if (!out) {
                message(ERROR, "cannot write AST node type list to file \"" + nodeListFileName.string() + "\"");
                return;
            }
            for (const std::string &name: names)
                out <<name <<"\n";
        }
    }
}

void
RosettaGenerator::generate(const Ast::Project::Ptr &project) {
    ASSERT_not_null(project);

    Hierarchy h = classHierarchy(project->allClassesFileOrder());
    checkClassHierarchy(h);
    const Classes classes = bottomUp(h);

    // Check that we have all the classes we need
    for (const Ast::Class::Ptr &c: classes) {
        std::string sageBase;
        for (const auto &base: c->inheritance) {
            if (base.second.size() > 2 && boost::starts_with(base.second, "Sg")) {
                if (!sageBase.empty()) {
                    message(ERROR, c->findAncestor<Ast::File>(), c->nameToken,
                            "class \"" + c->name + "\" has more than one Sage node as a base class: "
                            "\"" + sageBase + "\" and \"" + base.second + "\"");
                } else {
                    sageBase = base.second;
                }

                const auto baseVertex = h.findVertexKey(base.second);
                if (baseVertex == h.vertices().end()) {
                    // FIXME[Robb Matzke 2023-03-20]: points to the class name, but should point to the base class
                    // name. Unfortunately we didn't save that information when parsing.
                    if (settings.showingWarnings) {
                        message(WARN, c->findAncestor<Ast::File>(), c->nameToken,
                                "class \"" + c->name + "\" derives from class \"" + base.second + "\""
                                " whose definition is not known to Rosebud (did you intend to supply the base class's header file "
                                "on the rosebud command-line?)");
                    }
                }
            }
        }
    }

    // Open the stream to which we write ROSETTA input
    std::ofstream rosettaFile;
    if ("-" != rosettaFileName) {
#ifdef _MSC_VER
        if (rosettaFileName.empty()) {
            message(FATAL, "ROSETTA output file must be specified on Windows");
            exit(1);
        } else {
            rosettaFile.open(rosettaFileName.c_str());
        }
#else
        rosettaFile.open(rosettaFileName.empty() ? "/dev/null" : rosettaFileName.c_str());
#endif
    }
    std::ostream &rosetta = "-" == rosettaFileName ? std::cout : rosettaFile;
    if (!rosetta) {
        message(ERROR, "cannot open output file \"" + rosettaFileName.string() + "\"");
        return;
    }

    // Generate code
    genRosettaFileBegin(rosetta); {
        // ROSETTA files define classes from the bottom up because the base class definitions depend on the subclasses instead
        // of the other way around.
        genClassDeclarations(rosetta, classes);

        genRosettaFunctionBegin(rosetta); {
            for (auto c: classes)
                genClassDefinition(rosetta, c, h);
        } genRosettaFunctionEnd(rosetta);
    } genRosettaFileEnd(rosetta);

    // Generate build system files
    const std::vector<std::string> implFileNames = implementationFileNames(classes);
    adjustNodeList(project);
    genTupFile(implFileNames);
    genMakeFile(implFileNames);
    genCmakeFile(implFileNames);
}

} // namespace
