/*
        Copyright (C) 2004-2005 Cory Nelson

        This software is provided 'as-is', without any express or implied
        warranty.  In no event will the authors be held liable for any damages
        arising from the use of this software.

        Permission is granted to anyone to use this software for any purpose,
        including commercial applications, and to alter it and redistribute it
        freely, subject to the following restrictions:

        1. The origin of this software must not be misrepresented; you must not
                claim that you wrote the original software. If you use this software
                in a product, an acknowledgment in the product documentation would be
                appreciated but is not required.
        2. Altered source versions must be plainly marked as such, and must not be
                misrepresented as being the original software.
        3. This notice may not be removed or altered from any source distribution.
        
        CVS Info :
                $Author: phrostbyte $
                $Date: 2005/06/16 20:46:40 $
                $Revision: 1.1 $
*/

#include <cassert>
#include <sqlite3.h>
#include "sqlite3x.h"

namespace sqlite3x {

// Altered by ROSE Team [Robb P. Matzke 2013-04-05]
sqlite3_transaction::sqlite3_transaction(sqlite3_connection &con, LockType lt, DestMode dm)
    : con(con), intrans(LOCK_NONE), destmode(DEST_ROLLBACK) {
    begin(lt, dm);
}

// Altered by ROSE Team [Robb P. Matzke 2013-04-05]
sqlite3_transaction::~sqlite3_transaction() {
    if (intrans!=LOCK_NONE) {
        try {
            switch (destmode) {
                case DEST_ROLLBACK:
                    rollback();
                    break;
                case DEST_COMMIT:
                    commit();
                    break;
            }
        } catch(...) {
            return;
        }
    }
}

// Altered by ROSE Team [Robb P. Matzke 2013-04-05]
void
sqlite3_transaction::begin(LockType lt, DestMode dm)
{
    assert(intrans==LOCK_NONE);
    switch (lt) {
        case LOCK_NONE:
            break;
        case LOCK_SHARED:
            con.executenonquery("begin;");
            break;
        case LOCK_IMMEDIATE:
            con.executenonquery("begin immediate;");
            break;
        case LOCK_EXCLUSIVE:
            con.executenonquery("begin exclusive;");
            break;
    }
    intrans = lt;
    destmode = dm;
}

// Altered by ROSE Team [Robb P. Matzke 2013-04-05]
void
sqlite3_transaction::commit()
{
    con.executenonquery("commit;");
    intrans = LOCK_NONE;
}

// Altered by ROSE Team [Robb P. Matzke 2013-04-05]
void
sqlite3_transaction::rollback()
{
    con.executenonquery("rollback;");
    intrans = LOCK_NONE;
}

}
