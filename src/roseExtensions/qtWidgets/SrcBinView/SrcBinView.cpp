
#include "rose.h"

#include <iostream>

#include "AsmToSourceMapper.h"

#include "SrcBinView.h"
#include "ui_SrcBinView.h"

using namespace std;

SrcBinView::SrcBinView( QWidget *parent )
    : srcBinView( new Ui::SrcBinView() ),
      binFile( NULL ),
      srcFile( NULL )
{
    srcBinView->setupUi( this );

    /*for( int i( 0 ); i < project->numberOfFiles(); ++i )
    {
        SgFile *file( (*project)[i] );
        SgBinaryFile *binFiletmp( isSgBinaryFile( file ) );
        SgSourceFile *srcFiletmp( isSgSourceFile( file ) );
        
        if( binFiletmp )
            binFile = binFiletmp;
        if( srcFiletmp )
            srcFile = srcFiletmp;
    }

    AsmToSourceMapper map( binFile );
    map.annotate( srcFile );

    srcBinView->roseCodeEdit->setNode( srcFile );
    srcBinView->asmView->setNode( binFile );*/

    connect( srcBinView->asmView         , SIGNAL( clicked( SgNode * ) ),
             this                        , SLOT  ( setBinaryNode( SgNode * ) ) );
    connect( srcBinView->roseCodeEdit    , SIGNAL( cursorPositionChanged() ),
             this                        , SLOT  ( getCursorPosition() ) );

    fillMap( srcFile );

}

SrcBinView::~SrcBinView()
{}

void SrcBinView::setSourceNode( SgNode *node, bool setCodeEdit )
{
    typedef AstAttributeMechanism::iterator iterator;

    AstAttributeMechanism *attributes( node->get_attributeMechanism() );

    if( attributes )
    {
        for( iterator it( attributes->begin() ); it != attributes->end(); ++it )
        {
            AstBinaryNodeLink *nodeLink( dynamic_cast<AstBinaryNodeLink *>( it->second ) );

            if( nodeLink == NULL ) continue;

            if( nodeLink->size() == 1 )
            {
                srcBinView->asmView->gotoNode( nodeLink->at( 0 ).first );
            }
        }
    }

    if( setCodeEdit )
        srcBinView->roseCodeEdit->setNode( node );
}

void SrcBinView::setBinaryNode( SgNode *node )
{
    typedef AstAttributeMechanism::iterator iterator;

    AstAttributeMechanism *attributes( node->get_attributeMechanism() );

    if( attributes )
    {
        for( iterator it( attributes->begin() ); it != attributes->end(); ++it )
        {
            AstSourceNodeLink *nodeLink( dynamic_cast<AstSourceNodeLink *>( it->second ) );

            if( nodeLink == NULL ) continue;

            if( nodeLink->size() == 1 )
            {
                srcBinView->roseCodeEdit->setNode( nodeLink->at( 0 ).first );
            }
            else
            {
                for( vector<pair<SgNode *, SgNode *> >::iterator jt( nodeLink->begin() );
                     jt != nodeLink->end();
                     ++jt )
                {
                    if( isSgBasicBlock( jt->first ) ) continue;

                    srcBinView->roseCodeEdit->setNode( jt->first );
                    break;
                }    
            }
        }
    }

    srcBinView->asmView->gotoNode( node );
}

void SrcBinView::getCursorPosition()
{
    const QDocumentCursor &cursor( srcBinView->roseCodeEdit->cursor() );
    int line( cursor.lineNumber() + 1 );
    int col( cursor.columnNumber() + 1 );

    SourceRange range_start;
    SourceRange range_end;
    //range.line_start = line+1;
    range_start.start.first = line;
    range_start.start.second = 0;
    range_end.start.first = line;
    range_end.start.second = srcBinView->roseCodeEdit->text( line - 1 ).length();

    multimap<SourceRange,SgNode *>::iterator begin( 
            lineColToSgNode.upper_bound( range_start ) );
    multimap<SourceRange,SgNode *>::iterator end( 
            lineColToSgNode.upper_bound( range_end ) );

    if( end != lineColToSgNode.end() ) ++end;
    //if( begin != lineColToSgNode.begin() ) --begin;

    if( begin != lineColToSgNode.end() )
    {
        for( multimap<SourceRange,SgNode *>::iterator it( begin ); it != end; ++it )
        {
            if( line >= it->first.start.first &&
                line <= it->first.end.first &&
                col  >= it->first.start.second &&
                col  <= it->first.end.second )
            {
                setSourceNode( it->second, false );
                return;
            }
        }

        setSourceNode( begin->second, false );
    }
}

void SrcBinView::fillMap( SgNode *node )
{
    if( node == NULL ) return;

    for( size_t idx( 0 ); idx < node->get_numberOfTraversalSuccessors(); ++idx )
    {
        fillMap( node->get_traversalSuccessorByIndex( idx ) );
    }

    if( isSgLocatedNode( node ) )
    {
        SourceRange range;
        
        range.start.first = node->get_startOfConstruct()->get_line();
        range.start.second = node->get_startOfConstruct()->get_col();
        range.end.first = node->get_endOfConstruct()->get_line();
        range.end.second = node->get_endOfConstruct()->get_col();
        
        lineColToSgNode.insert( make_pair( range, node ) );
    }
}
