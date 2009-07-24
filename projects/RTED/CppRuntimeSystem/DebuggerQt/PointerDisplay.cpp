#include "PointerDisplay.h"

#include "PointerManager.h"
#include "VariablesType.h"
#include "TypeInfoDisplay.h"

#include <QIcon>



PointerDisplay::PointerDisplay(PointerInfo * pi_)
    : pi(pi_)
{
    typedef PropertyValueNode Pvn;
    QString srcAddr    = QString("0x%1").arg(pi->getSourceAddress(),0,16);
    QString targetAddr = QString("0x%1").arg(pi->getTargetAddress(),0,16);



    addChild( new Pvn("Source Address", srcAddr));
    addChild( new Pvn("Target Address", targetAddr));

    addChild( RsTypeDisplay::build(pi->getBaseType(),-1,"Base Type"));
}


QVariant PointerDisplay::data(int role, int column) const
{
    if(role == Qt::DisplayRole )
    {
        if(column == 0)
        {
            VariablesType * vt = pi->getVariable();
            if(vt)
                return vt->getName().c_str();
            else
                return "Memory " + QString("0x%1").arg(pi->getSourceAddress(),0,16) ;
        }
        if(column ==1)
            return  QString("Source Addr: 0x%1").arg(pi->getSourceAddress(),0,16);

    }
    /*
    if(role == Qt::DecorationRole && column ==0)
    {
        //TODO icon for pointer
    }*/

    return QVariant();
}

QStringList PointerDisplay::sectionHeader() const
{
    return QStringList() << "Title" << "Size" << "Offset";
}


PointerDisplay * PointerDisplay::build(PointerManager * pm)
{

    PointerDisplay * root = new PointerDisplay();

    PointerManager::PointerSetIter it = pm->getPointerSet().begin();

    for(; it != pm->getPointerSet().end(); ++it)
        root->addChild(new PointerDisplay(*it));


    return root;

}

