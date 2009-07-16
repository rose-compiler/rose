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


/*
 *  Copyright (C) 2002 Roberto Raggi (roberto@kdevelop.org)
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; see the file COPYING.  If not, write to
 *  the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 *  Boston, MA 02111-1307, USA.
 *
 */

#include "qeditor_indenter.h"
#include "qeditor.h"

namespace qrs {

QEditorIndenter::QEditorIndenter( QEditor* ed )
    : m_editor( ed )
{
}

QEditorIndenter::~QEditorIndenter()
{
}

int QEditorIndenter::indentation( const QString &s )
{
    if ( s.simplifyWhiteSpace().length() == 0 )
        return 0;

    int tabwidth = m_editor->tabStop();
    int i = 0;
    int ind = 0;
    while ( i < (int)s.length() ) {
        QChar c = s.at( i );
        if ( c == ' ' ){
            ind++;
        } else if ( c == '\t' ){
            ind += tabwidth;
        } else {
            break;
        }
        ++i;
    }
    return ind;
}

void QEditorIndenter::indentLine( Q3TextParagraph *p, int &oldIndent, int &newIndent )
{
    QString indentString;
    indentString.fill( ' ', newIndent );
    indentString.append( "a" );
    tabify( indentString );
    indentString.remove( indentString.length() - 1, 1 );
    newIndent = indentString.length();
    oldIndent = 0;
    while ( p->length() > 0 && ( p->at( 0 )->c == ' ' || p->at( 0 )->c == '\t' ) ) {
        ++oldIndent;
        p->remove( 0, 1 );
    }
    if ( p->string()->length() == 0 )
        p->append( " " );
    if ( !indentString.isEmpty() )
        p->insert( 0, indentString );
}

void QEditorIndenter::tabify( QString& s )
{
    int i = 0;
    int tabSize = m_editor->tabStop();
    for ( ;; ) {
        for ( int j = i; j < (int)s.length(); ++j ) {
            if ( s[ j ] != ' ' && s[ j ] != '\t' ) {
                if ( j > i ) {
                    QString t  = s.mid( i, j - i );
                    int spaces = 0;
                    for ( int k = 0; k < (int)t.length(); ++k )
                        spaces += ( t[ k ] == ' ' ? 1 : tabSize );
                    s.remove( i, t.length() );
                    int tabs = spaces / tabSize;
                    spaces = spaces - ( tabSize * tabs );
                    QString tmp;
                    tmp.fill( ' ', spaces );
                    if ( spaces > 0 )
                        s.insert( i, tmp );
                    tmp.fill( '\t', tabs );
                    if ( tabs > 0 )
                        s.insert( i, tmp );
                }
                break;
            }
        }
        i = s.find( '\n', i );
        if ( i == -1 )
            break;
        ++i;
    }
}

void QEditorIndenter::indent( Q3TextDocument*, Q3TextParagraph* parag, int* oldIndent, int* newIndent )
{
    int line = parag->paragId();
    QString text = m_editor->text( line );

    int oi = indentation( text );

    int ind = indentForLine( line );

    indentLine( parag, oi, ind );

    if( oldIndent ) *oldIndent = oi;
    if( newIndent ) *newIndent = ind;
}

int QEditorIndenter::previousNonBlankLine( int line )
{
    while( --line >=0 ){
	if( !editor()->text( line ).stripWhiteSpace().isEmpty() )
	    break;
    }
    return line;
}

}
