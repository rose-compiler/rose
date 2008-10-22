/***************************************************************************
    Jose Gabriel de Figueiredo Coutinho                                   
    jgfc@doc.ic.ac.uk                                                     
    Class: QRException     
    Description: Exception class
 ***************************************************************************/

#ifndef QREXCEPTIONCLASS_H
#define QREXCEPTIONCLASS_H

#include <string>
#include <stdarg.h>

// eAssert checks whether condition is true, otherwise it issues an exception. The second
// argument builds a message using printf-style arguments, and must be enclosed by parenthesis.
// e.g. eAssert(x < 5, ("error - invalid x (%d). it must be less than 5!", x));
#define eAssert(cond,err) if (!(cond)) { QRException q(__LINE__, __FILE__, #cond); q.msg err; throw q; } else

namespace qrs {

// this class is used to build a stack of messages to track
// call stack
class QRException {
public:
	// costructor is used to build the first message using printf-style arguments
	QRException(const char *msg, ...);

	// specifies the line and source-file where exception is thrown. Unlike the above
   // costructor, it does not build a message. To build a message, use msg(...).
   QRException(int line, const char *source_file, const char *condition);

	// this method is used to build subsequent messages
   void msg(const char *msg, ...);

            
protected:
   void  post_msg(int line, const char *source_file, const char *condition, const char *msg, va_list ap);
           
protected:
   int m_count; // counts the number of messages

};

}

#endif
