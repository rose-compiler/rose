#include "rose.h"


#include "AstDisplayInfo.h"

namespace AstDisplayInfo
{
    NodeType getType(SgNode* node)
    {
        SgClassDefinition * sgClassDefinition       = isSgClassDefinition(node);
        SgFunctionDefinition * sgFunctionDefinition = isSgFunctionDefinition(node);
        SgNamespaceDefinitionStatement * sgNSDef    = isSgNamespaceDefinitionStatement(node);

        //Assembler Nodes
        SgAsmFunctionDeclaration * sgAsmFunctionDecl= isSgAsmFunctionDeclaration(node);

        if(sgClassDefinition)          return CLASS;
        else if (sgFunctionDefinition) return FUNCTION;
        else if (sgNSDef)              return NAMESPACE;
        else if (sgAsmFunctionDecl)    return FUNCTION;
        else                           return UNKNOWN;

    }


    QString getShortNodeNameDesc(SgNode * node)
    {
        SgClassDefinition * sgClassDefinition       = isSgClassDefinition(node);
        SgFunctionDefinition * sgFunctionDefinition = isSgFunctionDefinition(node);
        SgNamespaceDefinitionStatement * sgNSDef    = isSgNamespaceDefinitionStatement(node);

        //Assembler Nodes
        SgAsmFunctionDeclaration * sgAsmFunctionDecl= isSgAsmFunctionDeclaration(node);

        SgName name;


        if(sgClassDefinition)          name = sgClassDefinition->get_declaration()->get_name();
        else if (sgFunctionDefinition) name = sgFunctionDefinition->get_declaration()->get_name();
        else if (sgNSDef)              name = sgNSDef->get_namespaceDeclaration()->get_name();
        else if (sgAsmFunctionDecl)    name = sgAsmFunctionDecl->get_name();
        else                           return getShortNodeTypeDesc(node);

        const std::string & s = name.getString();

        if (sgAsmFunctionDecl)
            return QString( StringUtility::demangledName(s).c_str());
        else
            return QString(s.c_str());
    }

    QString getShortNodeTypeDesc(SgNode * node)
    {
        SgClassDefinition * sgClassDefinition       = isSgClassDefinition(node);
        SgFunctionDefinition * sgFunctionDefinition = isSgFunctionDefinition(node);
        SgNamespaceDefinitionStatement * sgNSDef    = isSgNamespaceDefinitionStatement(node);
        SgForStatement * sgForStatement             = isSgForStatement(node);
        SgWhileStmt * sgWhileStatement              = isSgWhileStmt(node);

        //Assembler Nodes
        SgAsmFunctionDeclaration * sgAsmFunctionDecl= isSgAsmFunctionDeclaration(node);


        if      (sgClassDefinition)     return QObject::tr("Class");
        else if (sgFunctionDefinition)  return QObject::tr("Function");
        else if (sgNSDef)               return QObject::tr("Namespace");
        else if (sgForStatement)        return QObject::tr("For-Loop");
        else if (sgWhileStatement)      return QObject::tr("While-Loop");
        else if (sgAsmFunctionDecl)     return QObject::tr("Asm-Function");
        else                            return QString();
    }


    QIcon nodeIcon(SgNode * node)
    {
        SgClassDefinition * sgClassDefinition       = isSgClassDefinition(node);
        SgFunctionDefinition * sgFunctionDefinition = isSgFunctionDefinition(node);
        SgNamespaceDefinitionStatement * sgNSDef    = isSgNamespaceDefinitionStatement(node);
        SgForStatement * sgForStatement             = isSgForStatement(node);
        SgWhileStmt * sgWhileStatement              = isSgWhileStmt(node);

        //Assembler Nodes
        SgAsmFunctionDeclaration * sgAsmFunctionDecl= isSgAsmFunctionDeclaration(node);

        if      (sgClassDefinition)     return QIcon(":/util/NodeIcons/class.gif");
        else if (sgFunctionDefinition)  return QIcon(":/util/NodeIcons/function.gif");
        else if (sgNSDef)               return QIcon(":/util/NodeIcons/namespace.gif");
        else if (sgForStatement)        return QIcon(":/util/NodeIcons/loop.png");
        else if (sgWhileStatement)      return QIcon(":/util/NodeIcons/loop.png");
        else if (sgAsmFunctionDecl)     return QIcon(":/util/NodeIcons/function.gif");
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


