
#ifndef PROPERTYTREEMODEL_H
#define PROPERTYTREEMODEL_H

#include "ItemTreeModel.h"


/**
 *  TreeModel for a Tree like the PropertyEditor in Qt-Designer
 *              column count: 2
 *
 *      section-header1    | empty              (ItemTreeHeaderNode)
 *              -> property1   | value1         (ItemTreePropValueNode)
 *          -> property2   | value2             (ItemTreePropValueNode)
 *              section-header2    | empty              (ItemTreeHeaderNode)
 *         ->
 *
 */
class PropertyTreeModel : public ItemTreeModel
{
        Q_OBJECT

        class ItemTreeHeaderNode;
        class ItemTreePropValueNode;

        public:
                PropertyTreeModel(QObject * parent=NULL);
                virtual ~PropertyTreeModel();


                int addSection(const QString & sectionName);

                QModelIndex addEntryToSection(int sectionId,
                                                                          const QString & property,
                                                                          const QVariant & value);

                QModelIndex addEntry (const QModelIndex & parent,
                                                            const QString & property,
                                                            const QVariant & value);

                void clear();

        protected:
                // Root of the tree; is not displayed only its children
                ItemTreeNode * treeRoot;
};




#endif
