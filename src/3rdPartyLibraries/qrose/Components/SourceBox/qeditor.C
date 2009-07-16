/*
 *  This file is part of Klint
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

#include "qeditor.h"
//#include "qeditor_settings.h"
#include "qsourcecolorizer.h"
#include "cpp_colorizer.h"
#include "qeditor_indenter.h"
#include "simple_indent.h"
#include "cindent.h"
#include "parenmatcher.h"
#include "paragdata.h"

#include "q3richtext_px.h"
#include <qregexp.h>
#include <qmap.h>

using namespace std;

namespace qrs {

QEditor::QEditor( QWidget* parent, const char* name )
    : Q3TextEdit( parent, name )
{
	getDocument()->setUseFormatCollection( FALSE );

    parenMatcher = new ParenMatcher();

    m_currentLine = -1;

    getDocument()->addSelection( ParenMatcher::Match );
    getDocument()->addSelection( ParenMatcher::Mismatch );
    getDocument()->setSelectionColor( ParenMatcher::Match, QColor( 204, 232, 195 ) );
    getDocument()->setSelectionColor( ParenMatcher::Mismatch, Qt::magenta );
    //document()->setInvertSelectionText( ParenMatcher::Match, FALSE );
    //document()->setInvertSelectionText( ParenMatcher::Mismatch, FALSE );

    getDocument()->addSelection( sel0 );
    getDocument()->setSelectionColor( sel0, QColor( 204, 232, 195 ) );

    getDocument()->addSelection( sel1 );
    getDocument()->setSelectionColor( sel1, QColor( 204, 207, 255 ) );

    setLanguage("c++");

    setTabStop(2);

    setWordWrap(Q3TextEdit::NoWrap);
    setTextFormat(Qt::PlainText);



    //connect( this, SIGNAL(cursorPositionChanged(QTextCursor*) ),
	//     this, SLOT(doMatch(QTextCursor*)) );
//    connect( this, SIGNAL(cursorPositionChanged(int, int) ),
//	     this, SLOT(slotCursorPositionChanged(int, int)) );

}

QEditor::~QEditor()
{
    delete( parenMatcher );
}


void QEditor::doMatch( Q3TextCursor* c )
{
    if( parenMatcher->match(c) ){
	repaintChanged();
    }
}

void QEditor::gotoLine( int line, bool highlight )
{
    setCursorPosition( line, 0 );
    Q3TextParagraph *p = getDocument()->paragAt( line );
    if ( !p )
	return;
    Q3TextCursor c( getDocument() );
    emit ensureTextIsVisible( p );
    c.setParagraph( p );
    c.setIndex( 0 );
    if (highlight) {
    	getDocument()->removeSelection( sel0 );
    	getDocument()->setSelectionStart( sel0, c );
    }
    c.gotoLineEnd();
    if (highlight) {
    	getDocument()->setSelectionEnd( sel0, c );
    }
    viewport()->repaint( FALSE );
}

Q3TextCursor* QEditor::textCursor() const
{
    return (Q3TextCursor *) Q3TextEdit::textCursor();
}

/*
QTextDocument* QEditor::document() const
{
    return document();
} */

void QEditor::drawCursor( bool visible )
{
    Q3TextEdit::drawCursor( visible );
}

void QEditor::configChanged()
{
    updateStyles();
/*
    if( QEditorSettings::self()->wordWrap() ){
	setWordWrap( QEditor::WidgetWidth );
	setHScrollBarMode( QScrollView::AlwaysOff );
	setVScrollBarMode( QScrollView::AlwaysOn );
    } else {
	setWordWrap( QEditor::NoWrap );
	setHScrollBarMode( QScrollView::AlwaysOn );
	setVScrollBarMode( QScrollView::AlwaysOn );
    }
*/
    refresh();
}


void QEditor::updateStyles()
{
    //kdDebug(9032) << "QEditor::updateStyles()" << endl;
    int tabwidth = tabStop();
    QSourceColorizer* colorizer = dynamic_cast<QSourceColorizer*>( getDocument()->preProcessor() );
    if( colorizer ){
	setFont( colorizer->format( 0 )->font() );
	setTabStopWidth( colorizer->format(0)->width('x') * tabwidth );
	getDocument()->setTabStops( colorizer->format(0)->width('x') * tabwidth );
    }
    Q3TextEdit::updateStyles();
}

/*
void QEditor::setDocument( QTextDocument* doc )
{
    Q3TextEdit::setDocument( doc );
}
*/
void QEditor::repaintChanged()
{
    Q3TextEdit::repaintChanged();
}

QString QEditor::textLine( uint line ) const
{
    return text( line );
}

void QEditor::setLanguage( const QString& l )
{
//    kdDebug(9032) << "QEditor::setLanguage(" << l << ")" << endl;
    m_language = l;
    if( m_language == "c++" ){
        setElectricKeys( "{}" );
        getDocument()->setPreProcessor( new CppColorizer(this) );
        getDocument()->setIndent( new CIndent(this) );
    } else {
        setElectricKeys( QString::null );
        getDocument()->setPreProcessor( 0 );
        getDocument()->setIndent( new SimpleIndent(this) );
    }

    configChanged();
    sync();
}

QString QEditor::language() const
{
    return m_language;
}


void QEditor::setText( const QString& text )
{
//    setTextFormat( Q3TextEdit::PlainText );

    Q3TextEdit::setText(text);
    // hack to maintain font point size
    zoomTo(pointSize()-1);
    zoomTo(pointSize()+1);

    refresh();
}

void QEditor::slotCursorPositionChanged( int line, int column )
{
    Q_UNUSED( line );
    Q_UNUSED( column );
}

int QEditor::level( int line) const
{
    ParagData* data = (ParagData*) getDocument()->paragAt( line )->extraData();
    if( data ){
	return data->level();
    }
    return 0;
}


void QEditor::setLevel( int line, int lev )
{
    ParagData* data = (ParagData*) getDocument()->paragAt( line )->extraData();
    if( data ){
        return data->setLevel( lev );
    }
}

QSourceColorizer* QEditor::colorizer() const
{
    return dynamic_cast<QSourceColorizer*>( getDocument()->preProcessor() );
}

void QEditor::refresh()
{
	getDocument()->invalidate();
    Q3TextParagraph* p = getDocument()->firstParagraph();
    while( p ){
        p->format();
	p = p->next();
    }
    getDocument()->removeSelection( ParenMatcher::Match );
    getDocument()->removeSelection( ParenMatcher::Mismatch );
    ensureCursorVisible();
    repaintContents( false );
}


QEditorIndenter* QEditor::indenter() const
{
  return dynamic_cast<QEditorIndenter*>( getDocument()->indent() );
}

void QEditor::indent()
{
    Q3TextEdit::indent();
    if( !hasSelectedText() && text( textCursor()->paragraph()->paragId() ).stripWhiteSpace().isEmpty() )
	moveCursor( MoveLineEnd, false );
}

void QEditor::contentsMouseDoubleClickEvent( QMouseEvent * e )
{

    if ( e->button() != Qt::LeftButton ) {
        e->ignore();
        return;
    }

    int para = 0;
    int index = charAt( e->pos(), &para );

    emit doubleClicked(para, index);

}

bool QEditor::isDelimiter(const QChar& c)
{
    if (c == '_') return false;
    return !(c.isLetterOrNumber());
}

void QEditor::selectCode(int line0, int col0, int line1, int col1, TypeSel sel_type) {
	getDocument()->removeSelection(sel_type);
    setCursorPosition(line0, col0);
    Q3TextCursor c1 = *textCursor();

    setCursorPosition(line1, col1);
    Q3TextCursor c2 = *textCursor();

    getDocument()->setSelectionStart( sel_type, c1 );
    getDocument()->setSelectionEnd( sel_type, c2 );

    repaintChanged();
}

void QEditor::removeSelection(TypeSel sel_type) {
	getDocument()->removeSelection((int) sel_type);
   repaintChanged();
}

void QEditor::contentsMousePressEvent ( QMouseEvent * e ) {
    if (e->button() != Qt::RightButton) {
	e->ignore();
	return;
    }
    int line = 0;
    int col = charAt(e->pos(), &line);
    emit rightClicked(e->globalPos(), line, col);
}

int QEditor::tabStop() const {
    return m_tabStop;
}
void QEditor::setTabStop( int tab) {
    m_tabStop = tab;
}

Q3PopupMenu *QEditor::createPopupMenu(const QPoint &pos) {
    return NULL;
}

}
