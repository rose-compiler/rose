#ifndef ASTBROWSERWIDGET_H
#define ASTBROWSERWIDGET_H

#include <QTreeView>
#include <QtDesigner/QDesignerExportWidget>

#include "QtAstModel.h"

class SgNode;
class QtASTModel;
class AstFilterInterface;

class QDESIGNER_WIDGET_EXPORT AstBrowserWidget : public QTreeView
{
	Q_OBJECT

	public:
		AstBrowserWidget(QWidget*parent=0);
		AstBrowserWidget(SgNode * node, QWidget * parent=0);
		virtual ~AstBrowserWidget();

	public slots:
		void setNode(SgNode * node);

		//For resetting use newFilter=NULL
		void setFilter(AstFilterInterface * newFilter);
		void setFileFilter(int fileId);
                
	signals:
        /// Raised if clicked on an Entry
        /// warning: may be null (appropriate action would be to clear the attached view)
		void clicked(SgNode * node);
                void doubleClicked( SgNode * node );

		void clicked(const QString & filename,
		             int startLine, int startCol,
                int endLine, int endCol);

	protected slots:
		void viewClicked(const QModelIndex & ind);
                void viewDoubleClicked( const QModelIndex & ind );

	protected:
		QtAstModel * model;
};

#endif
