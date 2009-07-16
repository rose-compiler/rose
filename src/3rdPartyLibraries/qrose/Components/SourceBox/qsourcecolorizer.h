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
 *  Library General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; see the file COPYING.  If not, write to
 *  the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 *  Boston, MA 02111-1307, USA.
 *
 */

#ifndef qsourcecolorizer_h
#define qsourcecolorizer_h

#include "q3richtext_px.h"
#include <Qt3Support>
#include <QtGui>
#include <QtCore>

namespace qrs {

#define DECLARE_FORMAT_ITEM(type, id, f, c)\
{\
    QFont font = f; \
    QColor color = c; \
    m_formats.insert( type, qMakePair(QString(id), new Q3TextFormat(font, color)) ); \
}

#define UPDATE_FORMAT_ITEM(type, f, c)\
    m_formats[ type ].second->setFont( f ); \
    m_formats[ type ].second->setColor( c );

#define STORE_FORMAT_ITEM(type)\
{\
    QString id = m_formats[ type ].first; \
    Q3TextFormat* fmt = m_formats[ type ].second; \
}

class QEditor;

class HLItem{
public:
    HLItem( int state=0, int context=0 )
        : m_state( state ), m_context( context ) {}
    virtual ~HLItem() {}

    virtual int attr() const { return m_state; }
    virtual int context() const { return m_context; }

    virtual int checkHL( const QChar* buffer, int pos, int length, int*, int* ) = 0;

private:
    int m_state;
    int m_context;
};

class NumberHLItem: public HLItem{
public:
    NumberHLItem( int state, int context )
        : HLItem( state, context ) {}

    int checkHL( const QChar* buffer, int pos, int length, int*, int* ){
	//kdDebug(9032) << "NumberHLItem::checkHLItem" << endl;
	while( pos<length && buffer[pos].isNumber() ){
	    ++pos;
	}
	return pos;
    }
};

class WhiteSpacesHLItem: public HLItem{
public:
    WhiteSpacesHLItem( int state, int context )
        : HLItem( state, context ) {}

    int checkHL( const QChar* buffer, int pos, int length, int*, int* ){
	//kdDebug(9032) << "WhiteSpacesHLItem::checkHLItem" << endl;
	while( (pos<length) && buffer[pos].isSpace() ){
	    ++pos;
	}
	return pos;
    }
};

class KeywordsHLItem: public HLItem{
public:
    KeywordsHLItem( const char** keywords, int state, int ide_state, int context, bool finalize = true, bool ignoreCase = false )
        : HLItem( state, context ), m_ok(false), m_state(state), m_ide_state(ide_state), m_finalize(finalize), m_ignoreCase(ignoreCase) {
            int i = 1;
	    if ( ignoreCase ) {
		while( *keywords )
		    m_keywords.insert( QString(*keywords++).lower(), i++ );
	    } else {
		while( *keywords )
		    m_keywords.insert( QString(*keywords++), i++ );
            }
    }

    KeywordsHLItem( const QMap<QString, int> keywords, int state, int ide_state, int context, bool finalize = true, bool ignoreCase = false )
	: HLItem( state, context ), m_ok(false), m_state(state), m_ide_state(ide_state), m_finalize(finalize), m_ignoreCase(ignoreCase) {
	    m_keywords = keywords;
    }

    int attr() const { return m_ok ? m_state : m_ide_state; }

    int checkHL( const QChar* buffer, int pos, int length, int*, int* ){
	//kdDebug(9032) << "KeywordsHLItem::checkHLItem" << endl;

	int start_pos = pos;

	while( (pos<length) && (buffer[pos].isLetterOrNumber() || buffer[pos] == '_') )
	    ++pos;

	if( start_pos != pos ) {
	    if ( m_ignoreCase ) {
	    	m_ok = m_keywords.contains( QString(buffer+start_pos, pos-start_pos).lower() );
	    } else {
		m_ok = m_keywords.contains( QString(buffer+start_pos, pos-start_pos) );
	    }
	}

	return ( m_ok || m_finalize ) ? pos : start_pos;
    }

private:
    QMap<QString, int> m_keywords;
    bool m_ok;
    int m_state;
    int m_ide_state;
    bool m_finalize; //!< setting finalize to false allows to have another KeywordsHLItem in HLItemCollection after this one
    bool m_ignoreCase;
};

class StartsWithHLItem: public HLItem{
public:
    StartsWithHLItem( const QString& s, int state, int context )
        : HLItem( state, context ), m_text(s) {}

    int checkHL( const QChar* buffer, int pos, int length, int*, int* ){
	//kdDebug(9032) << "StartsWithHLItem::checkHLItem" << endl;
	if( (length - pos) >= (int)m_text.length() && QString(buffer+pos, m_text.length()) == m_text )
	    return length;

	return pos;
    }

private:
    QString m_text;
};

class StringHLItem: public HLItem{
public:
    StringHLItem( const QString& text, int state, int context )
        : HLItem( state, context ), m_text(text) {}

    int checkHL( const QChar* buffer, int pos, int length, int*, int* ){
        //kdDebug(9032) << "StringHLItem::checkHLItem" << endl;
	if( (length - pos) >= (int)m_text.length() && QString(buffer+pos, m_text.length()) == m_text )
	   return pos + m_text.length();

	return pos;
    }

private:
    QString m_text;
};

class RegExpHLItem: public HLItem{
public:
    RegExpHLItem( QString pattern, int state, int context )
        : HLItem( state, context ), m_rx( pattern ) {}

    int checkHL( const QChar* buffer, int pos, int length, int*, int* ){
	//kdDebug(9032) << "RegExpHLItem::checkHLItem" << endl;
	QString s( buffer, length );

	int idx = m_rx.search(s, pos);
	if( idx == pos )
	    return pos + m_rx.matchedLength();

	return pos;
    }

private:
    QRegExp m_rx;
};

class HexHLItem: public HLItem{
public:
    HexHLItem( int state, int context )
        : HLItem( state, context ) {}

    int checkHL( const QChar* buffer, int pos, int length, int*, int* ){
	if( (length-pos) > 2 ){
	    QString s( buffer+pos,2 );
	    if( s == "0x" || s == "0X" ){
		pos += 2;
		while( pos < length ){
		    const QChar& ch = buffer[ pos ];
		    if( ch.isNumber() || (ch >= 'a' && ch <= 'f') || (ch >= 'A' && ch <= 'F') )
			++pos;
		    else
			break;
		}

	    }
	}
	return pos;
    }
};

class HLItemCollection: public HLItem{
public:
    HLItemCollection( int state=0, int context=0 ): HLItem( state, context )
    { m_items.setAutoDelete( TRUE ); }

    void appendChild( HLItem* item ) { m_items.append( item ); }

    int checkHL( const QChar* buffer, int pos, int length, int* state, int* next ){
	Q3PtrListIterator<HLItem> it( m_items );

	while( it.current() ){
	    HLItem* item = it.current();

	    int npos = item->checkHL( buffer, pos, length, state, next );

	    if( npos > pos ){
	        pos = npos;
		if( state )
		    *state = item->attr();
		if( next )
		    *next = item->context();
		break;
	    }
	    ++it;
	}

	return pos;
    }

private:
    Q3PtrList<HLItem> m_items;
};

class QSourceColorizer: public Q3TextPreProcessor{
public:
    enum Type {
	Normal=0,
	PreProcessor,
	Keyword,
	BuiltInClass,
	Operator,
	Comment,
	Constant,
	String,

	Custom = 1000
    };

public:
    QSourceColorizer( QEditor* );
    virtual ~QSourceColorizer();

    QEditor* editor() const { return m_editor; }

    void insertHLItem( int, HLItemCollection* );

    void setSymbols( const QString&, const QString& );
    QString leftSymbols() const { return m_left; }
    QString rightSymbols() const { return m_right; }

    virtual Q3TextFormat* format( int key ) { return m_formats[ key ].second; }
    virtual Q3TextFormat* formatFromId( const QString& id );

    QStringList styleList() const;
    virtual void updateStyles( QMap<QString, QPair<QFont, QColor> >& values );
    virtual void process( Q3TextDocument*, Q3TextParagraph*, int, bool=FALSE );
    virtual int computeLevel( Q3TextParagraph*, int ) { return 0; }

protected:
    QEditor* m_editor;
    QMap<int, QPair<QString, Q3TextFormat*> > m_formats;
    Q3PtrList<HLItemCollection> m_items;
    QString m_left;
    QString m_right;
};

}
#endif
