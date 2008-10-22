/****************************************************************************
** Meta object code from reading C++ file 'QREdit.h'
**
** Created: Mon Oct 20 12:09:39 2008
**      by: The Qt Meta Object Compiler version 59 (Qt 4.4.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "QREdit.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'QREdit.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.4.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_qrs__QREdit[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // signals: signature, parameters, type, tag, flags
      18,   13,   12,   12, 0x05,

 // slots: signature, parameters, type, tag, flags
      39,   13,   12,   12, 0x09,

       0        // eod
};

static const char qt_meta_stringdata_qrs__QREdit[] = {
    "qrs::QREdit\0\0text\0changed(std::string)\0"
    "changeEvent(std::string)\0"
};

const QMetaObject qrs::QREdit::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_qrs__QREdit,
      qt_meta_data_qrs__QREdit, 0 }
};

const QMetaObject *qrs::QREdit::metaObject() const
{
    return &staticMetaObject;
}

void *qrs::QREdit::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_qrs__QREdit))
        return static_cast<void*>(const_cast< QREdit*>(this));
    return QWidget::qt_metacast(_clname);
}

int qrs::QREdit::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: changed((*reinterpret_cast< std::string(*)>(_a[1]))); break;
        case 1: changeEvent((*reinterpret_cast< std::string(*)>(_a[1]))); break;
        }
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void qrs::QREdit::changed(std::string _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
