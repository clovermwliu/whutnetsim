/****************************************************************************
** Meta object code from reading C++ file 'gui_ctrl.h'
**
** Created: Thu Feb 4 10:23:03 2010
**      by: The Qt Meta Object Compiler version 59 (Qt 4.3.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../INC/gui_ctrl.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'gui_ctrl.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.3.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

static const uint qt_meta_data_gui_ctrl[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
      10,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // slots: signature, parameters, type, tag, flags
      10,    9,    9,    9, 0x0a,
      30,    9,    9,    9, 0x0a,
      37,    9,    9,    9, 0x0a,
      50,    9,    9,    9, 0x0a,
      57,    9,    9,    9, 0x0a,
      64,    9,    9,    9, 0x0a,
      72,    9,    9,    9, 0x0a,
      79,    9,    9,    9, 0x0a,
      86,    9,    9,    9, 0x0a,
      98,    9,    9,    9, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_gui_ctrl[] = {
    "gui_ctrl\0\0NewSliderValue(int)\0Save()\0"
    "Record(bool)\0Play()\0Stop()\0Pause()\0"
    "Quit()\0Exit()\0TimerDone()\0newtask(QString)\0"
};

const QMetaObject gui_ctrl::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_gui_ctrl,
      qt_meta_data_gui_ctrl, 0 }
};

const QMetaObject *gui_ctrl::metaObject() const
{
    return &staticMetaObject;
}

void *gui_ctrl::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_gui_ctrl))
	return static_cast<void*>(const_cast< gui_ctrl*>(this));
    if (!strcmp(_clname, "Handler"))
	return static_cast< Handler*>(const_cast< gui_ctrl*>(this));
    return QObject::qt_metacast(_clname);
}

int gui_ctrl::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: NewSliderValue((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: Save(); break;
        case 2: Record((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 3: Play(); break;
        case 4: Stop(); break;
        case 5: Pause(); break;
        case 6: Quit(); break;
        case 7: Exit(); break;
        case 8: TimerDone(); break;
        case 9: newtask((*reinterpret_cast< QString(*)>(_a[1]))); break;
        }
        _id -= 10;
    }
    return _id;
}
