/****************************************************************************
** Meta object code from reading C++ file 'MyBars.h'
**
** Created: Tue Feb 10 18:25:02 2009
**      by: The Qt Meta Object Compiler version 59 (Qt 4.4.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "rose.h"
#include "../../../ROSE-svn/projects/BinQ/MyBars.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'MyBars.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.4.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_MyBars[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // signals: signature, parameters, type, tag, flags
      14,    8,    7,    7, 0x05,

 // slots: signature, parameters, type, tag, flags
      32,    8,    7,    7, 0x0a,
      46,    8,    7,    7, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_MyBars[] = {
    "MyBars\0\0value\0valueChanged(int)\0"
    "setValue(int)\0setValueY(int)\0"
};

const QMetaObject MyBars::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_MyBars,
      qt_meta_data_MyBars, 0 }
};

const QMetaObject *MyBars::metaObject() const
{
    return &staticMetaObject;
}

void *MyBars::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_MyBars))
        return static_cast<void*>(const_cast< MyBars*>(this));
    return QWidget::qt_metacast(_clname);
}

int MyBars::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: valueChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: setValue((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: setValueY((*reinterpret_cast< int(*)>(_a[1]))); break;
        }
        _id -= 3;
    }
    return _id;
}

// SIGNAL 0
void MyBars::valueChanged(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
