/****************************************************************************
** Meta object code from reading C++ file 'QRWindow.h'
**
** Created: Mon Oct 20 12:09:15 2008
**      by: The Qt Meta Object Compiler version 59 (Qt 4.4.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "QRWindow.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'QRWindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.4.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_qrs__QRWindow[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets

       0        // eod
};

static const char qt_meta_stringdata_qrs__QRWindow[] = {
    "qrs::QRWindow\0"
};

const QMetaObject qrs::QRWindow::staticMetaObject = {
    { &QRTiledWidget::staticMetaObject, qt_meta_stringdata_qrs__QRWindow,
      qt_meta_data_qrs__QRWindow, 0 }
};

const QMetaObject *qrs::QRWindow::metaObject() const
{
    return &staticMetaObject;
}

void *qrs::QRWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_qrs__QRWindow))
        return static_cast<void*>(const_cast< QRWindow*>(this));
    return QRTiledWidget::qt_metacast(_clname);
}

int qrs::QRWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QRTiledWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
QT_END_MOC_NAMESPACE
