#ifndef ASTBROWSERWIDGET_H
#define ASTBROWSERWIDGET_H

#include <QtDesigner/QDesignerExportWidget>

#include "RoseTreeView.h"
#include "QtAstModel.h"

class SgNode;
class QtASTModel;
class AstFilterInterface;

/**
 * Treeview for browsing a Sage-AST
 *
 * <img src="../AstBrowserWidget.jpg"  alt="Screenshot">
 *
 * This view shows a Qt-TreeView of a Sage-AST. \n
 * To set the parent node call setNode() \n
 * Signals are sent when the user clicks or doubleclicks ( see clicked() ) .  \n
 * These signals have parameters with the current SgNode clicked on,
 * or as alternative the source-code file and location.
 *
 * It's possible to filter the view with an AstFilterInterface
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

		/** Filters the view by using an AstFilterInterface
		 *   @param newFilter the filter, for resetting (show all nodes) use NULL */
		virtual void setFilter(AstFilterInterface * newFilter);

		/// Convenience function which creates an AstFilterFileById and filters the view with it
		virtual void setFileFilter(int fileId);

	protected:

		QtAstModel * model;
};

#endif
