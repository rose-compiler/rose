#include <sstream>
#include <QIcon>

#include "MemoryTypeDisplay.h"
#include "MemoryManager.h"
#include "PointerDisplay.h"

#include "PointerManager.h"
#include "CppRuntimeSystem.h"

#include "ModelRoles.h"

#include "TypeInfoDisplay.h"
#include "VariablesTypeDisplay.h"

static
bool onStack(const MemoryType& mt)
{
  return mt.howCreated() & (akStack | akGlobal);
}

MemoryTypeDisplay::MemoryTypeDisplay(const MemoryType& mt_, bool displayPointer)
    : mt(&mt_)
{
    typedef PropertyValueNode Pvn;

    std::stringstream out;

    out << mt->beginAddress();

    addChild( new Pvn("Address", out.str().c_str()));
    addChild( new Pvn("Size", static_cast<unsigned int>(mt->getSize())));
    addChild( new Pvn("Allocation at", mt->getPos().toString().c_str()));
    addChild( new Pvn("Init Status", mt->getInitString().c_str()) );
    addChild( new Pvn("Is On Stack", onStack(*mt)) );

    Pvn * typeInfoSection = new Pvn("Type Info","");
    typeInfoSection->setFirstColumnSpanned(true);
    addChild( typeInfoSection);


    MemoryType::TypeData::const_iterator it = mt->typeData().begin();





    for(; it != mt->typeData().end();  ++it)
        typeInfoSection->addChild( RsTypeDisplay::build(it->second,it->first));

    //pointer->setIcon(QIcon(":/icons/arrow.png"));


    if(displayPointer)
    {

        PointerManager * pm = RuntimeSystem::instance()->getPointerManager();

        {
            Pvn * pointerIn = new Pvn("Pointer to this Allocation","");
            pointerIn->setFirstColumnSpanned(true);

            PointerManager::PointerSetIter i   = pm->sourceRegionIter(mt->beginAddress());
            PointerManager::PointerSetIter end = pm->sourceRegionIter(mt->lastValidAddress());

            for(; i != end; ++i)
                pointerIn->addChild(new PointerDisplay(*i));
        }

        {
            Pvn * pointerOut = new Pvn("Pointer into this Allocation","");
            pointerOut->setFirstColumnSpanned(true);

            PointerManager::TargetToPointerMap::const_iterator i   = pm->targetRegionIterBegin(mt->beginAddress());
            PointerManager::TargetToPointerMap::const_iterator end = pm->targetRegionIterEnd(mt->lastValidAddress());

            for(; i!= end; ++i)
                pointerOut->addChild(new PointerDisplay(i->second));
        }

    }
}



QVariant MemoryTypeDisplay::data(int role, int column) const
{
    if(role == Qt::DisplayRole)
    {
        if(column == 0)
        {
            QString addrStr = QString("Address 0x%1").arg(mt->beginAddress().local,0,16);
            return addrStr;
        }
        else if (column == 1)
            return mt->getPos().toString().c_str();
    }

    // \pp commented out to avoid compile error
    //~ if ( role == MemoryTypeRole)
        //~ return QVariant::fromValue<const MemoryType*>(mt);

    if(role == Qt::DecorationRole && column ==0)
    {
        return onStack(*mt) ? QIcon(":/icons/variable.gif") : QIcon(":/icons/allocation.gif");
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
        if ( onStack(it->second) && !showStack ) continue;

        if ( (!onStack(it->second)) && !showStack ) continue;

        root->addChild(new MemoryTypeDisplay(it->second));
    }

    return root;

}
