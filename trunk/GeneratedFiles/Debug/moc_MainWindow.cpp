/****************************************************************************
** Meta object code from reading C++ file 'gui_frame.h'
**
** Created: Thu Feb 4 10:23:03 2010
**      by: The Qt Meta Object Compiler version 59 (Qt 4.3.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../INC/gui_frame.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'gui_frame.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.3.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

static const uint qt_meta_data_gui_frame[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       9,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // signals: signature, parameters, type, tag, flags
      25,   11,   10,   10, 0x05,
      48,   10,   10,   10, 0x05,
      60,   10,   10,   10, 0x05,

 // slots: signature, parameters, type, tag, flags
      79,   10,   10,   10, 0x0a,
      90,   10,   10,   10, 0x0a,
     100,   10,   10,   10, 0x0a,
     111,   10,   10,   10, 0x0a,
     121,   10,   10,   10, 0x0a,
     134,   10,   10,   10, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_gui_frame[] = {
    "gui_frame\0\0configureFile\0"
    "openConfigure(QString)\0globalfun()\0"
    "fileready(QString)\0openfile()\0newfile()\0"
    "editfile()\0testent()\0dirsetting()\0"
    "fileprocess(QString)\0"
};

const QMetaObject gui_frame::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_gui_frame,
      qt_meta_data_gui_frame, 0 }
};

const QMetaObject *gui_frame::metaObject() const
{
    return &staticMetaObject;
}

void *gui_frame::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_gui_frame))
	return static_cast<void*>(const_cast< gui_frame*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int gui_frame::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: openConfigure((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 1: globalfun(); break;
        case 2: fileready((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 3: openfile(); break;
        case 4: newfile(); break;
        case 5: editfile(); break;
        case 6: testent(); break;
        case 7: dirsetting(); break;
        case 8: fileprocess((*reinterpret_cast< QString(*)>(_a[1]))); break;
        }
        _id -= 9;
    }
    return _id;
}

// SIGNAL 0
void gui_frame::openConfigure(const QString & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void gui_frame::globalfun()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}

// SIGNAL 2
void gui_frame::fileready(QString _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
