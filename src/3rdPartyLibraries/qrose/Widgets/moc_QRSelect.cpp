/****************************************************************************
** Meta object code from reading C++ file 'QRSelect.h'
**
** Created: Mon Oct 20 12:09:41 2008
**      by: The Qt Meta Object Compiler version 59 (Qt 4.4.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "QRSelect.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'QRSelect.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.4.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_qrs__QRSelect[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // signals: signature, parameters, type, tag, flags
      21,   15,   14,   14, 0x05,
      35,   15,   14,   14, 0x05,
      48,   15,   14,   14, 0x05,

 // slots: signature, parameters, type, tag, flags
      64,   15,   14,   14, 0x0a,
      81,   15,   14,   14, 0x0a,
      97,   15,   14,   14, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_qrs__QRSelect[] = {
    "qrs::QRSelect\0\0index\0selected(int)\0"
    "clicked(int)\0dblClicked(int)\0"
    "selectEvent(int)\0clickEvent(int)\0"
    "dblClickEvent(int)\0"
};

const QMetaObject qrs::QRSelect::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_qrs__QRSelect,
      qt_meta_data_qrs__QRSelect, 0 }
};

const QMetaObject *qrs::QRSelect::metaObject() const
{
    return &staticMetaObject;
}

void *qrs::QRSelect::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_qrs__QRSelect))
        return static_cast<void*>(const_cast< QRSelect*>(this));
    return QWidget::qt_metacast(_clname);
}

int qrs::QRSelect::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: selected((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: clicked((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: dblClicked((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: selectEvent((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 4: clickEvent((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 5: dblClickEvent((*reinterpret_cast< int(*)>(_a[1]))); break;
        }
        _id -= 6;
    }
    return _id;
}

// SIGNAL 0
void qrs::QRSelect::selected(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void qrs::QRSelect::clicked(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void qrs::QRSelect::dblClicked(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
QT_END_MOC_NAMESPACE
