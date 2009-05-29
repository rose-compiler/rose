#ifndef BASTMODEL_H
#define BASTMODEL_H

#include "util/ItemTreeModel.h"

class SgNode;
class AstFilterInterface;

class BAstModel : public ItemTreeModel
{
    Q_OBJECT

    public:
        BAstModel(QObject * parent);
        virtual ~BAstModel();

        void setNode(SgNode * proj);
        void setFilter(AstFilterInterface * filter);

        SgNode * getNodeFromIndex(const QModelIndex & ind);

    protected:
        virtual QVariant data (const QModelIndex & ind, int role) const;

        void updateModel();

        AstFilterInterface * curFilter;
        SgNode * curNode;

};

#endif
