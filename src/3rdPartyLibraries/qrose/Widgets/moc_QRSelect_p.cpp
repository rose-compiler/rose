/****************************************************************************
** Meta object code from reading C++ file 'QRSelect_p.h'
**
** Created: Mon Oct 20 12:09:45 2008
**      by: The Qt Meta Object Compiler version 59 (Qt 4.4.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "QRSelect_p.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'QRSelect_p.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.4.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_qrs__QRSelect_p[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // slots: signature, parameters, type, tag, flags
      22,   17,   16,   16, 0x0a,
      51,   17,   16,   16, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_qrs__QRSelect_p[] = {
    "qrs::QRSelect_p\0\0item\0"
    "clickEvent(QListWidgetItem*)\0"
    "dblClickEvent(QListWidgetItem*)\0"
};

const QMetaObject qrs::QRSelect_p::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_qrs__QRSelect_p,
      qt_meta_data_qrs__QRSelect_p, 0 }
};

const QMetaObject *qrs::QRSelect_p::metaObject() const
{
    return &staticMetaObject;
}

void *qrs::QRSelect_p::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_qrs__QRSelect_p))
        return static_cast<void*>(const_cast< QRSelect_p*>(this));
    return QObject::qt_metacast(_clname);
}

int qrs::QRSelect_p::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: clickEvent((*reinterpret_cast< QListWidgetItem*(*)>(_a[1]))); break;
        case 1: dblClickEvent((*reinterpret_cast< QListWidgetItem*(*)>(_a[1]))); break;
        }
        _id -= 2;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
