#ifndef BASTVIEW_H
#define BASTVIEW_H

#include "RoseTreeView.h"
#include <QtDesigner/QDesignerExportWidget>

class SgNode;
class ItemTreeModel;

class AstFilterInterface;

/**
 * \brief A tree showing namespaces,classes,function,loops (not raw SgNodes)
 *
 * \image html BeautifiedAst.jpg
 *
 * This view is intended for user which are not familiar with the ROSE data structures,
 * it presents a view of the code including only elements a C++ developer should be familiar with (classes, namespaces,functions etc)
 *
 * There are two usage modes:
 *  - project mode (if node is a SgProject)
 *      - view is sorted after include and source files
 *  - if node is no SgProject: all elements of subtree spanned by this SgNode
 *    are displayed
 *
 *  Filtering is also possible via the AstFilterInterface
 *
 *  The classes for viewing beautified AST's are BAstNode and BAstModel
 *  they are also used by the ProjectManager to show an outline of the files in the project
 */
class QDESIGNER_WIDGET_EXPORT BAstView : public RoseTreeView
{
    Q_OBJECT

    public:
        BAstView(QWidget * parent=NULL);
        virtual ~BAstView();


    protected:
        void updateModel();
};

#endif
