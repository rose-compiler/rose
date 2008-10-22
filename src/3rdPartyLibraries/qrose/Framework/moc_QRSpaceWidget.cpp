/****************************************************************************
** Meta object code from reading C++ file 'QRSpaceWidget.h'
**
** Created: Mon Oct 20 12:09:18 2008
**      by: The Qt Meta Object Compiler version 59 (Qt 4.4.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "QRSpaceWidget.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'QRSpaceWidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.4.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_qrs__QRSpaceWidget[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // slots: signature, parameters, type, tag, flags
      27,   20,   19,   19, 0x09,

       0        // eod
};

static const char qt_meta_stringdata_qrs__QRSpaceWidget[] = {
    "qrs::QRSpaceWidget\0\0parent\0"
    "childIsAttached(QRTiledWidget*)\0"
};

const QMetaObject qrs::QRSpaceWidget::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_qrs__QRSpaceWidget,
      qt_meta_data_qrs__QRSpaceWidget, 0 }
};

const QMetaObject *qrs::QRSpaceWidget::metaObject() const
{
    return &staticMetaObject;
}

void *qrs::QRSpaceWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_qrs__QRSpaceWidget))
        return static_cast<void*>(const_cast< QRSpaceWidget*>(this));
    return QWidget::qt_metacast(_clname);
}

int qrs::QRSpaceWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: childIsAttached((*reinterpret_cast< QRTiledWidget*(*)>(_a[1]))); break;
        }
        _id -= 1;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
