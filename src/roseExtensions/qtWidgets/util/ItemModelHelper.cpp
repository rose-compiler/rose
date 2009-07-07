#include "ItemModelHelper.h"


#include <QDebug>


QModelIndex findSgNodeInModel(const SgNode * node,
                              const QAbstractItemModel & model,
                              const QModelIndex & subTreeInd,
                              int column)
{
    if(node==NULL)
        return QModelIndex();

    for(int r=0; r < model.rowCount(subTreeInd); r++ )
    {
        QModelIndex curInd = model.index(r,column,subTreeInd);
        SgNode * inModel = qvariant_cast<SgNode*> ( model.data(curInd,SgNodeRole)) ;
        if(inModel == node)
            return curInd;
        else
        {
            QModelIndex subTreeRes = findSgNodeInModel(node,model,curInd);
            if(subTreeRes.isValid())
                return subTreeRes;
        }
    }

    return QModelIndex();
}


QModelIndex findVariantInModel(const QVariant & value,
                              const QAbstractItemModel & model,
                              int role,
                              const QModelIndex & subTreeInd,
                              int column)
{
    for(int r=0; r < model.rowCount(subTreeInd); r++ )
    {
        QModelIndex curInd = model.index(r,column,subTreeInd);
        QVariant inModel = model.data(curInd,role) ;
        if(inModel == value)
            return curInd;
        else
        {
            QModelIndex subTreeRes = findVariantInModel(value,model,role,curInd,column);
            if(subTreeRes.isValid())
                return subTreeRes;
        }
    }

    return QModelIndex();
}



