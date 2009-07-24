

#ifndef MODELROLES_H_
#define MODELROLES_H_


class MemoryType;

enum { MemoryTypeRole = Qt::UserRole+1,
       VariablesTypeRole,
       SourcePosRole};

typedef MemoryType* MemoryTypePtr;
typedef VariablesType* VariablesTypePtr;

Q_DECLARE_METATYPE(MemoryTypePtr)
Q_DECLARE_METATYPE(VariablesTypePtr)

#endif
