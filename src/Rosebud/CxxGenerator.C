#include <Rosebud/CxxGenerator.h>

#include <boost/format.hpp>

namespace Rosebud {

std::string
CxxGenerator::machineGenerated(char commentType) {
    static const std::string mesg = "THIS FILE IS MACHINE GENERATED  --  DO NOT MODIFY";
    if (mesg.size() < outputWidth) {
        return makeTitleComment(std::string((outputWidth - mesg.size()) / 2, ' ') + mesg,
                                "", commentType, outputWidth);
    } else {
        return makeTitleComment(mesg, "", commentType, outputWidth);
    }
}

// Emit the destructor for the class.
void
CxxGenerator::genDestructor(std::ostream &header, std::ostream &impl, const Ast::Class::Ptr &c) {
    ASSERT_not_null(c);

    if (header) {
        header <<"\n"
               <<THIS_LOCATION <<"public:\n"
               <<"    /** Destructor. */\n"
               <<"    virtual ~" <<c->name <<"();\n";
    }

    if (impl) {
        impl <<"\n"
             <<THIS_LOCATION <<c->name <<"::~" <<c->name <<"() {\n"
             <<"    destructorHelper();\n"
             <<"}\n";
    }
}

void
CxxGenerator::genConstructorBody(std::ostream&, const Ast::Class::Ptr&) {}

// Emit the default constructor
void
CxxGenerator::genDefaultConstructor(std::ostream &header, std::ostream &impl, const Ast::Class::Ptr &c, Access access) {
    ASSERT_not_null(c);

    // Default constructor declaration
    if (header) {
        header <<"\n"
               <<THIS_LOCATION <<toString(access) + ":\n"
               <<"    /** Default constructor. */\n"
               <<"    " <<c->name <<"();\n";
    }

    // Default constructor implementation
    if (impl) {
        impl <<"\n"
             <<THIS_LOCATION <<c->name <<"::" <<c->name <<"()";
        size_t nInits = 0;
        for (const auto &p: c->properties) {
            const std::string expr = ctorInitializerExpression(p(), initialValue(p()));
            if (!expr.empty()) {
                impl <<"\n" <<THIS_LOCATION <<locationDirective(p(), p->startToken)
                     <<"    " <<(0 == nInits++ ? ": " : ", ") <<propertyDataMemberName(p()) <<"(" <<expr <<")";
            }
        }
        impl <<" {";
        genConstructorBody(impl, c);
        impl <<"}\n";
    }
}

// Emit the ctor_args constructor
bool
CxxGenerator::genArgsConstructor(std::ostream &header, std::ostream &impl, const Ast::Class::Ptr &c, const Hierarchy &h,
                                 Access access) {
    ASSERT_not_null(c);

    // Get the properties that are constructor arguments. If there are none, then we don't need this constructor.
    std::vector<Ast::Property::Ptr> args = allConstructorArguments(c, h);
    if (args.empty())
        return false;

    //--------------------------------------------------------------------------------------------------------------------------
    // Constructor declaration
    //--------------------------------------------------------------------------------------------------------------------------

    // Constructor declaration having the names of all the Rosebud::ctor_arg properties as argument names.
    header <<"\n"
           <<THIS_LOCATION <<toString(access) <<":\n"
           <<"    /** Constructor. */\n"
           <<"    " <<(args.size() == 1 ? "explicit " : "") <<c->name <<"(";
    for (const auto &p: args) {
        auto argClass = p->findAncestor<Ast::Class>();
        ASSERT_not_null(argClass);
        header <<(p == args.front() ? "" : ",\n    " + std::string(c->name.size() + 1, ' '))
               <<constRef(propertyMutatorArgumentType(p)) <<" " <<p->name;
    }
    header <<");\n";

    //--------------------------------------------------------------------------------------------------------------------------
    // Constructor definition
    //--------------------------------------------------------------------------------------------------------------------------

    // Show information about how the constructor args map to classes. This comment is to help when debugging ROSE.
    impl <<"\n"
         <<THIS_LOCATION <<"// The association between constructor arguments and their classes:\n";
    for (const auto &p: args) {
        auto argClass = p->findAncestor<Ast::Class>();
        ASSERT_not_null(argClass);
        impl <<(boost::format("//    property=%-16s class=%s\n") % p->name % argClass->name);
    }

    // Implementation declaration, almost the same as above.
    impl <<THIS_LOCATION <<c->name <<"::" <<c->name <<"(";
    for (const auto &p: args) {
        auto argClass = p->findAncestor<Ast::Class>();
        impl <<(p == args.front() ? "" : ",\n" + std::string(2*c->name.size()+3, ' '))
             <<constRef(propertyMutatorArgumentType(p)) <<" " <<p->name;
    }
    impl <<")";

    // Emit the initializer for the base class if there is a base class, and remove those properties from the `args` list.
    size_t nInits = 0;
    for (auto base: c->inheritance) {
        if (base.first != "public")
            continue;
        const std::string baseClassName = base.second;
        const auto baseClassVertex = h.findVertexKey(baseClassName);
        if (baseClassVertex == h.vertices().end())
            continue;

        Ast::Class::Ptr baseClass = baseClassVertex->value();
        const size_t nBaseArgs = allConstructorArguments(baseClass, h).size();
        ASSERT_require(nBaseArgs <= args.size());
        if (nBaseArgs > 0) {
            impl <<"\n    : " <<baseClassName <<"(";
            for (size_t i = 0; i < nBaseArgs; ++i) {
                auto p = args.front();
                auto argClass = p->findAncestor<Ast::Class>();
                ASSERT_not_null(argClass);
                impl <<(i ? ", " : "") <<p->name;
                args.erase(args.begin());
            }
            impl <<")";
            ++nInits;
        }
    }

    // Emit the property initializations, some of which come from arguments
    for (const auto &p: c->properties) {
        auto arg = std::find(args.begin(), args.end(), p());
        auto argClass = p->findAncestor<Ast::Class>();
        if (arg != args.end()) {
            // Property is initialized from an argument
            impl <<(nInits++ ? "\n    , " : "\n    : ")
                 <<propertyDataMemberName(p()) <<"(";
            if (p->findAttribute("Rosebud::traverse")) {
                impl <<"initParentPointer(" <<ctorInitializerExpression(p(), p->name) <<", this)";
            } else {
                impl <<ctorInitializerExpression(p(), p->name);
            }
            impl <<")";
        } else {
            // Property is initialized from the property's initial value expression
            const std::string initExpr = ctorInitializerExpression(p(), initialValue(p()));
            if (!initExpr.empty()) {
                impl <<(nInits++ ? "\n    , " : "\n    : ")
                     <<propertyDataMemberName(p()) <<"(" <<initExpr <<")";
            }
        }
    }

    impl <<" {";
    genConstructorBody(impl, c);
    impl <<"}\n";
    return true;
}

// Initializer expression for property constructors
std::string
CxxGenerator::ctorInitializerExpression(const Ast::Property::Ptr&, const std::string &expr) {
    return expr;
}

// Reset statement with terminating semicolon for properties
std::string
CxxGenerator::resetStatement(const Ast::Property::Ptr &p) {
    const std::string init = initialValue(p);
    if (init.empty()) {
        return "";
    } else {
        return propertyDataMemberName(p) + " = " + init + ";";
    }
}

std::string
CxxGenerator::propertyDataMemberType(const Ast::Property::Ptr &p) {
    ASSERT_not_null(p);
    ASSERT_not_null(p->cType);
    const std::string retval = p->cType->string();
    ASSERT_forbid(retval.empty());
    return retval;
}

std::string
CxxGenerator::propertyAccessorReturnType(const Ast::Property::Ptr &p) {
    return removeVolatileMutable(propertyDataMemberType(p));
}

std::string
CxxGenerator::propertyMutatorArgumentType(const Ast::Property::Ptr &p) {
    return propertyDataMemberType(p);
}

std::string
CxxGenerator::removePointer(const std::string &type) const {
    // Raw pointer
    std::regex rawPtr("(.*?)\\s*\\*");
    std::smatch found;
    if (std::regex_match(type, found, rawPtr)) {
        return found.str(1);
    }

    // ROSE naming convention pointer
    std::regex rosePtr("(.*?)(::)?(Const)?Ptr");
    if (std::regex_match(type, found, rosePtr)) {
        return found.str(1);
    }

    // Like std::remove_pointer, anything else is returned as-is
    return type;
}

std::string
CxxGenerator::valueType(const Ast::Property::Ptr &p) {
    const std::string retval = removeVolatileMutable(propertyDataMemberType(p));
    ASSERT_forbid(retval.empty());
    return retval;
}

std::string
CxxGenerator::initialValue(const Ast::Property::Ptr &p) {
    ASSERT_not_null(p);
    return p->cInit ? p->cInit->string() : "";
}

// Emit the function that initializes properties. This is not used by generated code since the initializations happen
// in the constructors, but it can be useful in user-defined constructors where the user is not necessarily expected to
// know the names of all the property data members.
void
CxxGenerator::genInitProperties(std::ostream &header, std::ostream &impl, const Ast::Class::Ptr &c) {
    ASSERT_not_null(c);

    if (header) {
        header <<"\n"
               <<THIS_LOCATION <<"protected:\n"
               <<"    /** Initialize all properties that have explicit initial values.\n"
               <<"     *\n"
               <<"     *  This function is mostly for use in user-defined constructors where the user desires to initialize\n"
               <<"     *  all the properties but does not know the names of the data members that store the property values.\n"
               <<"     *  This function initializes the properties that have explicit initializations within this class, but\n"
               <<"     *  does not recursively initialize base classes. */\n"
               <<"    void initializeProperties();\n";
    }

    if (impl) {
        impl <<"\n"
             <<THIS_LOCATION <<"void\n"
             <<c->name <<"::initializeProperties() {\n";

        // Initialize the properties of this class
        for (const auto &p: c->properties) {
            const std::string stmt = resetStatement(p());
            if (!stmt.empty())
                impl <<"    " <<stmt <<"\n";
        }
        impl <<"}\n";
    }
}

} // namespace
