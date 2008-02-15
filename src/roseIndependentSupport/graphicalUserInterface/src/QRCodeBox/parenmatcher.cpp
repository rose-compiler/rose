
#include "parenmatcher.h"
#include "paragdata.h"

#include <private/qrichtext_p.h>
#include <qtextedit.h>
#include <qapplication.h>

ParenMatcher::ParenMatcher()
{
    enabled = TRUE;
}

bool ParenMatcher::match( QTextCursor *cursor )
{
    if ( !enabled )
	return FALSE;
    bool ret = FALSE;

    QChar c( cursor->paragraph()->at( cursor->index() )->c );
    bool ok1 = FALSE;
    bool ok2 = FALSE;
    if ( c == '{' || c == '(' || c == '[' ) {
	ok1 = checkOpenParen( cursor );
	ret = ok1 || ret;
    } else if ( cursor->index() > 0 ) {
	c = cursor->paragraph()->at( cursor->index() - 1 )->c;
	if ( c == '}' || c == ')' || c == ']' ) {
	    ok2 = checkClosedParen( cursor );
	    ret = ok2 || ret;
	}
    }
    return ret;
}

bool ParenMatcher::checkOpenParen( QTextCursor *cursor )
{
    if ( !cursor->paragraph()->extraData() )
	return FALSE;
    QValueList<Symbol> parenList = ( (ParagData*)cursor->paragraph()->extraData() )->symbolList();

    Symbol openParen, closedParen;
    QTextParagraph *closedParenParag = cursor->paragraph();

    int i = 0;
    int ignore = 0;
    bool foundOpen = FALSE;
    QChar c = cursor->paragraph()->at( cursor->index() )->c;
    for (;;) {
	if ( !foundOpen ) {
	    if ( i >= (int)parenList.count() )
		goto bye;
	    openParen = parenList[ i ];
	    if ( openParen.pos() != cursor->index() ) {
		++i;
		continue;
	    } else {
		foundOpen = TRUE;
		++i;
	    }
	}

	if ( i >= (int)parenList.count() ) {
	    for (;;) {
		closedParenParag = closedParenParag->next();
		if ( !closedParenParag )
		    goto bye;
		if ( closedParenParag->extraData() &&
		     ( (ParagData*)closedParenParag->extraData() )->symbolList().count() > 0 ) {
		    parenList = ( (ParagData*)closedParenParag->extraData() )->symbolList();
		    break;
		}
	    }
	    i = 0;
	}

	closedParen = parenList[ i ];
	if ( closedParen.type() == Symbol::Left ) {
	    ignore++;
	    ++i;
	    continue;
	} else {
	    if ( ignore > 0 ) {
		ignore--;
		++i;
		continue;
	    }

	    int id = Match;
	    if ( c == '{' && closedParen.ch() != '}' ||
		 c == '(' && closedParen.ch() != ')' ||
		 c == '[' && closedParen.ch() != ']' )
		id = Mismatch;
	    cursor->document()->setSelectionStart( id, *cursor );
	    int tidx = cursor->index();
	    QTextParagraph *tstring = cursor->paragraph();
	    cursor->setParagraph( closedParenParag );
	    cursor->setIndex( closedParen.pos() + 1 );
	    cursor->document()->setSelectionEnd( id, *cursor );
	    cursor->setParagraph( tstring );
	    cursor->setIndex( tidx );
	    return TRUE;
	}
    }

 bye:
    return FALSE;
}

bool ParenMatcher::checkClosedParen( QTextCursor *cursor )
{
    if ( !cursor->paragraph()->extraData() )
	return FALSE;
    QValueList<Symbol> parenList = ( (ParagData*)cursor->paragraph()->extraData() )->symbolList();

    Symbol openParen, closedParen;
    QTextParagraph *openParenParag = cursor->paragraph();

    int i = parenList.count() - 1;
    int ignore = 0;
    bool foundClosed = FALSE;
    QChar c = cursor->paragraph()->at( cursor->index() - 1 )->c;
    for (;;) {
	if ( !foundClosed ) {
	    if ( i < 0 )
		goto bye;
	    closedParen = parenList[ i ];
	    if ( closedParen.pos() != cursor->index() - 1 ) {
		--i;
		continue;
	    } else {
		foundClosed = TRUE;
		--i;
	    }
	}

	if ( i < 0 ) {
	    for (;;) {
		openParenParag = openParenParag->prev();
		if ( !openParenParag )
		    goto bye;
		if ( openParenParag->extraData() &&
		     ( (ParagData*)openParenParag->extraData() )->symbolList().count() > 0 ) {
		    parenList = ( (ParagData*)openParenParag->extraData() )->symbolList();
		    break;
		}
	    }
	    i = parenList.count() - 1;
	}

	openParen = parenList[ i ];
	if ( openParen.type() == Symbol::Right ) {
	    ignore++;
	    --i;
	    continue;
	} else {
	    if ( ignore > 0 ) {
		ignore--;
		--i;
		continue;
	    }

	    int id = Match;
	    if ( c == '}' && openParen.ch() != '{' ||
		 c == ')' && openParen.ch() != '(' ||
		 c == ']' && openParen.ch() != '[' )
		id = Mismatch;
	    cursor->document()->setSelectionStart( id, *cursor );
	    int tidx = cursor->index();
	    QTextParagraph *tstring = cursor->paragraph();
	    cursor->setParagraph( openParenParag );
	    cursor->setIndex( openParen.pos() );
	    cursor->document()->setSelectionEnd( id, *cursor );
	    cursor->setParagraph( tstring );
	    cursor->setIndex( tidx );
	    return TRUE;
	}
    }

 bye:
    return FALSE;
}
