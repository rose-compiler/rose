/****************************************************************************
** Meta object code from reading C++ file 'QRProgress.h'
**
** Created: Mon Oct 20 12:09:42 2008
**      by: The Qt Meta Object Compiler version 59 (Qt 4.4.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "QRProgress.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'QRProgress.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.4.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_qrs__QRProgress[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // signals: signature, parameters, type, tag, flags
      17,   16,   16,   16, 0x05,
      33,   27,   16,   16, 0x05,
      45,   16,   16,   16, 0x05,

 // slots: signature, parameters, type, tag, flags
      53,   16,   16,   16, 0x0a,
      66,   27,   16,   16, 0x0a,
      81,   16,   16,   16, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_qrs__QRProgress[] = {
    "qrs::QRProgress\0\0started()\0value\0"
    "ticked(int)\0ended()\0startEvent()\0"
    "tickEvent(int)\0endEvent()\0"
};

const QMetaObject qrs::QRProgress::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_qrs__QRProgress,
      qt_meta_data_qrs__QRProgress, 0 }
};

const QMetaObject *qrs::QRProgress::metaObject() const
{
    return &staticMetaObject;
}

void *qrs::QRProgress::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_qrs__QRProgress))
        return static_cast<void*>(const_cast< QRProgress*>(this));
    return QWidget::qt_metacast(_clname);
}

int qrs::QRProgress::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: started(); break;
        case 1: ticked((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: ended(); break;
        case 3: startEvent(); break;
        case 4: tickEvent((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 5: endEvent(); break;
        }
        _id -= 6;
    }
    return _id;
}

// SIGNAL 0
void qrs::QRProgress::started()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}

// SIGNAL 1
void qrs::QRProgress::ticked(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void qrs::QRProgress::ended()
{
    QMetaObject::activate(this, &staticMetaObject, 2, 0);
}
QT_END_MOC_NAMESPACE
