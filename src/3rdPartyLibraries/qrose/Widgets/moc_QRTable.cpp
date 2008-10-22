/****************************************************************************
** Meta object code from reading C++ file 'QRTable.h'
**
** Created: Mon Oct 20 12:09:37 2008
**      by: The Qt Meta Object Compiler version 59 (Qt 4.4.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "QRTable.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'QRTable.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.4.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_qrs__QRTable[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // signals: signature, parameters, type, tag, flags
      36,   14,   13,   13, 0x05,
      71,   63,   13,   13, 0x05,
      88,   63,   13,   13, 0x05,
     105,   63,   13,   13, 0x05,

 // slots: signature, parameters, type, tag, flags
     125,   14,   13,   13, 0x09,
     156,   63,   13,   13, 0x09,
     177,   63,   13,   13, 0x09,
     197,   63,   13,   13, 0x09,

       0        // eod
};

static const char qt_meta_stringdata_qrs__QRTable[] = {
    "qrs::QRTable\0\0col,row,oldCol,oldRow\0"
    "activated(int,int,int,int)\0col,row\0"
    "changed(int,int)\0clicked(int,int)\0"
    "dblClicked(int,int)\0activateEvent(int,int,int,int)\0"
    "changeEvent(int,int)\0clickEvent(int,int)\0"
    "dblClickEvent(int,int)\0"
};

const QMetaObject qrs::QRTable::staticMetaObject = {
    { &QTableWidget::staticMetaObject, qt_meta_stringdata_qrs__QRTable,
      qt_meta_data_qrs__QRTable, 0 }
};

const QMetaObject *qrs::QRTable::metaObject() const
{
    return &staticMetaObject;
}

void *qrs::QRTable::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_qrs__QRTable))
        return static_cast<void*>(const_cast< QRTable*>(this));
    return QTableWidget::qt_metacast(_clname);
}

int qrs::QRTable::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QTableWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: activated((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3])),(*reinterpret_cast< int(*)>(_a[4]))); break;
        case 1: changed((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 2: clicked((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 3: dblClicked((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 4: activateEvent((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3])),(*reinterpret_cast< int(*)>(_a[4]))); break;
        case 5: changeEvent((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 6: clickEvent((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 7: dblClickEvent((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        }
        _id -= 8;
    }
    return _id;
}

// SIGNAL 0
void qrs::QRTable::activated(int _t1, int _t2, int _t3, int _t4)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)), const_cast<void*>(reinterpret_cast<const void*>(&_t4)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void qrs::QRTable::changed(int _t1, int _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void qrs::QRTable::clicked(int _t1, int _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void qrs::QRTable::dblClicked(int _t1, int _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}
QT_END_MOC_NAMESPACE
