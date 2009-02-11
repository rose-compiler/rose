/***************************************************************************
    Jose Gabriel de Figueiredo Coutinho                                   
    jgfc@doc.ic.ac.uk                                                     
    Class: QRException     
 ***************************************************************************/

#include <QRException.h>

#include <string>
#include <exception>
    
using namespace std;

namespace qrs {


QRException::QRException(const char *msg, ...) {
   fprintf(stderr, "QROSE EXCEPTION\n");
   m_count = 0;
   va_list ap;
   va_start(ap, msg);
   post_msg(-1, 0, 0, msg, ap);
   va_end(ap);
}

	
QRException::QRException(int line, const char *source_file, const char *condition) {
   if (string(condition) == "0")
     fprintf(stderr, "QROSE EXCEPTION: [line %d - %s]\n", line, source_file);
   else
      fprintf(stderr, "QROSE EXCEPTION: %s [line %d - %s]\n", condition, line, source_file);      
   m_count = 0;   
}

  
void QRException::msg(const char *msg, ...) {
    va_list ap;
    va_start(ap, msg);
    post_msg(-1, 0, 0, msg, ap);
    va_end(ap);
}

void QRException::post_msg(int line, const char *source_file, const char *condition, const char *msg, va_list ap) {
    fprintf(stderr, "%*c + ", (m_count +1) * 2 ,' ');
    vfprintf(stderr, msg, ap);
    fprintf(stderr, "\n");
    m_count++;
}



} // namespace qrs

