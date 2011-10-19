
#include "sage3basic.h"

#include <iostream>
#include <sstream>

#include "AsmToSourceMapper.h"

using namespace std;

#if USE_ROSE_DWARF_SUPPORT
struct compare_address
{
    compare_address()
    {}

    compare_address( uint64_t address_ )
        : address( address_ )
    {}

    bool operator() ( SgNode *node )
    {
        SgAsmDwarfLine *dwarfLine( isSgAsmDwarfLine( node ) );
        assert( dwarfLine );

        return dwarfLine->get_address() == address;
    }

    bool operator() ( SgNode *n1, SgNode *n2 )
    {
        SgAsmDwarfLine *dwarfLine1( isSgAsmDwarfLine( n1 ) );
        SgAsmDwarfLine *dwarfLine2( isSgAsmDwarfLine( n2 ) );

        return dwarfLine1->get_address() < dwarfLine2->get_address();
    }

    uint64_t address;
};
#endif


AsmToSourceMapper::AsmToSourceMapper( SgBinaryComposite *file_ )
    : file( file_ )
{
    addFile();
}

void AsmToSourceMapper::addFile()
{
#if USE_ROSE_DWARF_SUPPORT
    typedef std::vector<SgNode *>::iterator nodeIterator;

    // obtaining a vector with all the dwarf Information
    std::vector<SgNode *> dwarfLines(
            NodeQuery::querySubTree( file, V_SgAsmDwarfLine ) );

    // obtaining a vector with all the Assembler Instructions
    // note: assuming the vector is sorted w.r.t. the addresses
    std::vector<SgNode *> asmInstructions(
            NodeQuery::querySubTree( file, V_SgAsmInstruction ) );

    // return now if one of those vectors is empty
    if( dwarfLines.size() == 0 ||
        asmInstructions.size() == 0 )
        return;

    
    // sorting the dwarfLine information by address (ascending)
    sort( dwarfLines.begin(), dwarfLines.end(), compare_address() );

    const nodeIterator& dwarfLinesBegin( dwarfLines.begin() );
    SgAsmDwarfLine *firstDwarfLine( isSgAsmDwarfLine( *dwarfLinesBegin ) );
    assert( firstDwarfLine );

    const nodeIterator& dwarfLinesEnd( dwarfLines.end() );
    SgAsmDwarfLine *lastDwarfLine( isSgAsmDwarfLine( *(dwarfLinesEnd-1) ) );
    assert( lastDwarfLine );

    const nodeIterator& asmInstructionsBegin( asmInstructions.begin() );
    const nodeIterator& asmInstructionsEnd( asmInstructions.end() );

    nodeIterator currentInstrItr( asmInstructionsBegin );

    cout << "dwarfLines: " << dwarfLines.size() << endl;

    cout << hex;
    for( nodeIterator currentDwarfLineItr( dwarfLinesBegin );
         currentDwarfLineItr != dwarfLinesEnd - 1;
         ++currentDwarfLineItr )
    {
        nodeIterator nextDwarfLineItr( currentDwarfLineItr+1 );

        SgAsmDwarfLine *currentDwarfLine( isSgAsmDwarfLine( *currentDwarfLineItr ) );
        assert( currentDwarfLine );
        
        SgAsmDwarfLine *nextDwarfLine( isSgAsmDwarfLine( *nextDwarfLineItr ) );
        assert( nextDwarfLine );

        // searching for the start address node
        SgAsmInstruction *start( NULL );
        do
        {
            SgAsmInstruction *currentInstr( isSgAsmInstruction( *currentInstrItr ) );
            assert( currentInstr );

            start = currentInstr;

            ++currentInstrItr;
        } while( start->get_address() < currentDwarfLine->get_address() );

        --currentInstrItr;

        // searching for the end address node
        // if the adjacent dwarfLine Infos point to the same address,
        // start = end
        SgAsmInstruction *end( NULL );
        if( currentDwarfLine->get_address() == nextDwarfLine->get_address() )
        {
            end = start;
        }
        else
        {
            // otherwise search again through the vector until node is reached
            SgAsmInstruction *currentInstr( NULL );
            do
            {
                currentInstr = isSgAsmInstruction( *currentInstrItr );
                assert( currentInstr );

                ++currentInstrItr;
            } while( currentInstr->get_address() < nextDwarfLine->get_address() );

            --currentInstrItr;

            end = isSgAsmInstruction( *( currentInstrItr - 1 ) );
        }

        // insert into multiset
        // block
        assert( start->get_parent() );
        SgAsmFunction *fun( 
                isSgAsmFunction( start->get_parent()->get_parent() ) );
        assert( fun );

        // continue if we accidently got a block over function boundaries
        assert( end->get_parent() );
        if( fun != end->get_parent()->get_parent() )
            continue;

        asmMappingInfos.insert( AsmMappingInfo( fun, start, end,
                    Sg_File_Info::getFilenameFromID( currentDwarfLine->get_file_id() ),
                    currentDwarfLine->get_line() ) );
        
        //cout << "current Dwarf Line: " << currentDwarfLine->get_address()
             //<< " file id: " << dec << currentDwarfLine->get_file_id() << hex << endl
             //<< ", next Dwarf Line: " << nextDwarfLine->get_address() << endl
             //<< "block start: " << start->get_address()
             //<< ", block end: " << end->get_address() << endl
             //<< "File info: " << endl
             //<< Sg_File_Info::getFilenameFromID( currentDwarfLine->get_file_id() ) << " "
             //<< "line: " << dec << currentDwarfLine->get_line() << hex
             //<< endl << endl;
    }
    cout << dec;
#endif
}

void AsmToSourceMapper::annotate( SgSourceFile *srcFile )
{
    const string srcPrefix( "BinaryLink/" );
    const string binPrefix( "SourceLink/" );
    stringstream id( "" );

    id << srcPrefix << file->getFileName();
    const string srcId( id.str() );

    addAttribute<AstBinaryNodeLink>( srcFile, id.str(), file, 0 );

    id.str( "" );
    id << binPrefix << srcFile->getFileName();
    const string binId( id.str() );

    addAttribute<AstSourceNodeLink>( file, id.str(), srcFile, 0 );

#if USE_ROSE_DWARF_SUPPORT
    
    for( size_t idx( 0 ); idx < srcFile->get_numberOfTraversalSuccessors(); ++idx )
    {
        annotate( srcFile->get_traversalSuccessorByIndex( idx ), srcId, binId );
    }

#endif
}

void AsmToSourceMapper::annotate( SgNode *node, const std::string &srcId, const std::string &binId )
{
    typedef std::multiset<AsmMappingInfo>::iterator iterator;

    if( node == NULL )
        return;

    if( isSgLocatedNode( node ) )
    {
        AstBinaryNodeLink *nodeLink( NULL );

        Sg_File_Info *fileNode( node->get_file_info() );
        Sg_File_Info *startNode( node->get_startOfConstruct() );
        Sg_File_Info *endNode( node->get_endOfConstruct() );

        /*if( fileNode->get_filenameString() == "compilerGenerated" )
            return;*/

        iterator begin( 
                asmMappingInfos.lower_bound( AsmMappingInfo( fileNode->get_filenameString(),
                                                             startNode->get_line() ) ) );
        iterator end( 
                asmMappingInfos.upper_bound( AsmMappingInfo( fileNode->get_filenameString(),
                                                             endNode->get_line() ) ) );

        std::vector<std::pair<SgNode *, SgNode *> > address_ranges;

        if( begin == asmMappingInfos.end() )
        {
            assert( end == asmMappingInfos.end() );
        }
        else
        {
            for( iterator it( begin ); it != end; ++it )
            {
                assert( it->fileName() == fileNode->get_filenameString() );
                if( it->line() < startNode->get_line() ||
                    it->line() > endNode->get_line() )
                    continue;

                // annotating the binary AST
                SgNode *startBlock( it->start()->get_parent() );
                SgNode *endBlock( it->end()->get_parent() );
                assert( it->end()->get_parent() );
                assert( startBlock->get_parent() == endBlock->get_parent() );

                SgNode *fun( startBlock->get_parent() );

                for( int blockIdx( fun->getChildIndex( startBlock ) );
                     blockIdx < fun->getChildIndex( endBlock ) + 1;
                     ++blockIdx )
                {
                    SgNode *block( fun->get_traversalSuccessorByIndex( blockIdx ) );
                    int idxStart( 0 );
                    int idxEnd( block->get_numberOfTraversalSuccessors() );

                    if( block == startBlock )
                        idxStart = block->get_childIndex( it->start() );
                    if( block == endBlock )
                        idxEnd = block->get_childIndex( it->end() ) + 1; 

                    for( int idx( idxStart ); idx < idxEnd; ++idx )
                    {
                        SgNode *tmp( block->get_traversalSuccessorByIndex( idx ) );
                        addAttribute<AstSourceNodeLink>( tmp, binId, node, 0 );
                    }
                }
                
                pair<SgNode *, SgNode *> newEntry( it->start(), it->end() );

                if( node->attributeExists( srcId ) )
                {
                    nodeLink = dynamic_cast<AstBinaryNodeLink *>( 
                            node->getAttribute( srcId ) );
                }
           
                else {
                    nodeLink = new AstBinaryNodeLink;
                    node->addNewAttribute( srcId, nodeLink );
                }

                // annotate the source AST
                if( address_ranges.size() == 0 )
                    address_ranges.push_back( newEntry );
                // try to merge adjacent address ranges ...
                else
                {
                    pair<SgNode *, SgNode *> &prev_range( *( address_ranges.end() - 1 ) );
                    
                    SgNode *prev_end( prev_range.second );

                    if( prev_end->get_parent() == it->start()->get_parent() )
                    {
                        assert( prev_end->get_parent() );
                        SgNode *parent( prev_end->get_parent() );
                        assert( parent == it->start()->get_parent() );

                        std::vector<SgNode *> instructions (
                                NodeQuery::querySubTree( file, V_SgAsmInstruction ) );

                        std::vector<SgNode *>::iterator itr_prev_end( 
                                find( instructions.begin(), instructions.end(), prev_end ) );

                        if( itr_prev_end != instructions.end() )
                        {
                            ++itr_prev_end;
                            if( *itr_prev_end == it->start() )
                            {
                                prev_range.second = it->end();
                                //cout << "new end: " << it->end() << " "
                                //     << address_ranges[address_ranges.size()-1].second
                                //     << endl;
                            }
                        }
                    }

                    /*SgAsmInstruction *last_end( isSgAsmInstruction( prev_range->second ) );
                    SgAsmInstruction *current_start( isSgAsmInstruction( it->start() ) );*/

                    /*if( prev_range.second == it->start() )
                    {
                        prev_range.second = it->end();
                        cout << "new end: " << it->end() << " " 
                             << address_ranges[address_ranges.size()-1].second
                             << endl;
                    }
                    else*/
                        address_ranges.push_back( newEntry );
                }

                typedef vector<pair<SgNode *, SgNode *> >::iterator range_iterator;

                for( range_iterator jt( address_ranges.begin() );
                     jt != address_ranges.end();
                     ++jt )
                {
                    bool insert( true );

                    for( range_iterator kt( nodeLink->begin() );
                         kt != nodeLink->end();
                         ++kt )
                    {
                        SgAsmInstruction *start( isSgAsmInstruction( jt->first ) );
                        SgAsmInstruction *end  ( isSgAsmInstruction( jt->second ) );
                        SgAsmInstruction *cur_start( isSgAsmInstruction( kt->first ) );
                        SgAsmInstruction *cur_end  ( isSgAsmInstruction( kt->second ) );

                        std::pair<SgNode *, SgNode *> &range( *kt );

                        if( start->get_address() >= cur_start->get_address() )
                        {
                            insert = false;
                            if( end->get_address() > cur_end->get_address() )
                                range.second = end;

                            break;
                        }
                    }

                    if( insert )
                        nodeLink->push_back( *jt );
                }
                //copy( address_ranges.begin(), address_ranges.end(), 
                //      back_inserter( *nodeLink ) );

                assert( it->fileName() == fileNode->get_filenameString() );
            }

            if( isSgBasicBlock( node ) && address_ranges.size() > 0 )
            {
                SgNode *parent( isSgFunctionDefinition( node->get_parent() ) );
                if( parent )
                {
                    if( parent->attributeExists( srcId ) )
                    {
                        AstNodeLinkAttribute *link( dynamic_cast<AstNodeLinkAttribute *>( parent->getAttribute( srcId ) ) );
                        assert( link );
                        copy( nodeLink->begin(), nodeLink->end(), back_inserter( *link ) );
                    }
                    else
                        parent->addNewAttribute( srcId, nodeLink->copy() );

                    SgNode *grandparent( isSgFunctionDeclaration( parent->get_parent() ) );
                    if( grandparent )
                    {
                        assert( nodeLink->size() == 1 );
                        SgNode *binFunNode( nodeLink->at( 0 ).first->get_parent()->get_parent() );

                        addAttribute<AstBinaryNodeLink>( grandparent, srcId, binFunNode, 0 );
                        addAttribute<AstSourceNodeLink>( binFunNode, binId, grandparent, 0 );
                    }
                }
            }
        }
    }

    for( size_t idx( 0 ); idx < node->get_numberOfTraversalSuccessors(); ++idx )
    {
        annotate( node->get_traversalSuccessorByIndex( idx ), srcId, binId );
    }
}

template< typename NodeLink >
void AsmToSourceMapper::addAttribute( SgNode *node, const string &id, SgNode *start, SgNode *end )
{
    NodeLink *nodeLink( NULL );
    pair<SgNode *, SgNode *> newEntry( start, end );

    if( node->attributeExists( id ) )
    {
        nodeLink = dynamic_cast<NodeLink *>( node->getAttribute( id ) );

        if( find( nodeLink->begin(), nodeLink->end(), newEntry ) != nodeLink->end() )
            return;
    }
    else
    {
        nodeLink = new NodeLink;
        node->addNewAttribute( id, nodeLink );
    }

    nodeLink->push_back( newEntry );
}
