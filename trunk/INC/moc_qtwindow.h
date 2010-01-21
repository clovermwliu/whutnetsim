/****************************************************************************
** QTWindow meta object code from reading C++ file 'qtwindow.h'
**
** Created: Fri Dec 15 16:14:29 2006
**      by: The Qt MOC ($Id: qt/moc_yacc.cpp   3.3.5   edited Sep 2 14:41 $)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#undef QT_NO_COMPAT
#include "qtwindow.h"
#include <qmetaobject.h>
#include <qapplication.h>

#include <private/qucomextra_p.h>
#if !defined(Q_MOC_OUTPUT_REVISION) || (Q_MOC_OUTPUT_REVISION != 26)
#error "This file was generated using the moc from 3.3.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

const char *QTWindow::className() const
{
    return "QTWindow";
}

QMetaObject *QTWindow::metaObj = 0;
static QMetaObjectCleanUp cleanUp_QTWindow( "QTWindow", &QTWindow::staticMetaObject );

#ifndef QT_NO_TRANSLATION
QString QTWindow::tr( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "QTWindow", s, c, MyApplication::DefaultCodec );
    else
	return QString::fromLatin1( s );
}
#ifndef QT_NO_TRANSLATION_UTF8
QString QTWindow::trUtf8( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "QTWindow", s, c, MyApplication::UnicodeUTF8 );
    else
	return QString::fromUtf8( s );
}
#endif // QT_NO_TRANSLATION_UTF8

#endif // QT_NO_TRANSLATION

QMetaObject* QTWindow::staticMetaObject()
{
    if ( metaObj )
	return metaObj;
    QMetaObject* parentObject = QObject::staticMetaObject();
    static const QUParameter param_slot_0[] = {
	{ 0, &static_QUType_int, 0, QUParameter::In }
    };
    static const QUMethod slot_0 = {"NewSliderValue", 1, param_slot_0 };
    static const QUMethod slot_1 = {"Save", 0, 0 };
    static const QUParameter param_slot_2[] = {
	{ 0, &static_QUType_bool, 0, QUParameter::In }
    };
    static const QUMethod slot_2 = {"Record", 1, param_slot_2 };
    static const QUMethod slot_3 = {"Play", 0, 0 };
    static const QUMethod slot_4 = {"Stop", 0, 0 };
    static const QUMethod slot_5 = {"Pause", 0, 0 };
    static const QUMethod slot_6 = {"Quit", 0, 0 };
    static const QUMethod slot_7 = {"Exit", 0, 0 };
    static const QUMethod slot_8 = {"TimerDone", 0, 0 };
    static const QMetaData slot_tbl[] = {
	{ "NewSliderValue(int)", &slot_0, QMetaData::Public },
	{ "Save()", &slot_1, QMetaData::Public },
	{ "Record(bool)", &slot_2, QMetaData::Public },
	{ "Play()", &slot_3, QMetaData::Public },
	{ "Stop()", &slot_4, QMetaData::Public },
	{ "Pause()", &slot_5, QMetaData::Public },
	{ "Quit()", &slot_6, QMetaData::Public },
	{ "Exit()", &slot_7, QMetaData::Public },
	{ "TimerDone()", &slot_8, QMetaData::Public }
    };
    metaObj = QMetaObject::new_metaobject(
	"QTWindow", parentObject,
	slot_tbl, 9,
	0, 0,
#ifndef QT_NO_PROPERTIES
	0, 0,
	0, 0,
#endif // QT_NO_PROPERTIES
	0, 0 );
    cleanUp_QTWindow.setMetaObject( metaObj );
    return metaObj;
}

void* QTWindow::qt_cast( const char* clname )
{
    if ( !qstrcmp( clname, "QTWindow" ) )
	return this;
    if ( !qstrcmp( clname, "Handler" ) )
	return (Handler*)this;
    return QObject::qt_cast( clname );
}

bool QTWindow::qt_invoke( int _id, QUObject* _o )
{
    switch ( _id - staticMetaObject()->slotOffset() ) {
    case 0: NewSliderValue((int)static_QUType_int.get(_o+1)); break;
    case 1: Save(); break;
    case 2: Record((bool)static_QUType_bool.get(_o+1)); break;
    case 3: Play(); break;
    case 4: Stop(); break;
    case 5: Pause(); break;
    case 6: Quit(); break;
    case 7: Exit(); break;
    case 8: TimerDone(); break;
    default:
	return QObject::qt_invoke( _id, _o );
    }
    return TRUE;
}

bool QTWindow::qt_emit( int _id, QUObject* _o )
{
    return QObject::qt_emit(_id,_o);
}
#ifndef QT_NO_PROPERTIES

bool QTWindow::qt_property( int id, int f, QVariant* v)
{
    return QObject::qt_property( id, f, v);
}

bool QTWindow::qt_static_property( QObject* , int , int , QVariant* ){ return FALSE; }
#endif // QT_NO_PROPERTIES
