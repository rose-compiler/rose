#include "sage3basic.h"


#include "AstDisplayInfo.h"
#include <string>
namespace AstDisplayInfo
{
    NodeType getType(SgNode* node)
    {
        SgClassDefinition *    sgClassDefinition    = isSgClassDefinition(node);
        SgFunctionDefinition * sgFunctionDefinition = isSgFunctionDefinition(node);
        SgFunctionDeclaration *sgFunctionDecl       = isSgFunctionDeclaration(node);

        SgNamespaceDefinitionStatement * sgNSDef    = isSgNamespaceDefinitionStatement(node);

        //Assembler Nodes
        SgAsmFunction * sgAsmFunctionDecl= isSgAsmFunction(node);

        if(sgClassDefinition)          return CLASS;
        else if (sgFunctionDefinition) return FUNCTION;
        else if (sgFunctionDecl)       return FUNCTION;
        else if (sgNSDef)              return NAMESPACE;
        else if (sgAsmFunctionDecl)    return FUNCTION;
        else                           return UNKNOWN;
    }


    QString getShortNodeNameDesc(SgNode * node)
    {
        SgClassDefinition * sgClassDefinition        = isSgClassDefinition(node);
        SgClassDeclaration* sgClassDeclaration       = isSgClassDeclaration(node);
        SgFunctionDefinition * sgFunctionDefinition  = isSgFunctionDefinition(node);
        SgFunctionDeclaration * sgFunctionDecl       = isSgFunctionDeclaration(node);
        SgNamespaceDefinitionStatement * sgNSDef     = isSgNamespaceDefinitionStatement(node);
        SgNamespaceDeclarationStatement * sgNsDecl   = isSgNamespaceDeclarationStatement(node);


        SgFile * sgFile                              = isSgFile(node);
        SgProject * sgProject                        = isSgProject(node);

        SgVariableDeclaration * sgVarDecl            = isSgVariableDeclaration(node);
        SgInitializedName * sgInitName               = isSgInitializedName(node);

        SgTypedefDeclaration * sgTypeDefDecl         = isSgTypedefDeclaration(node);
        SgEnumDeclaration *   sgEnumDecl             = isSgEnumDeclaration(node);
        SgPragmaDeclaration * sgPragmaDecl           = isSgPragmaDeclaration(node);

        //Assembler Nodes
        SgAsmFunction * sgAsmFunctionDecl  = isSgAsmFunction(node);

        std::string s;

        if(sgClassDefinition)          s = sgClassDefinition->get_declaration()->get_name().getString();
        else if (sgClassDeclaration)   s = sgClassDeclaration->get_name().getString();
        else if (sgFunctionDefinition) s = sgFunctionDefinition->get_declaration()->get_name().getString();
        else if (sgNSDef)              s = sgNSDef->get_namespaceDeclaration()->get_name().getString();
        else if (sgNsDecl)             s = "namespace " + sgNsDecl->get_name().getString();
        else if (sgFunctionDecl)       s = sgFunctionDecl->get_name().getString();
        else if (sgAsmFunctionDecl)    s = StringUtility::demangledName( sgAsmFunctionDecl->get_name());
        else if (sgTypeDefDecl)        s = "typedef " + sgTypeDefDecl->get_name().getString();
        else if (sgEnumDecl)           s = "enum " + sgEnumDecl->get_name().getString();
        else if (sgPragmaDecl)         s = "pragma " + sgPragmaDecl->get_pragma()->get_name();
        else if (sgVarDecl)            s = "Variable Declaration";
        else if (sgInitName)           s = sgInitName->get_name().getString();
        else if (sgFile)               s = sgFile->get_sourceFileNameWithoutPath();
        else if (sgProject)            s = "Project";
        else                           return getShortNodeTypeDesc(node);


        return QString(s.c_str());
    }

    QString getShortNodeTypeDesc(SgNode * node)
    {
        SgClassDefinition * sgClassDefinition       = isSgClassDefinition(node);

        SgFunctionDefinition * sgFunctionDefinition = isSgFunctionDefinition(node);
        SgFunctionDeclaration * sgFunctionDecl      = isSgFunctionDeclaration(node);
        SgNamespaceDefinitionStatement * sgNSDef    = isSgNamespaceDefinitionStatement(node);
        SgForStatement * sgForStatement             = isSgForStatement(node);
        SgWhileStmt * sgWhileStatement              = isSgWhileStmt(node);

        //Assembler Nodes
        SgAsmFunction * sgAsmFunctionDecl= isSgAsmFunction(node);


        if      (sgClassDefinition)     return QObject::tr("Class");
        else if (sgFunctionDefinition)  return QObject::tr("Function");
        else if (sgFunctionDecl)        return QObject::tr("Function");
        else if (sgNSDef)               return QObject::tr("Namespace");
        else if (sgForStatement)        return QObject::tr("For-Loop");
        else if (sgWhileStatement)      return QObject::tr("While-Loop");
        else if (sgAsmFunctionDecl)     return QObject::tr("Asm-Function");
        else                            return QString();
    }


    QIcon nodeIcon(SgNode * node)
    {
        SgClassDefinition * sgClassDefinition       = isSgClassDefinition(node);
        SgClassDeclaration* sgClassDeclaration      = isSgClassDeclaration(node);

        SgFunctionDefinition * sgFunctionDefinition = isSgFunctionDefinition(node);
        SgFunctionDeclaration * sgFunctionDecl      = isSgFunctionDeclaration(node);
        SgNamespaceDefinitionStatement * sgNSDef    = isSgNamespaceDefinitionStatement(node);
        SgNamespaceDeclarationStatement * sgNsDecl  = isSgNamespaceDeclarationStatement(node);

        SgForStatement * sgForStatement             = isSgForStatement(node);
        SgWhileStmt * sgWhileStatement              = isSgWhileStmt(node);

        SgTypedefDeclaration * sgTypeDefDecl        = isSgTypedefDeclaration(node);
        SgEnumDeclaration *   sgEnumDecl            = isSgEnumDeclaration(node);
        SgPragmaDeclaration * sgPragmaDecl          = isSgPragmaDeclaration(node);

        SgVariableDeclaration * sgVarDecl           = isSgVariableDeclaration(node);
        SgInitializedName * sgInitName              = isSgInitializedName(node);


        SgSourceFile * sgSrcFile                    = isSgSourceFile(node);
        SgBinaryComposite * sgBinFile               = isSgBinaryComposite(node);
        SgProject * sgProject                       = isSgProject(node);


        //Assembler Nodes
        SgAsmFunction * sgAsmFunctionDecl= isSgAsmFunction(node);

        if      (sgClassDefinition)     return QIcon(":/util/NodeIcons/class.gif");
        else if (sgClassDeclaration)    return QIcon(":/util/NodeIcons/class.gif");
        else if (sgFunctionDefinition)  return QIcon(":/util/NodeIcons/function.gif");
        else if (sgFunctionDecl)        return QIcon(":/util/NodeIcons/function.gif");
        else if (sgNSDef)               return QIcon(":/util/NodeIcons/namespace.gif");
        else if (sgNsDecl)              return QIcon(":/util/NodeIcons/namespace.gif");
        else if (sgForStatement)        return QIcon(":/util/NodeIcons/loop.png");
        else if (sgWhileStatement)      return QIcon(":/util/NodeIcons/loop.png");
        else if (sgAsmFunctionDecl)     return QIcon(":/util/NodeIcons/function.gif");
        else if (sgSrcFile)             return QIcon(":/util/NodeIcons/sourcefile.gif");
        else if (sgBinFile)             return QIcon(":/util/NodeIcons/binaryfiles.gif");
        else if (sgProject)             return QIcon(":/util/NodeIcons/project.gif");
        else if (sgTypeDefDecl)         return QIcon(":/util/NodeIcons/typedef.gif");
        else if (sgEnumDecl)            return QIcon(":/util/NodeIcons/enum.gif");
        else if (sgPragmaDecl)          return QIcon(":/util/NodeIcons/pragma.gif");
        else if (sgVarDecl)             return QIcon(":/util/NodeIcons/variable.gif");
        else if (sgInitName)            return QIcon(":/util/NodeIcons/variable.gif");
        else                            return QIcon();
    }

    QIcon includedFilesIcon()
    {
        return QIcon(":/util/NodeIcons/includes.gif");
    }

    QIcon includeFileIcon()
    {
        return QIcon(":/util/NodeIcons/include.gif");
    }

    QIcon sourceFileIcon()
    {
        return QIcon(":/util/NodeIcons/sourcefile.gif");
    }

}


