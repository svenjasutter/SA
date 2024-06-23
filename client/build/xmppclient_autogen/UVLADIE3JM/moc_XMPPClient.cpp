/****************************************************************************
** Meta object code from reading C++ file 'XMPPClient.hpp'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.4.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../src/XMPPClient.hpp"
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'XMPPClient.hpp' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.4.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
namespace {
struct qt_meta_stringdata_XMPPClient_t {
    uint offsetsAndSizes[18];
    char stringdata0[11];
    char stringdata1[12];
    char stringdata2[1];
    char stringdata3[18];
    char stringdata4[13];
    char stringdata5[8];
    char stringdata6[19];
    char stringdata7[14];
    char stringdata8[9];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_XMPPClient_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_XMPPClient_t qt_meta_stringdata_XMPPClient = {
    {
        QT_MOC_LITERAL(0, 10),  // "XMPPClient"
        QT_MOC_LITERAL(11, 11),  // "OnConnected"
        QT_MOC_LITERAL(23, 0),  // ""
        QT_MOC_LITERAL(24, 17),  // "OnMessageReceived"
        QT_MOC_LITERAL(42, 12),  // "QXmppMessage"
        QT_MOC_LITERAL(55, 7),  // "message"
        QT_MOC_LITERAL(63, 18),  // "OnPresenceReceived"
        QT_MOC_LITERAL(82, 13),  // "QXmppPresence"
        QT_MOC_LITERAL(96, 8)   // "presence"
    },
    "XMPPClient",
    "OnConnected",
    "",
    "OnMessageReceived",
    "QXmppMessage",
    "message",
    "OnPresenceReceived",
    "QXmppPresence",
    "presence"
};
#undef QT_MOC_LITERAL
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_XMPPClient[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,   32,    2, 0x0a,    1 /* Public */,
       3,    1,   33,    2, 0x0a,    2 /* Public */,
       6,    1,   36,    2, 0x0a,    4 /* Public */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 4,    5,
    QMetaType::Void, 0x80000000 | 7,    8,

       0        // eod
};

Q_CONSTINIT const QMetaObject XMPPClient::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_XMPPClient.offsetsAndSizes,
    qt_meta_data_XMPPClient,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_XMPPClient_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<XMPPClient, std::true_type>,
        // method 'OnConnected'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'OnMessageReceived'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QXmppMessage &, std::false_type>,
        // method 'OnPresenceReceived'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QXmppPresence &, std::false_type>
    >,
    nullptr
} };

void XMPPClient::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<XMPPClient *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->OnConnected(); break;
        case 1: _t->OnMessageReceived((*reinterpret_cast< std::add_pointer_t<QXmppMessage>>(_a[1]))); break;
        case 2: _t->OnPresenceReceived((*reinterpret_cast< std::add_pointer_t<QXmppPresence>>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
        case 1:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QXmppMessage >(); break;
            }
            break;
        }
    }
}

const QMetaObject *XMPPClient::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *XMPPClient::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_XMPPClient.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int XMPPClient::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
