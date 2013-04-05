#ifndef ROSE_Project_CodeProperties_H
#define ROSE_Project_CodeProperties_H

#include <stdarg.h>

namespace CodeProperties {

void message(std::ostream &out, const SgLocatedNode *node, const char *fmt, ...) {
    static const int COL_OFFSET = -1;           // Use -1 for Emacs, where columns are zero-origin (ROSE uses 1-origin)

    Sg_File_Info *where = node->get_startOfConstruct();
    if (where) {
        std::string filename = where->get_filename();
        // Use only the base name of the file so our test harness isn't confused by the location of the ROSE source tree.
        size_t slash = filename.find_last_of('/');
        if (slash!=std::string::npos)
            filename = filename.substr(slash+1);
        out <<(filename.empty() ? "__undefined__" : filename) <<":";

        if (where->get_line()>0) { // 1-origin
            out <<":" <<where->get_line();
            if (where->get_col()>0) // 1-origin
                out <<"." <<where->get_col()+COL_OFFSET;
            out <<":";
        }
        out <<" ";
    }

    va_list ap;
    char buf[256];
    va_start(ap, fmt);
    int nchars = vsnprintf(buf, sizeof buf, fmt, ap);
    va_end(ap);

    assert(nchars>=0);
    if ((size_t)nchars<sizeof buf) {
        out <<buf <<(!buf[0] || '\n'!=buf[strlen(buf)-1] ? "\n" : "");
    } else {
        char *buf = new char[nchars+1];
        va_start(ap, fmt);
        vsnprintf(buf, nchars+1, fmt, ap);
        va_end(ap);
        out <<buf <<(!buf[0] || '\n'!=buf[strlen(buf)-1] ? "\n" : "");
        delete[] buf;
    }
}
    
} // namespace

#endif
