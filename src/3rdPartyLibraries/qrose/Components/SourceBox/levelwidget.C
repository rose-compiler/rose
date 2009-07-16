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

#include "levelwidget.h"
#include "qeditor.h"
#include "paragdata.h"

#include "q3richtext_px.h"
#include <Qt3Support>
#include <QtGui>
#include <QtCore>

using namespace std;

namespace qrs {

const char * plus_xpm[] = {
"12 12 3 1",
"       c None",
".      c #000000",
"+      c #FFFFFF",
" .........  ",
" .+++++++.  ",
" .+++++++.  ",
" .+++++++.  ",
" .+++.+++.  ",
" .+++.+++.  ",
" .+.....+.  ",
" .+++.+++.  ",
" .+++.+++.  ",
" .+++++++.  ",
" .+++++++.  ",
" .........  " };

const  char * minus_xpm[] = {
"12 12 3 1",
"       c None",
".      c #000000",
"+      c #FFFFFF",
" .........  ",
" .+++++++.  ",
" .+++++++.  ",
" .+++++++.  ",
" .+++++++.  ",
" .+++++++.  ",
" .+.....+.  ",
" .+++++++.  ",
" .+++++++.  ",
" .+++++++.  ",
" .+++++++.  ",
" .........  "};

static QPixmap *plusPixmap = 0;
static QPixmap *minusPixmap = 0;


LevelWidget::LevelWidget( QEditor* editor, QWidget* parent, const char* name )
	: QWidget( parent, name, Qt::WNoAutoErase ),
	  m_editor( editor )
{
    if( !plusPixmap ){
        plusPixmap = new QPixmap( plus_xpm );
        minusPixmap = new QPixmap( minus_xpm );
    }

    setFixedWidth( 16 );

    connect( m_editor->verticalScrollBar(), SIGNAL( valueChanged( int ) ),
	     this, SLOT( doRepaint() ) );
    connect( m_editor, SIGNAL( textChanged() ),
	     this, SLOT( doRepaint() ) );
    connect( m_editor, SIGNAL( parsed() ),
	     this, SLOT( doRepaint() ) );
    doRepaint();
}

LevelWidget::~LevelWidget()
{
}

void LevelWidget::paintEvent( QPaintEvent* /*e*/ )
{
    buffer.fill( /*backgroundColor()*/ );

    Q3TextParagraph *p = m_editor->getDocument()->firstParagraph();
    QPainter painter( &buffer );
    int yOffset = m_editor->contentsY();
    while ( p ) {
	if ( !p->isVisible() ) {
	    p = p->next();
	    continue;
	}
	if ( p->rect().y() + p->rect().height() - yOffset < 0 ) {
	    p = p->next();
	    continue;
	}
	if ( p->rect().y() - yOffset > height() )
	    break;

	ParagData* data = (ParagData*) p->extraData();
	if( data ){

	    int prevLevel = 0;
	    if( p->prev() ){
		prevLevel = ((ParagData*) p->prev()->extraData())->level();
	    }

	    if( data->isBlockStart() ){
		if( data->isOpen() ){
                    painter.drawLine( minusPixmap->width() / 2 + 2,
                                      p->rect().y() + p->rect().height() - yOffset,
                                      minusPixmap->width() / 2 - 2,
                                      p->rect().y() + p->rect().height() - yOffset );

		    painter.drawPixmap( 0, p->rect().y() +
					( p->rect().height() - minusPixmap->height() ) / 2 -
					yOffset, *minusPixmap );
		} else {
		    painter.drawPixmap( 0, p->rect().y() +
					( p->rect().height() - plusPixmap->height() ) / 2 -
					yOffset, *plusPixmap );
		}
	    } else if( data->level() < prevLevel ){
		painter.drawLine( plusPixmap->width() / 2, p->rect().y() - yOffset,
				  plusPixmap->width() / 2, p->rect().y() + p->rect().height() - yOffset );

		painter.drawLine( plusPixmap->width() / 2 + 2,
				  p->rect().y() + p->rect().height() - yOffset,
				  plusPixmap->width() / 2 - 2,
				  p->rect().y() + p->rect().height() - yOffset );
	    } else if( data->level() != 0 ){
		painter.drawLine( plusPixmap->width() / 2, p->rect().y() - yOffset,
				  plusPixmap->width() / 2, p->rect().y() + p->rect().height() - yOffset );
	    }
	}
	p = p->next();
    }

    painter.end();
    bitBlt( this, 0, 0, &buffer );
}

void LevelWidget::resizeEvent( QResizeEvent *e )
{
    buffer.resize( e->size() );
    QWidget::resizeEvent( e );
}

void LevelWidget::mousePressEvent( QMouseEvent* e )
{
    Q3TextParagraph *p = m_editor->getDocument()->firstParagraph();
    int yOffset = m_editor->contentsY();
    while ( p ) {
	if ( e->y() >= p->rect().y() - yOffset && e->y() <= p->rect().y() + p->rect().height() - yOffset ) {
	    ParagData *data = (ParagData*) p->extraData();
            if( data && data->isBlockStart() ){

                if( data->isOpen() ){
                    emit collapseBlock( p );
                } else {
                    emit expandBlock( p );
                }
	    }
	    break;
	}
	p = p->next();
    }

    doRepaint();
}
}
