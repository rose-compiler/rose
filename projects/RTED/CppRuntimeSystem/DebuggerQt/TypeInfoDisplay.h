
#ifndef TYPEINFODISPLAY_H
#define TYPEINFODISPLAY_H

#include "ItemTreeNode.h"

class TypeSystem;
class RsType;

class RsTypeDisplay : public ItemTreeNode
{
    public:

        virtual QVariant data(int role, int column=0) const;
        virtual QStringList sectionHeader() const;

        static RsTypeDisplay * build (TypeSystem * ts);

        static RsTypeDisplay * build (const RsType* t, int memberOffset=-1, const QString & memberName="");
    protected:

        RsTypeDisplay(const RsType* type, const QString& memberName="", int offset=-1);





        const RsType * type; ///< type information
        QString name;  ///< name of member (when contained by other class)
        int offset;    ///< offset in containing type
};







#endif
