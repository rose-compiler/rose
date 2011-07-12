#include "TypeInfoDisplay.h"
#include "TypeSystem.h"

#include <QIcon>
#include <QDebug>

RsTypeDisplay::RsTypeDisplay(const RsType* t, const QString & n, int off)
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
        const RsClassType* classType = dynamic_cast<const RsClassType*> (type);
        if(classType)
            return QIcon(":/util/NodeIcons/class.gif");

        const RsArrayType* arrType =  dynamic_cast<const RsArrayType*> (type);
        if(arrType )
            return QIcon(":/icons/array.gif");

        const RsBasicType * basicType = dynamic_cast<const RsBasicType*> (type);
        if(basicType)
            return QIcon(":/icons/basic_type.gif");

    }


    return QVariant();
}


RsTypeDisplay* RsTypeDisplay::build(TypeSystem * t)
{
    typedef TypeSystem::NamedTypeContainer::const_iterator ConstIterator;

    RsTypeDisplay * root = new RsTypeDisplay(NULL,"root",-1);

    for (ConstIterator it = t->begin(); it != t->end(); ++it)
    {
        root->addChild( build(it->second,-1,"") );
    }

    return root;
}


RsTypeDisplay * RsTypeDisplay::build (const RsType* t, int memberOffset, const QString& memberName)
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
