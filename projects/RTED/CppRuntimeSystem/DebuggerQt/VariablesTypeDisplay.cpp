#include "VariablesTypeDisplay.h"
#include "CppRuntimeSystem.h"

#include "TypeInfoDisplay.h"
#include "VariablesTypeDisplay.h"
#include "MemoryTypeDisplay.h"
#include "PointerDisplay.h"

#include "ModelRoles.h"

#include <QIcon>


VariablesTypeDisplay::VariablesTypeDisplay(VariablesType * vt_, bool displayMem)
    : vt(vt_)
{
    typedef PropertyValueNode PVN;
    QString addrStr = QString("0x%1").arg(vt->getAddress(),0,16);

    addChild( new PVN("Mangled Name",vt->getMangledName().c_str()));
    addChild( new PVN("Address",addrStr));
    addChild( RsTypeDisplay::build(vt->getType(),-1,"Type:"));



    PointerInfo * pi = vt->getPointerInfo();
    if(pi)
    {
        addChild(new PointerDisplay(pi));
    }

}



QVariant VariablesTypeDisplay::data(int role, int column) const
{
    if(role == Qt::DisplayRole)
    {
        if(column == 0)
            return vt->getName().c_str();
    }
    if(role == VariablesTypeRole)
        return QVariant::fromValue<VariablesType*>(vt);

    if(role == Qt::DecorationRole && column ==0)
        return QIcon(":/icons/variable.gif");

    return QVariant();
}

QStringList VariablesTypeDisplay::sectionHeader() const
{
    return QStringList() << "Title" << "Size" << "Offset";
}


VariablesTypeDisplay * VariablesTypeDisplay::build(StackManager * sm)
{
    typedef PropertyValueNode PVN;

    VariablesTypeDisplay * root = new VariablesTypeDisplay();

    for(int i=0; i < sm->getScopeCount(); i++)
    {
        PVN * curScope = new PVN(sm->getScopeName(i).c_str(),"");
        curScope->setFirstColumnSpanned(true);
        curScope->setIcon(QIcon(":/icons/scope.gif"));
        root->addChild(curScope);

        RuntimeSystem::VariableIter it = sm->variablesBegin(i);
        for(; it != sm->variablesEnd(i); ++it)
            curScope->addChild(new VariablesTypeDisplay(*it));

    }

    return root;
}
