/*
 *  This file is part of Klint
 *  Copyright (C) 2001 Roberto Raggi (roberto@kdevelop.org)
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

#include "qsourcecolorizer.h"
#include "paragdata.h"
#include "qeditor.h"

/*
#include <kapplication.h>
#include <kdebug.h>
#include <kconfig.h>
#include <kglobalsettings.h>
*/

using namespace std;

namespace qrs {

QSourceColorizer::QSourceColorizer( QEditor* editor )
    : Q3TextPreProcessor(), m_editor( editor )
{
    m_items.setAutoDelete( TRUE );
/*
    QFont defaultFont = KGlobalSettings::fixedFont();
    KConfig* config = QEditorPartFactory::instance()->config();
    config->setGroup( "General" );  // or colorizer ?!?!?
*/
    QFont defaultFont("Monospace", 10);

    m_formats.clear();

    DECLARE_FORMAT_ITEM( Normal, "Normal", defaultFont, Qt::black );
    DECLARE_FORMAT_ITEM( PreProcessor, "PreProcessor", defaultFont, QColor( 0x80, 0x00, 0x80 ) ); //Qt::darkGreen
    DECLARE_FORMAT_ITEM( Keyword, "Keyword", defaultFont, QColor( 0x0e, 0x23, 0xad ) ); //QColor( 0xff, 0x77, 0x00 )
    DECLARE_FORMAT_ITEM( BuiltInClass, "Built-in Class", defaultFont, QColor( 0xff, 0x77, 0x00 ) );
    DECLARE_FORMAT_ITEM( Operator, "Operator", defaultFont, Qt::black );
    DECLARE_FORMAT_ITEM( Comment, "Comment", defaultFont, QColor( 0x06, 0x78, 0x17) ); //QColor( 0xdd, 0x00, 0x00 )
    DECLARE_FORMAT_ITEM( Constant, "Constant", defaultFont, QColor( 0x00, 0x00, 0xff ) ); //Qt::darkBlue
    DECLARE_FORMAT_ITEM( String, "String", defaultFont, QColor( 0xde, 0x19, 0x07 ) ); //QColor( 0x00, 0xaa, 0x00 )

    setSymbols( "{[(", "}])" );
}

QSourceColorizer::~QSourceColorizer()
{
    /*
    KConfig* config = QEditorPartFactory::instance()->config();
    config->setGroup( "General" );  // or colorizer ?!?!?
*/
    while( !m_formats.empty() ){
        QMap<int, QPair<QString, Q3TextFormat*> >::Iterator it = m_formats.begin();
        STORE_FORMAT_ITEM( it.key() );
        delete( (*it).second );
        m_formats.erase( it );
    }

  //  config->sync();
}


void QSourceColorizer::updateStyles( QMap<QString, QPair<QFont, QColor> >& values )
{
    //KConfig* config = QEditorPartFactory::instance()->config();
    //config->setGroup( "General" );  // or colorizer ?!?!?

    QMap<QString, QPair<QFont, QColor> >::Iterator it = values.begin();
    while( it != values.end() ){
        Q3TextFormat* fmt = formatFromId( it.key() );
        if( fmt ){
            fmt->setFont( (*it).first );
            fmt->setColor( (*it).second );
        }
        ++it;
    }

    {
        QMap<int, QPair<QString, Q3TextFormat*> >::Iterator it = m_formats.begin();
        while( it != m_formats.end() ){
            STORE_FORMAT_ITEM( it.key() );
            ++it;
        }
    }

 //   config->sync();
}


void QSourceColorizer::process( Q3TextDocument* doc, Q3TextParagraph* parag, int,
				bool invalidate )
{
    int state = 0;
    int startLevel = 0;
    int startState = 0;

    if( parag->prev() ){
	if( parag->prev()->endState() == -1 )
	    process( doc, parag->prev(), 0, FALSE );
	startState = parag->prev()->endState();
	startLevel = ((ParagData*) parag->prev()->extraData())->level();
    }
    state = startState;

    ParagData* extra = (ParagData*) parag->extraData();
    if( extra ){
	extra->clear();
    } else {
	extra = new ParagData();
	parag->setExtraData( extra );
    }

    HLItemCollection* ctx = m_items.at( state );
    QString s = m_editor->text( parag->paragId() );
    const QChar* buffer = s.unicode();
    int length = s.length();

    int pos = 0;
    while( pos < length ){
	int attr = 0;
	int next = state;

	int npos = ctx->checkHL( buffer, pos, s.length(), &attr, &next );
	if( npos > pos ){
	    state = next;
	    ctx = m_items.at( state );
	    parag->setFormat( pos, npos, format(attr) );
	    pos = npos;
	} else {
	    const QChar& ch = buffer[ pos ];
	    int a = ctx->attr();
	    if( !a ){
		if( m_left.find(ch) != -1 ){
		    extra->add( Symbol::Left, ch, pos );
		} else if( m_right.find(ch) != -1 ){
		    extra->add( Symbol::Right, ch, pos );
		}
	    }
	    parag->setFormat( pos, pos+1, format(a) );
	    ++pos;
	}
    }

    int oldState = parag->endState();

    if( state != oldState ){
	parag->setEndState( state );
    }

    int oldLevel = extra->level();
    int level = computeLevel( parag, startLevel );
    if( level != oldLevel ){
	extra->setLevel( level > 0 ? level : 0 );
    }

    parag->setFirstPreProcess( FALSE );

    Q3TextParagraph *p = parag->next();

    if( (oldLevel != level
	 || (oldState == -1 && parag->prev() && parag->endState() != parag->prev()->endState())
	 || (oldState != -1 && oldState != state))
	&& invalidate && p && !p->firstPreProcess() && p->endState() != -1 )
    {
	//kdDebug(9032) << "invalidate!!! line = " << parag->paragId() << endl;
	int count = 0;
	while ( p ) {
	    if ( p->endState() == -1 )
		return;

	    ++count;
	    p->setEndState( -1 );
	    p = p->next();
	}
	//kdDebug(9032) << "invalidated " << count+1 << " lines" << endl;
    }
}

void QSourceColorizer::insertHLItem( int id, HLItemCollection* item )
{
    m_items.insert( id, item );
}

void QSourceColorizer::setSymbols( const QString& l, const QString& r )
{
    m_left = l;
    m_right = r;
}

Q3TextFormat* QSourceColorizer::formatFromId( const QString& id )
{
    QMap<int, QPair<QString, Q3TextFormat*> >::Iterator it = m_formats.begin();
    while( it != m_formats.end() ){
        if( (*it).first == id ){
            return (*it).second;
        }
        ++it;
    }
    return 0;
}

QStringList QSourceColorizer::styleList() const
{
    QStringList lst;

    QMap<int, QPair<QString, Q3TextFormat*> >::ConstIterator it = m_formats.begin();
    while( it != m_formats.end() ){
        lst << (*it).first;
        ++it;
    }

    return lst;
}

}
