#include "PointerDisplay.h"

#include "PointerManager.h"
#include "VariablesType.h"
#include "TypeInfoDisplay.h"

#include <QIcon>

#include "ptrops_operators.h"


QVariant as_QVariant(Address addr)
{
  std::stringstream out;

  out << addr;

  return QVariant(out.str().c_str());
}

QVariant as_QVariant(Address addr, std::string prefix)
{
  std::stringstream out;

  out << prefix << addr;

  return QVariant(out.str().c_str());
}

PointerDisplay::PointerDisplay(const PointerInfo* pi_)
    : pi(pi_)
{
    typedef PropertyValueNode Pvn;

    addChild( new Pvn("Source Address", as_QVariant(pi->getSourceAddress())) );
    addChild( new Pvn("Target Address", as_QVariant(pi->getTargetAddress())) );

    addChild( RsTypeDisplay::build(pi->getBaseType(),-1,"Base Type"));
}


QVariant PointerDisplay::data(int role, int column) const
{
    if(role == Qt::DisplayRole )
    {
        if(column == 0)
        {
            const VariablesType * vt = pi->getVariable();
            if(vt)
                return vt->getName().c_str();
            else
                return as_QVariant(pi->getSourceAddress(), "Memory ");
        }
        if(column ==1)
            return as_QVariant(pi->getSourceAddress(), "Source Addr: ");

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
