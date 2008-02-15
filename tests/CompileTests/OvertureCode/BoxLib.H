/*
** This software is copyright (C) by the Lawrence Berkeley National
** Laboratory.  Permission is granted to reproduce this software for
** non-commercial purposes provided that this notice is left intact.
**  
** It is acknowledged that the U.S. Government has rights to this
** software under Contract DE-AC03-765F00098 between the U.S.  Department
** of Energy and the University of California.
**  
** This software is provided as a professional and academic contribution
** for joint exchange. Thus it is experimental, is provided ``as is'',
** with no warranties of any kind whatsoever, no support, no promise of
** updates, or printed documentation. By using this software, you
** acknowledge that the Lawrence Berkeley National Laboratory and Regents
** of the University of California shall have no liability with respect
** to the infringement of other copyrights by any part of this software.
**  
** For further information about this software, contact:
** 
**         Dr. John Bell
**         Bldg. 50D, Rm. 129,
**         Lawrence Berkeley National Laboratory
**         Berkeley, CA, 94720
**         jbbell@lbl.gov
*/

#ifndef _BOXLIB_H_
#define _BOXLIB_H_

//
// $Id: BoxLib.H,v 1.1 2004/07/07 10:27:01 dquinlan Exp $
//

//
//@Man:
//@Memo: Error-reporting Functions and Version Number
/*@Doc:

  This class consists entirely of static error-reporting functions that
  are used throughout the BoxLib implementation together with a version
  string.  In the best of all possible worlds we'd use a namespace, but
  we aren't there, yet :-)  Note that all the output functions add a
  terminating exclamation mark, so there is no need to add any punction
  to sentences that get output explicitly.
*/

class BoxLib
{
protected:
    //
    // A null string used as a default argument by the following functions.
    //
    static const char* nullString;
public:
    //
    //@ManDoc: Print out message to cerr and exit via BoxLib::Abort().
    //
    static void Error (const char * msg = nullString);
    //
    //@ManDoc: Print out warning message to cerr.
    //
    static void Warning (const char * msg = nullString);
    //
    //@ManDoc: Print out message to cerr and exit via abort().
    //
    static void Abort (const char * msg = nullString);

    /*@ManDoc: Prints assertion failed messages to cerr and exits
               via abort().  Intended for use by the boxAssert() macro
               in <Assert.H>.
    */
    static void Assert (const char* EX,
                        const char* file,
                        int         line);

    /*@ManDoc: Prints out an out-of-memory message and aborts.  It is
             called by various BoxLib routines when a call to new fails.

             Called as BoxLib::OutOfMemory(\_\_FILE\_\_, \_\_LINE\_\_);
    */
    static void OutOfMemory (const char* file,
                             int         line);

    /*@ManDoc: The BoxLib version string.
    
       Has the form:
    
         boxlib version 2.0 built Jun 25 1996 at 14:52:36
   */
    static const char * const version;
};

#endif /*_BOXLIB_H_*/
