#include "MemoryTypeDisplay.h"
#include "MemoryManager.h"

#include "TypeInfoDisplay.h"
#include "VariablesTypeDisplay.h"
#include <QIcon>


MemoryTypeDisplay::MemoryTypeDisplay(MemoryType * mt_, bool displayPointer)
    : mt(mt_)
{
    typedef PropertyValueNode Pvn;
    QString addrStr = QString("0x%1").arg(mt->getAddress(),0,16);

    addChild( new Pvn("Address",addrStr));
    addChild( new Pvn("Size", static_cast<unsigned int>(mt->getSize())));
    addChild( new Pvn("Allocation at",mt->getPos().toString().c_str()));
    addChild( new Pvn("Init Status", mt->getInitString().c_str()) );
    addChild( new Pvn("Is On Stack", mt->isOnStack()));

    Pvn * typeInfoSection = new Pvn("Type Info","");
    typeInfoSection->setFirstColumnSpanned(true);
    addChild( typeInfoSection);


    MemoryType::TypeInfoMap::const_iterator it = mt->getTypeInfoMap().begin();
    for(; it != mt->getTypeInfoMap().end();  ++it)
        typeInfoSection->addChild( RsTypeDisplay::build(it->second,it->first));

    //pointer->setIcon(QIcon(":/icons/arrow.png"));


    if(displayPointer)
    {
        Pvn * pointerIn = new Pvn("Pointer to this Allocation","");
        pointerIn->setFirstColumnSpanned(true);



        //TODO pointer structure has changed -> adapt display
        //Pointer into this
        /*
        MemoryType::VariableSet::const_iterator i = mt->getPointerSet().begin();
        for(; i != mt->getPointerSet().end(); ++i)
            pointer->addChild( new VariablesTypeDisplay(*i,false) );
         */
    }
}





QVariant MemoryTypeDisplay::data(int role, int column) const
{
    if(role == Qt::DisplayRole)
    {
        if(column == 0)
        {
            QString addrStr = QString("Address 0x%1").arg(mt->getAddress(),0,16);
            return addrStr;
        }
        else if (column == 1)
            return mt->getPos().toString().c_str();
    }
    if(role == Qt::DecorationRole && column ==0)
    {
        return mt->isOnStack() ? QIcon(":/icons/variable.gif") : QIcon(":/icons/allocation.gif");
    }

    return QVariant();
}

QStringList MemoryTypeDisplay::sectionHeader() const
{
    return QStringList() << "Title" << "Size" << "Offset";
}


MemoryTypeDisplay * MemoryTypeDisplay::build(MemoryManager * mm, bool showHeap, bool showStack)
{

    MemoryTypeDisplay * root = new MemoryTypeDisplay();

    if(!showHeap && !showStack)
        return root;

    MemoryManager::MemoryTypeSet::const_iterator it = mm->getAllocationSet().begin();
    for(; it != mm->getAllocationSet().end(); ++it )
    {
        if( (*it)->isOnStack() && !showStack)
            continue;

        if( !(*it)->isOnStack() && !showHeap)
            continue;

        root->addChild(new MemoryTypeDisplay(*it));
    }

    return root;

}
