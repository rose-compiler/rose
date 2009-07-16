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

#include "linenumberwidget.h"
#include "qeditor.h"
#include "paragdata.h"

#include "q3richtext_px.h"

namespace qrs {

LineNumberWidget::LineNumberWidget( QEditor* editor, QWidget* parent, const char* name )
 : QWidget( parent, name, Qt::WNoAutoErase  ),
   m_editor( editor )
{
 setFixedWidth( 50 );

 connect( m_editor->verticalScrollBar(), SIGNAL( valueChanged( int ) ),
    this, SLOT( doRepaint() ) );
 connect( m_editor, SIGNAL( textChanged() ),
    this, SLOT( doRepaint() ) );
        doRepaint();
}

LineNumberWidget::~LineNumberWidget()
{
}

void LineNumberWidget::paintEvent( QPaintEvent* /*e*/ )
{
 buffer.fill();

 Q3TextParagraph *p = m_editor->getDocument()->firstParagraph();
 QPainter painter( &buffer );
        QFont f(painter.font());
 f.setPointSize(m_editor->pointSize());
        painter.setFont(f);
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
  //ParagData *paragData = (ParagData*)p->extraData();

  painter.drawText( 0, p->rect().y() - yOffset+3,
      buffer.width() - 10, p->rect().height(),
      Qt::AlignRight | Qt::AlignBottom,
      QString::number(p->paragId()+1) );
  p = p->next();
 }

 painter.end();
 bitBlt( this, 0, 0, &buffer );
}

void LineNumberWidget::resizeEvent( QResizeEvent *e )
{
 buffer.resize( e->size() );
 QWidget::resizeEvent( e );
}

}
