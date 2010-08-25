#ifndef ASTBROWSERWIDGET_H
#define ASTBROWSERWIDGET_H

#include <QtDesigner/QDesignerExportWidget>

#include "RoseTreeView.h"
#include "QtAstModel.h"

class SgNode;
class QtASTModel;
class AstFilterInterface;

/**
 * \brief Treeview for browsing a Sage-AST
 *
 * \image html AstBrowserWidget.jpg
 *
 * This view shows a Qt-TreeView of a Sage-AST. \n
 * For changing the display call setNode() which sets a new rootNode or gotoNode() which selects a given node
 *
 * If the user clicks on a node the signal nodeActivated() is emitted, on when double clicking nodeActivatedAlt() is called
 *
 * It's possible to filter the view with an AstFilterInterface. For example filtering out all nodes which are part of header files etc.
 *
 * To get icons and node-descriptions the functions in namespace AstDisplayInfo are used
 *
 * A custom model is used for respresenting the AST: QtAstModel
 */
class QDESIGNER_WIDGET_EXPORT AstBrowserWidget : public RoseTreeView
{
	Q_OBJECT

	public:
		AstBrowserWidget(QWidget*parent=0);
		AstBrowserWidget(SgNode * node, QWidget * parent=0);
		virtual ~AstBrowserWidget();

	public slots:
                /// Sets the root node of the View
		virtual void setNode(SgNode * node);
        virtual bool gotoNode( SgNode *node )
                { return false; }

		/** Filters the view by using an AstFilterInterface
                 *   @param newFilter the filter, for resetting (show all nodes) use NULL */
                virtual void setFilter(AstFilterInterface * newFilter);

		/// Convenience function which creates an AstFilterFileById and filters the view with it
		virtual void setFileFilter(int fileId);

	protected:

		QtAstModel * model;
};

#endif
