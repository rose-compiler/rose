
#include "rose.h"

#include <iostream>
#include <sstream>

#include "AsmToSourceMapper.h"

using namespace std;

struct compare_address
{
    compare_address( uint64_t address_ )
        : address( address_ )
    {}

    bool operator() ( SgNode *node )
    {
        SgAsmDwarfLine *dwarfLine( isSgAsmDwarfLine( node ) );
        assert( dwarfLine );

        return dwarfLine->get_address() == address;
    }

    uint64_t address;
};

AsmFunctions::AsmFunctions( SgBinaryFile *node )
{
    addFile( node );
}

void AsmFunctions::addFile( SgBinaryFile *node )
{
    SgNodeContainer functions = NodeQuery::querySubTree( node, V_SgAsmFunctionDeclaration );
    dwarfLines = NodeQuery::querySubTree( node, V_SgAsmDwarfLine );

    const_iterator dwarfIter = dwarfLines.begin();

    Sg_File_Info *binaryFileInfo( node->get_file_info() );
      
    binaryFileIdToName = binaryFileInfo->get_fileidtoname_map();

    cout << "Number of functions in binary file: " << functions.size() << endl;

    for( const_iterator funcIter = functions.begin();
            funcIter != functions.end(); ++funcIter )
    {
        SgNode *fun( *funcIter );
        assert( isSgAsmFunctionDeclaration( fun ) );
        
        //cout << "Processing function: " << hex << fun->get_address() << dec << endl;

        //matchDwarfLine( fun );

        for( size_t i( 0 ); i < fun->get_numberOfTraversalSuccessors(); ++i )
        {
            SgNode *block( fun->get_traversalSuccessorByIndex( i ) );
            assert( isSgAsmBlock( block ) );

            //cout << "   Processing Block: " << hex << block->get_address() << dec << endl;

            //matchDwarfLine( block );
            for( size_t j( 0 ); j < block->get_numberOfTraversalSuccessors(); ++j )
            {
                SgNode *instr( block->get_traversalSuccessorByIndex( j ) );
                //cout << instr->class_name() << endl;
                assert( isSgAsmInstruction( instr ) );

                matchDwarfLine( isSgAsmStatement( instr ) );
            }
        }

    }
    cout << "Number of elements in set: " << addressToFileInfo.size() << endl;
}

void AsmFunctions::annotate( SgNode *node ) const
{
    if( !node ) return;

    if( isSgLocatedNode( node ) )
    {
        Sg_File_Info *fileNode( node->get_file_info() );
        assert( fileNode );

        FileInfo tmp( fileNode->get_filenameString(),
                      fileNode->get_line(),
                      fileNode->get_col() );

        std::set<FileInfo>::iterator pos( addressToFileInfo.find( tmp ) );

        if( pos != addressToFileInfo.end() )
        {
            SgNode *binaryNode( pos->node );
            SgAsmInstruction *instr( isSgAsmInstruction( binaryNode ) );
            assert( instr );

            stringstream ss;
            ss << pos->fileId << ":" << instr->get_address();

            node->addNewAttribute( ss.str(),
                                   new AstBinaryNodeLink( binaryNode ) );
            ss.clear();
            ss << fileNode->get_filenameString() << ":"
               << fileNode->get_line() << ","
               << fileNode->get_col() << " " << node;

            //cout << "Annotating " << ss.str() << endl;

            binaryNode->addNewAttribute( ss.str(),
                                         new AstSourceNodeLink( node ) );
        }
    }

    for( size_t i( 0 ); i < node->get_numberOfTraversalSuccessors(); ++i )
    {
        annotate( node->get_traversalSuccessorByIndex( i ) );
    }
}

void AsmFunctions::matchDwarfLine( SgAsmStatement *node )
{

    SgNodeContainer::iterator lineSearch( dwarfLines.begin() );
    while( ( lineSearch = find_if( lineSearch, dwarfLines.end(), 
                    compare_address( node->get_address() ) ) )
            != dwarfLines.end() )
    {
        SgAsmDwarfLine *dwarfLine( isSgAsmDwarfLine( *lineSearch ) );
        assert( dwarfLine );
        //cout << "found match ... " << hex << dwarfLine->get_address() << dec << endl;
    
        FileInfo newInfo( binaryFileIdToName[ dwarfLine->get_file_id() ],
                          dwarfLine->get_line(),
                          dwarfLine->get_column(),
                          node );
        
        addressToFileInfo.insert( newInfo );

        ++lineSearch;
    }
}

