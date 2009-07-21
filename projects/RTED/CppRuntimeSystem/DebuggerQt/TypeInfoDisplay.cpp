#include "TypeInfoDisplay.h"
#include "TypeSystem.h"

#include <QIcon>
#include <QDebug>

RsTypeDisplay::RsTypeDisplay(RsType * t, const QString & n, int off)
    : type(t), name(n),offset(off)
{
}

QStringList RsTypeDisplay::sectionHeader() const
{
    return QStringList() << "Name" << "Size" << "Offset";
}

QVariant RsTypeDisplay::data(int role, int column) const
{
    // First column "memberName : className"
    // Second column size
    // third  column offset

    if(role == Qt::DisplayRole)
    {
        if(column ==0)
        {
            if(!name.isEmpty())
                return name + " : " + type->getName().c_str();
            else
                return QString(type->getName().c_str());
        }
        else if (column == 1)
            return static_cast<unsigned int>(type->getByteSize());
        else if (column == 2)
        {
            QVariant res(offset); // if offset==-1 -> leave column empty
            return offset>=0 ? res : QVariant();
        }
    }

    if( role == Qt::DecorationRole && column ==0)
    {
        RsClassType * classType = dynamic_cast<RsClassType*> (type);
        if(classType)
            return QIcon(":/util/NodeIcons/class.gif");

        RsArrayType * arrType =  dynamic_cast<RsArrayType*> (type);
        if(arrType )
            return QIcon(":/icons/array.gif");

        RsBasicType * basicType = dynamic_cast<RsBasicType*> (type);
        if(basicType)
            return QIcon(":/icons/basic_type.gif");

    }


    return QVariant();
}


RsTypeDisplay * RsTypeDisplay::build (TypeSystem * t)
{
    RsTypeDisplay * root = new RsTypeDisplay(NULL,"root",-1);

    for(TypeSystem::const_iterator it = t->begin(); it != t->end(); ++it)
    {
        root->addChild( build(*it,-1,"") );
    }

    return root;
}


RsTypeDisplay * RsTypeDisplay::build (RsType * t,
                                         int memberOffset,
                                         const QString & memberName)
{
    RsTypeDisplay * curNode = new RsTypeDisplay(t,memberName,memberOffset);

    for(int i=0; i< t->getSubtypeCount(); i++)
    {
        QString subTypeStr( t->getSubTypeString(i).c_str() );
        RsTypeDisplay * c =build(t->getSubtype(i),t->getSubtypeOffset(i),subTypeStr);
        curNode->addChild(c);
    }
    return curNode;
}
