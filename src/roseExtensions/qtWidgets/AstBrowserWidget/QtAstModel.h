#ifndef ASTTREEMODEL_H
#define ASTTREEMODEL_H



#include <QAbstractItemModel>

#include "AstFilters.h"

class SgNode;
class QtAstModel : public QAbstractItemModel
{
	Q_OBJECT
		class ModelNode;

	public:
		QtAstModel(SgNode * node, QObject * parent= 0);
		virtual ~QtAstModel();



		virtual Qt::ItemFlags flags (const QModelIndex & index) const;
		virtual QVariant data       (const QModelIndex & index, int role = Qt::DisplayRole ) const;
		virtual QVariant headerData (int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;

		virtual int  rowCount    (const QModelIndex & parent = QModelIndex() ) const;
		virtual int  columnCount (const QModelIndex & parent = QModelIndex() ) const;


		virtual QModelIndex index ( int row, int column, const QModelIndex & parent = QModelIndex() ) const;
		virtual QModelIndex parent ( const QModelIndex & index ) const;

		//@warning may return NULL!
		virtual SgNode * getNodeFromIndex(const QModelIndex & ind);

	public slots:
		void setNode(SgNode * node);

		/** Tree can be filtered by ASTFilterInterface
		 * Deletion of filter is done by ASTTreeModel
		 * set to NULL, for unfiltered view  */
		void setFilter(AstFilterInterface * filter);

	protected:
		ModelNode * treeRoot;
		AstFilterInterface * filter;
};

#endif
