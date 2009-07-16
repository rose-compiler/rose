// (C) 2001-2002 Trolltech AS

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

#ifndef paragdata_h
#define paragdata_h

#include "q3richtext_px.h"
#include <Qt3Support>
#include <QtGui>
#include <QtCore>

namespace qrs {

class Symbol{
public:
    enum Type { Left, Right };
    Symbol() {}

    Symbol( int tp, const QChar& ch, int pos ):
	    m_tp( tp ),
	    m_ch( ch ),
	    m_pos( pos )
	    {}

    Symbol( const Symbol& source )
	    : m_tp( source.m_tp ),
	    m_ch( source.m_ch ),
	    m_pos( source.m_pos )
	    {}

    Symbol& operator = ( const Symbol& source ){
	m_tp = source.m_tp;
	m_ch = source.m_ch;
	m_pos = source.m_pos;
	return *this;
    }

    bool operator == ( const Symbol& p ) const {
	return m_tp == p.m_tp && m_ch == p.m_ch && m_pos == p.m_pos;
    }

    int type() const { return m_tp; }
    QChar ch() const { return m_ch; }
    int pos() const { return m_pos; }

private:
    int m_tp;
    QChar m_ch;
    int m_pos;
};


class ParagData: public Q3TextParagraphData{
public:
    ParagData();
    virtual ~ParagData();

    void clear();
    Q3ValueList<Symbol> symbolList() const { return m_symbolList; }

    void add( int, const QChar&, int );
    void join( Q3TextParagraphData* );

    int level() const;
    void setLevel( int );

    bool isOpen() const;
    void setOpen( bool );

    bool isBlockStart() const;
    void setBlockStart( bool );

    bool isParsed() const;
    void setParsed( bool );

    uint mark() const;
    void setMark( uint );

    int lastLengthForCompletion;

private:
    Q3ValueList<Symbol> m_symbolList;
    int m_level;
    int m_mark;
    bool m_open;
    bool m_blockStart;
    bool m_parsed;
};

}
#endif
