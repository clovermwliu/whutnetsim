/****************************************************************************
** Meta object code from reading C++ file 'qtwindow.h'
**
** Created: Wed Jan 20 18:13:39 2010
**      by: The Qt Meta Object Compiler version 59 (Qt 4.3.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../INC/qtwindow.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qtwindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.3.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

static const uint qt_meta_data_QTWindow[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
      11,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // signals: signature, parameters, type, tag, flags
      10,    9,    9,    9, 0x05,

 // slots: signature, parameters, type, tag, flags
      26,    9,    9,    9, 0x0a,
      46,    9,    9,    9, 0x0a,
      53,    9,    9,    9, 0x0a,
      66,    9,    9,    9, 0x0a,
      73,    9,    9,    9, 0x0a,
      80,    9,    9,    9, 0x0a,
      88,    9,    9,    9, 0x0a,
      95,    9,    9,    9, 0x0a,
     102,    9,    9,    9, 0x0a,
     114,    9,    9,    9, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_QTWindow[] = {
    "QTWindow\0\0closeqtwindow()\0NewSliderValue(int)\0"
    "Save()\0Record(bool)\0Play()\0Stop()\0"
    "Pause()\0Quit()\0Exit()\0TimerDone()\0"
    "closeEvent(QCloseEvent*)\0"
};

const QMetaObject QTWindow::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_QTWindow,
      qt_meta_data_QTWindow, 0 }
};

const QMetaObject *QTWindow::metaObject() const
{
    return &staticMetaObject;
}

void *QTWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QTWindow))
	return static_cast<void*>(const_cast< QTWindow*>(this));
    if (!strcmp(_clname, "Handler"))
	return static_cast< Handler*>(const_cast< QTWindow*>(this));
    return QObject::qt_metacast(_clname);
}

int QTWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: closeqtwindow(); break;
        case 1: NewSliderValue((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: Save(); break;
        case 3: Record((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 4: Play(); break;
        case 5: Stop(); break;
        case 6: Pause(); break;
        case 7: Quit(); break;
        case 8: Exit(); break;
        case 9: TimerDone(); break;
        case 10: closeEvent((*reinterpret_cast< QCloseEvent*(*)>(_a[1]))); break;
        }
        _id -= 11;
    }
    return _id;
}

// SIGNAL 0
void QTWindow::closeqtwindow()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}
