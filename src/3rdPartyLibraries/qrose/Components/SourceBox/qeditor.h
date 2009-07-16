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

#ifndef qeditor_h
#define qeditor_h

#include <Qt3Support>
#include <QtGui>
#include <QtCore>

namespace qrs {

class ParenMatcher;
class QPopupMenu;
class QSourceColorizer;
class QEditorIndenter;
class QEditorKey;
class Q3TextDocument;
class Q3TextCursor;
class Q3TextParagraph;

class QEditor: public Q3TextEdit
{
    Q_OBJECT

public:
    typedef enum TypeSel { sel0 = 1000, sel1 = 2000 };
    QEditor( QWidget* parent=0, const char* name=0 );
    virtual ~QEditor();

    QEditorIndenter* indenter() const;
    Q3TextCursor* textCursor() const;
 //   void setDocument( QTextDocument*);

    Q3TextDocument* getDocument() const { return (Q3TextDocument *) document(); }

    void drawCursor( bool visible );
    void updateStyles();
    void repaintChanged();

    virtual QString electricKeys() const { return m_electricKeys; }
    virtual void setElectricKeys( const QString& keys ) { m_electricKeys = keys; }

    virtual int tabStop() const;
    virtual void setTabStop( int );

    virtual QString textLine( uint ) const;

    virtual void setLanguage( const QString& );
    virtual QString language() const;

    virtual void setText( const QString& );

    virtual int level( int ) const;
    virtual void setLevel( int, int );

    virtual void indent();

    QSourceColorizer* colorizer() const;

    virtual void gotoLine( int line, bool highlight = false);
    virtual void selectCode(int line0, int col0, int line1, int col1, TypeSel type_sel);
    void removeSelection(TypeSel type_sel);

signals:
    void parsed();
    void ensureTextIsVisible( Q3TextParagraph* );
    void rightClicked(const QPoint &, int, int);

public slots:
    virtual void configChanged();
    virtual void refresh();

private slots:
    void doMatch( Q3TextCursor* );
    void slotCursorPositionChanged( int, int );

protected:
    void contentsMouseDoubleClickEvent( QMouseEvent * e );
    void contentsMousePressEvent ( QMouseEvent * e );
    bool isDelimiter(const QChar& c);
    Q3PopupMenu *createPopupMenu(const QPoint &pos);

private:
    ParenMatcher* parenMatcher;
    QString m_language;
    int m_currentLine;
    int m_tabStop;
    QString m_electricKeys;
};

}
#endif
