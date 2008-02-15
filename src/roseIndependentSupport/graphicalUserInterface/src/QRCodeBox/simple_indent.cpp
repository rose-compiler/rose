#include "simple_indent.h"
#include "qsourcecolorizer.h"
#include "paragdata.h"
#include "qeditor.h"

SimpleIndent::SimpleIndent( QEditor* ed )
    : QEditorIndenter( ed )
{

}

SimpleIndent::~SimpleIndent()
{

}

int SimpleIndent::indentForLine( int line )
{
    int ind = 0;

    --line;
    while( line>=0 ){
        QString raw_text = editor()->text( line );
        if( !raw_text.stripWhiteSpace().isEmpty() ){
            ind = indentation( raw_text );
            break;
        }
	--line;
    }
    return ind;
}
