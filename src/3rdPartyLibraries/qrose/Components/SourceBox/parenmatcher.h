 /**********************************************************************
** Copyright (C) 2000 Trolltech AS.  All rights reserved.
**
** This file is part of Qt Designer.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file COPYING included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/

#ifndef PARENMATCHER_H
#define PARENMATCHER_H

#include "q3richtext_px.h"
#include <Qt3Support>
#include <QtGui>
#include <QtCore>

namespace qrs {

class QTextCursor;

struct Paren
{
    Paren() : type( Open ), chr( ' ' ), pos( -1 ) {}
    Paren( int t, const QChar &c, int p ) : type( (Type)t ), chr( c ), pos( p ) {}
    enum Type { Open, Closed };
    Type type;
    QChar chr;
    int pos;
};

typedef Q3ValueList<Paren> ParenList;

class ParenMatcher
{
public:
    enum Selection {
	Match = 1,
	Mismatch
    };

    ParenMatcher();

    virtual bool match( Q3TextCursor *c );

    void setEnabled( bool b ) { enabled = b; }

private:
    bool checkOpenParen( Q3TextCursor *c );
    bool checkClosedParen( Q3TextCursor *c );

    bool enabled;

};

}
#endif
