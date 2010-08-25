#ifndef ASTTREEMODEL_H
#define ASTTREEMODEL_H



#include <QAbstractItemModel>

#include "AstFilters.h"

class SgNode;

/**
 * \brief A ModelView class which represents the data in an AST
 *
 * If you are not familiar with the Qt Model-View interfaces see the Qt-documentation for QAbstractItemModel
 *
 * The data are fetched from the AST on demand while the user browses through it
 * so the model is build up on demand
 *
 * There is a second tree build up (out of ModelNodes) which just holds the display-information
 * but usually this display-tree is smaller than the whole AST because it's build up on demand
 */
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

                /// Should not be used any more, query for the SgNodeRole of an index
                /// @warning may return NULL!
                virtual SgNode * getNodeFromIndex(const QModelIndex & ind);

        public slots:
                /// Set new root of the tree
                void setNode(SgNode * node);

                /** Tree can be filtered by ASTFilterInterface
                 * Deletion of filter is done by this class
                 * set to NULL, for unfiltered view  */
                void setFilter(AstFilterInterface * filter);

        protected:
                ModelNode * treeRoot;
                AstFilterInterface * filter;
};

#endif
