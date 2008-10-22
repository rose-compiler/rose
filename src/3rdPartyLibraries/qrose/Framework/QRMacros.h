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
   typedef void (*CBIntType)(int);
   typedef void (*CBInt2Type)(int, int);
   typedef void (*CBInt3Type)(int, int, int);
   typedef void (*CBInt4Type)(int, int, int, int);
   typedef void (*CBStrType)(const char *str);
}

#endif
