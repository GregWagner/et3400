/****************************************************************************
** Meta object code from reading C++ file 'disassembly_view.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.9.7)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../src/widgets/disassembly_view.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'disassembly_view.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.9.7. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_DisassemblyView_t {
    QByteArrayData data[14];
    char stringdata0[147];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_DisassemblyView_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_DisassemblyView_t qt_meta_stringdata_DisassemblyView = {
    {
QT_MOC_LITERAL(0, 0, 15), // "DisassemblyView"
QT_MOC_LITERAL(1, 16, 8), // "onScroll"
QT_MOC_LITERAL(2, 25, 0), // ""
QT_MOC_LITERAL(3, 26, 5), // "steps"
QT_MOC_LITERAL(4, 32, 15), // "onOffsetUpdated"
QT_MOC_LITERAL(5, 48, 6), // "offset"
QT_MOC_LITERAL(6, 55, 6), // "onSize"
QT_MOC_LITERAL(7, 62, 3), // "max"
QT_MOC_LITERAL(8, 66, 15), // "onAddBreakpoint"
QT_MOC_LITERAL(9, 82, 6), // "offs_t"
QT_MOC_LITERAL(10, 89, 7), // "address"
QT_MOC_LITERAL(11, 97, 18), // "onRemoveBreakpoint"
QT_MOC_LITERAL(12, 116, 23), // "onAddorRemoveBreakpoint"
QT_MOC_LITERAL(13, 140, 6) // "redraw"

    },
    "DisassemblyView\0onScroll\0\0steps\0"
    "onOffsetUpdated\0offset\0onSize\0max\0"
    "onAddBreakpoint\0offs_t\0address\0"
    "onRemoveBreakpoint\0onAddorRemoveBreakpoint\0"
    "redraw"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_DisassemblyView[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       6,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   49,    2, 0x06 /* Public */,
       4,    1,   52,    2, 0x06 /* Public */,
       6,    1,   55,    2, 0x06 /* Public */,
       8,    1,   58,    2, 0x06 /* Public */,
      11,    1,   61,    2, 0x06 /* Public */,
      12,    1,   64,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      13,    0,   67,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::Int,    3,
    QMetaType::Void, QMetaType::Int,    5,
    QMetaType::Void, QMetaType::Int,    7,
    QMetaType::Void, 0x80000000 | 9,   10,
    QMetaType::Void, 0x80000000 | 9,   10,
    QMetaType::Void, 0x80000000 | 9,   10,

 // slots: parameters
    QMetaType::Void,

       0        // eod
};

void DisassemblyView::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        DisassemblyView *_t = static_cast<DisassemblyView *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->onScroll((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: _t->onOffsetUpdated((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: _t->onSize((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: _t->onAddBreakpoint((*reinterpret_cast< offs_t(*)>(_a[1]))); break;
        case 4: _t->onRemoveBreakpoint((*reinterpret_cast< offs_t(*)>(_a[1]))); break;
        case 5: _t->onAddorRemoveBreakpoint((*reinterpret_cast< offs_t(*)>(_a[1]))); break;
        case 6: _t->redraw(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            typedef void (DisassemblyView::*_t)(int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&DisassemblyView::onScroll)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (DisassemblyView::*_t)(int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&DisassemblyView::onOffsetUpdated)) {
                *result = 1;
                return;
            }
        }
        {
            typedef void (DisassemblyView::*_t)(int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&DisassemblyView::onSize)) {
                *result = 2;
                return;
            }
        }
        {
            typedef void (DisassemblyView::*_t)(offs_t );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&DisassemblyView::onAddBreakpoint)) {
                *result = 3;
                return;
            }
        }
        {
            typedef void (DisassemblyView::*_t)(offs_t );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&DisassemblyView::onRemoveBreakpoint)) {
                *result = 4;
                return;
            }
        }
        {
            typedef void (DisassemblyView::*_t)(offs_t );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&DisassemblyView::onAddorRemoveBreakpoint)) {
                *result = 5;
                return;
            }
        }
    }
}

const QMetaObject DisassemblyView::staticMetaObject = {
    { &QFrame::staticMetaObject, qt_meta_stringdata_DisassemblyView.data,
      qt_meta_data_DisassemblyView,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *DisassemblyView::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *DisassemblyView::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_DisassemblyView.stringdata0))
        return static_cast<void*>(this);
    return QFrame::qt_metacast(_clname);
}

int DisassemblyView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QFrame::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 7)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 7)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 7;
    }
    return _id;
}

// SIGNAL 0
void DisassemblyView::onScroll(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void DisassemblyView::onOffsetUpdated(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void DisassemblyView::onSize(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void DisassemblyView::onAddBreakpoint(offs_t _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void DisassemblyView::onRemoveBreakpoint(offs_t _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void DisassemblyView::onAddorRemoveBreakpoint(offs_t _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
