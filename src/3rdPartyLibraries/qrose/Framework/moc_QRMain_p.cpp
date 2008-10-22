/****************************************************************************
** Meta object code from reading C++ file 'QRMain_p.h'
**
** Created: Mon Oct 20 12:09:12 2008
**      by: The Qt Meta Object Compiler version 59 (Qt 4.4.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "QRMain_p.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'QRMain_p.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.4.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_qrs__QRCBVoid[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // slots: signature, parameters, type, tag, flags
      15,   14,   14,   14, 0x09,

       0        // eod
};

static const char qt_meta_stringdata_qrs__QRCBVoid[] = {
    "qrs::QRCBVoid\0\0dispatcher()\0"
};

const QMetaObject qrs::QRCBVoid::staticMetaObject = {
    { &QRCallback::staticMetaObject, qt_meta_stringdata_qrs__QRCBVoid,
      qt_meta_data_qrs__QRCBVoid, 0 }
};

const QMetaObject *qrs::QRCBVoid::metaObject() const
{
    return &staticMetaObject;
}

void *qrs::QRCBVoid::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_qrs__QRCBVoid))
        return static_cast<void*>(const_cast< QRCBVoid*>(this));
    return QRCallback::qt_metacast(_clname);
}

int qrs::QRCBVoid::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QRCallback::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: dispatcher(); break;
        }
        _id -= 1;
    }
    return _id;
}
static const uint qt_meta_data_qrs__QRCBBool[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // slots: signature, parameters, type, tag, flags
      17,   15,   14,   14, 0x09,

       0        // eod
};

static const char qt_meta_stringdata_qrs__QRCBBool[] = {
    "qrs::QRCBBool\0\0b\0dispatcher(bool)\0"
};

const QMetaObject qrs::QRCBBool::staticMetaObject = {
    { &QRCallback::staticMetaObject, qt_meta_stringdata_qrs__QRCBBool,
      qt_meta_data_qrs__QRCBBool, 0 }
};

const QMetaObject *qrs::QRCBBool::metaObject() const
{
    return &staticMetaObject;
}

void *qrs::QRCBBool::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_qrs__QRCBBool))
        return static_cast<void*>(const_cast< QRCBBool*>(this));
    return QRCallback::qt_metacast(_clname);
}

int qrs::QRCBBool::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QRCallback::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: dispatcher((*reinterpret_cast< bool(*)>(_a[1]))); break;
        }
        _id -= 1;
    }
    return _id;
}
static const uint qt_meta_data_qrs__QRCBInt[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // slots: signature, parameters, type, tag, flags
      16,   14,   13,   13, 0x09,

       0        // eod
};

static const char qt_meta_stringdata_qrs__QRCBInt[] = {
    "qrs::QRCBInt\0\0i\0dispatcher(int)\0"
};

const QMetaObject qrs::QRCBInt::staticMetaObject = {
    { &QRCallback::staticMetaObject, qt_meta_stringdata_qrs__QRCBInt,
      qt_meta_data_qrs__QRCBInt, 0 }
};

const QMetaObject *qrs::QRCBInt::metaObject() const
{
    return &staticMetaObject;
}

void *qrs::QRCBInt::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_qrs__QRCBInt))
        return static_cast<void*>(const_cast< QRCBInt*>(this));
    return QRCallback::qt_metacast(_clname);
}

int qrs::QRCBInt::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QRCallback::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: dispatcher((*reinterpret_cast< int(*)>(_a[1]))); break;
        }
        _id -= 1;
    }
    return _id;
}
static const uint qt_meta_data_qrs__QRCBInt2[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // slots: signature, parameters, type, tag, flags
      19,   15,   14,   14, 0x09,
      41,   39,   14,   14, 0x09,

       0        // eod
};

static const char qt_meta_stringdata_qrs__QRCBInt2[] = {
    "qrs::QRCBInt2\0\0i,j\0dispatcher(int,int)\0"
    "p\0dispatcher(QPoint)\0"
};

const QMetaObject qrs::QRCBInt2::staticMetaObject = {
    { &QRCallback::staticMetaObject, qt_meta_stringdata_qrs__QRCBInt2,
      qt_meta_data_qrs__QRCBInt2, 0 }
};

const QMetaObject *qrs::QRCBInt2::metaObject() const
{
    return &staticMetaObject;
}

void *qrs::QRCBInt2::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_qrs__QRCBInt2))
        return static_cast<void*>(const_cast< QRCBInt2*>(this));
    return QRCallback::qt_metacast(_clname);
}

int qrs::QRCBInt2::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QRCallback::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: dispatcher((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 1: dispatcher((*reinterpret_cast< const QPoint(*)>(_a[1]))); break;
        }
        _id -= 2;
    }
    return _id;
}
static const uint qt_meta_data_qrs__QRCBInt3[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // slots: signature, parameters, type, tag, flags
      21,   15,   14,   14, 0x09,

       0        // eod
};

static const char qt_meta_stringdata_qrs__QRCBInt3[] = {
    "qrs::QRCBInt3\0\0i,j,k\0dispatcher(int,int,int)\0"
};

const QMetaObject qrs::QRCBInt3::staticMetaObject = {
    { &QRCallback::staticMetaObject, qt_meta_stringdata_qrs__QRCBInt3,
      qt_meta_data_qrs__QRCBInt3, 0 }
};

const QMetaObject *qrs::QRCBInt3::metaObject() const
{
    return &staticMetaObject;
}

void *qrs::QRCBInt3::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_qrs__QRCBInt3))
        return static_cast<void*>(const_cast< QRCBInt3*>(this));
    return QRCallback::qt_metacast(_clname);
}

int qrs::QRCBInt3::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QRCallback::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: dispatcher((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        }
        _id -= 1;
    }
    return _id;
}
static const uint qt_meta_data_qrs__QRCBInt4[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // slots: signature, parameters, type, tag, flags
      23,   15,   14,   14, 0x09,

       0        // eod
};

static const char qt_meta_stringdata_qrs__QRCBInt4[] = {
    "qrs::QRCBInt4\0\0i,j,k,l\0"
    "dispatcher(int,int,int,int)\0"
};

const QMetaObject qrs::QRCBInt4::staticMetaObject = {
    { &QRCallback::staticMetaObject, qt_meta_stringdata_qrs__QRCBInt4,
      qt_meta_data_qrs__QRCBInt4, 0 }
};

const QMetaObject *qrs::QRCBInt4::metaObject() const
{
    return &staticMetaObject;
}

void *qrs::QRCBInt4::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_qrs__QRCBInt4))
        return static_cast<void*>(const_cast< QRCBInt4*>(this));
    return QRCallback::qt_metacast(_clname);
}

int qrs::QRCBInt4::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QRCallback::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: dispatcher((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3])),(*reinterpret_cast< int(*)>(_a[4]))); break;
        }
        _id -= 1;
    }
    return _id;
}
static const uint qt_meta_data_qrs__QRCBStr[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // slots: signature, parameters, type, tag, flags
      18,   14,   13,   13, 0x09,
      38,   14,   13,   13, 0x09,

       0        // eod
};

static const char qt_meta_stringdata_qrs__QRCBStr[] = {
    "qrs::QRCBStr\0\0str\0dispatcher(QString)\0"
    "dispatcher(const char*)\0"
};

const QMetaObject qrs::QRCBStr::staticMetaObject = {
    { &QRCallback::staticMetaObject, qt_meta_stringdata_qrs__QRCBStr,
      qt_meta_data_qrs__QRCBStr, 0 }
};

const QMetaObject *qrs::QRCBStr::metaObject() const
{
    return &staticMetaObject;
}

void *qrs::QRCBStr::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_qrs__QRCBStr))
        return static_cast<void*>(const_cast< QRCBStr*>(this));
    return QRCallback::qt_metacast(_clname);
}

int qrs::QRCBStr::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QRCallback::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: dispatcher((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 1: dispatcher((*reinterpret_cast< const char*(*)>(_a[1]))); break;
        }
        _id -= 2;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
