#ifndef ITEMMODELHELPER_H
#define ITEMMODELHELPER_H

#include <QModelIndex>
#include <QMetaType>

class SgNode;

enum { SgNodeRole = Qt::UserRole+1};
typedef SgNode* SgNodePtr;
Q_DECLARE_METATYPE(SgNodePtr)

/// used by drag and drop functions
//const QString SG_NODE_MIMETYPE = "application/SgNode";


/// Searches for an SgNode in a model via SgNodeRole only in first column!
/// @param node the node to search for
/// @param model the model in which the search is performed
/// @param subTree Only the subtree spanned by this modelindex is searched
QModelIndex findSgNodeInModel(const SgNode * node,
                              const QAbstractItemModel & model,
                              const QModelIndex & subTree = QModelIndex(),
                              int column=0);


QModelIndex findVariantInModel(const QVariant & value,
                               const QAbstractItemModel & model,
                               int role = Qt::DisplayRole,
                               const QModelIndex & subTree = QModelIndex(),
                               int column = 0);




#endif
