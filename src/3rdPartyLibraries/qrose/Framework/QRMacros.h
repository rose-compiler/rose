/***************************************************************************
    Jose Gabriel de Figueiredo Coutinho
    jgfc@doc.ic.ac.uk
    Class: QRWindow
    Description:
***************************************************************************/

#ifndef QROSEMACROS_H
#define QROSEMACROS_H

#ifdef __CYGWIN__
#define Q_OS_WIN32
#endif

class QObject;

namespace qrs {
   typedef void (*CBVoidType)();
   typedef void (*CBBoolType)(bool);
   typedef void (*CBIntBoolType)(int, bool);
   typedef void (*CBIntType)(int);
   typedef void (*CBInt2Type)(int, int);
   typedef void (*CBInt3Type)(int, int, int);
   typedef void (*CBInt4Type)(int, int, int, int);
   typedef void (*CBStrType)(const char *str);
}

#define QR_CONC(x,y) x##y
#define QR_P(obj) _private->obj
#define QR_PDECL(cls) cls *_private

// for legacy ROSE components
#define QR_EXPORT(t,v) public: t* QR_CONC(getWidget,v) () { return QR_CONC(m_wg,v); } protected: t* QR_CONC(m_wg,v)

#endif
