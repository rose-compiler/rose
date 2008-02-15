
#include "cpp_colorizer.h"
//#include "qeditor_part.h"
#include "paragdata.h"

#include <qfont.h>
#include <private/qrichtext_p.h>

#include <qdom.h>
#include <qfile.h>

/*
#include <kapplication.h>
#include <kdebug.h>
#include <kconfig.h>
#include <kstandarddirs.h>
*/

static const char *cpp_keywords[] = {
    "break", "case", "continue", "default", "do", "else", "enum", "extern",
    "for", "goto", "if", "return", "sizeof", "struct",
    "switch", "typedef", "union", "while",
    "auto", "char", "uchar", "const", "double", "float", "int", "uint", "long", "register",
    "short", "signed", "static", "unsigned", "void", "volatile",
    "asm", "catch", "class", "const_cast", "const", "delete", "dynamic_cast",
    "explicit", "export", "false", "friend", "inline", "namespace", "new",
    "operator", "private", "protected", "public", "reinterpret_cast",
    "static_cast", "template", "this", "throw", "true", "try", "typeid",
    "typename", "using", "virtual",
    "and_eq", "and", "bitand", "bitor", "compl", "not_eq", "not", "or_eq", "or",
    "xor_eq", "xor",
    "slots", "signals",
    "bool", "wchar_t", "mutable",
    0
};

class CppPreprocHLItem: public HLItem{
public:
    CppPreprocHLItem( int state, int context )
        : HLItem( state, context ) {}

    int checkHL( const QChar* buffer, int pos, int length, int*, int* ){
	//kdDebug(9032) << "CppPreprocHLItem::checkHLItem" << endl;
	int start_pos = pos;
	
	while( (pos<length) && buffer[pos].isSpace() )
	    ++pos;
	
	if( (pos < length) && buffer[pos] == '#' )
	    return pos + 1;

	return start_pos;
    }
};

class CppPreprocLineHLItem: public HLItem{
public:
    CppPreprocLineHLItem( int state, int context )
        : HLItem( state, context ) {}

    int checkHL( const QChar* buffer, int pos, int length, int*, int* ){
	//kdDebug(9032) << "CppPreprocLineHLItem::checkHLItem" << endl;
	int end_pos = length - 1;
	
	while( (end_pos>=0) && buffer[end_pos].isSpace() )
	    --end_pos;
	
	if( (end_pos >= 0) && buffer[end_pos] == '\\' )
	    return length;
	
	return pos;
    }
};

using namespace std;

CppColorizer::CppColorizer( QEditor* editor )
    : QSourceColorizer( editor )
{
    loadDynamicKeywords();
	
    // default context
    HLItemCollection* context0 = new HLItemCollection( 0 );
    context0->appendChild( new CppPreprocHLItem( PreProcessor, 4 ) );
    context0->appendChild( new WhiteSpacesHLItem( Normal, 0 ) );
    context0->appendChild( new StringHLItem( "'", String, 1 ) );
    context0->appendChild( new StringHLItem( "\"", String, 2 ) );
    context0->appendChild( new StringHLItem( "/*", Comment, 3 ) );
    context0->appendChild( new StartsWithHLItem( "//", Comment, 0 ) );	
    context0->appendChild( new HexHLItem( Constant, 0 ) );
    context0->appendChild( new NumberHLItem( Constant, 0 ) );
    context0->appendChild( new KeywordsHLItem( m_dynamicKeywords, BuiltInClass, Normal, 0, false ) );
    context0->appendChild( new KeywordsHLItem( cpp_keywords, Keyword, Normal, 0 ) );

    HLItemCollection* context1 = new HLItemCollection( String );
    context1->appendChild( new StringHLItem( "\\\\", String, 1 ) );
    context1->appendChild( new StringHLItem( "\\'", String, 1 ) );
    context1->appendChild( new StringHLItem( "'", String, 0 ) );

    HLItemCollection* context2 = new HLItemCollection( String );
    context2->appendChild( new StringHLItem( "\\\\", String, 2 ) );
    context2->appendChild( new StringHLItem( "\\\"", String, 2 ) );
    context2->appendChild( new StringHLItem( "\"", String, 0 ) );

    HLItemCollection* context3 = new HLItemCollection( Comment );
    context3->appendChild( new StringHLItem( "*/", Comment, 0 ) );

    HLItemCollection* context4 = new HLItemCollection( PreProcessor );
    context4->appendChild( new CppPreprocLineHLItem( PreProcessor, 4 ) );
    context4->appendChild( new StartsWithHLItem( "", PreProcessor, 0 ) );


    m_items.append( context0 );
    m_items.append( context1 );
    m_items.append( context2 );
    m_items.append( context3 );
    m_items.append( context4 );
}

CppColorizer::~CppColorizer()
{
}

void CppColorizer::loadDynamicKeywords()
{
    return;
    
    QString strFileNameTag( "name" );
    QString strClassNameTag( "name" );
    
    m_dynamicKeywords.clear();
    
    QString hlFileDir;// = KGlobal::dirs()->findResourceDir( "data", "highlighting.xml" );
    
    hlFileDir += "qeditorpart/highlight/";
    
    //kdDebug(9032) << "Highlighting Dir: " << hlFileDir << endl;
    
    if( hlFileDir.isNull() )
	return;
    
    QDomDocument hlFile( "hlfile" ), curDoc ( "classlist" );
    QFile hlRawFile( hlFileDir + "highlighting.xml" );
    int keywordIndex = 0;
    if( !hlRawFile.open( IO_ReadOnly ) )
	return;
    if( !hlFile.setContent( &hlRawFile ) ) {
	hlRawFile.close();
	return;
    }
    hlRawFile.close();
    
    QDomElement e = hlFile.documentElement();
    QDomNode n = e.firstChild();
    while( !n.isNull() ) {
	e = n.toElement();
	if( !e.isNull() ) {
	    
	    // kdDebug(9032) << "Loading classes-file: " << (hlFileDir + e.attribute( strFileNameTag )) << endl;
	    
	    QFile clsRawFile( hlFileDir + e.attribute( strFileNameTag ) );
	    if( clsRawFile.open( IO_ReadOnly ) && curDoc.setContent( &clsRawFile ) ) {
	    
		QDomElement e = curDoc.documentElement();
		QDomNode n = e.firstChild();
		while( !n.isNull() ) {
		    e = n.toElement();
		    if( !e.isNull()) {
			// kdDebug(9032) << "Adding dynamic keyword: '" << e.attribute( strClassNameTag ) << "'" << endl;
			m_dynamicKeywords.insert( e.attribute( strClassNameTag ), keywordIndex++ );
		    }
		    n = n.nextSibling();
		}
		
	    }
	    clsRawFile.close();
	}
	n = n.nextSibling();
    }
}

int CppColorizer::computeLevel( QTextParagraph* parag, int startLevel )
{
    int level = startLevel;

    ParagData* data = (ParagData*) parag->extraData();
    if( !data ){
        return startLevel;
    }

    data->setBlockStart( false );

    QValueList<Symbol> symbols = data->symbolList();
    QValueList<Symbol>::Iterator it = symbols.begin();
    while( it != symbols.end() ){
        Symbol sym = *it++;
        if( sym.ch() == '{' ){
            ++level;
        } else if( sym.ch() == '}' ){
            --level;
        }
    }

    if( level > startLevel ){
        data->setBlockStart( true );
    }

    return level;
}

