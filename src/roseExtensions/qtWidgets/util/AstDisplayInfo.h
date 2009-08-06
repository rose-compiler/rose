#ifndef ASTDISPLAYINFO_H
#define ASTDISPLAYINFO_H


#include <QIcon>

/**
 * \brief Returns display information for SgNodes
 */
namespace AstDisplayInfo
{

        /// the first 4 entries are only used by beautified ast
        enum NodeType
        {
            ROOT,
            INCLUDE_ROOT,
            SRC_FILE,
            INCLUDE_FILE,
            NAMESPACE,
            CLASS,
            FUNCTION,
            LOOP,
            UNKNOWN
        };

        /// Returns "type" of node (class,function,loop,namespace) at Sg*Definitions
        NodeType getType(SgNode*);

        /// Returns a short description of an Sg*Definition
        /// for named types (classes, functions etc) it returns the name
        /// if there is no adequate name for an SgNode it returns the
        /// getShortNodeTypeDesc
        /// if node is no definition it returns an empty string
        QString getShortNodeNameDesc(SgNode*);

        /// returns a description for SgNode-Type
        /// if SgNode is for example of type SgClassDefinition it returns "class"
        QString getShortNodeTypeDesc(SgNode*);

        /// icon for SgFunctionDeclaration, SgClassDeclaration...
        QIcon nodeIcon(SgNode *);
        /// icon with several include files
        QIcon includedFilesIcon();
        /// icon with one include file
        QIcon includeFileIcon();
        /// icon with one source-file
        QIcon sourceFileIcon();
};

#endif
