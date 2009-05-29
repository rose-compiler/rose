
#include "rose.h"

#include <iostream>
#include <QDebug>

#include "AsmInstructionsBar.h"

using namespace std;

AsmInstructionsBar::AsmInstructionsBar( QWidget *parent )
   : QGraphicsView( parent )
{
   setScene( &scene );
   setDragMode( QGraphicsView::ScrollHandDrag );
}

AsmInstructionsBar::~AsmInstructionsBar()
{}

void AsmInstructionsBar::setNode( SgNode *node )
{
   cout << "setting up AsmInstructionsBar" << endl;

   typedef Rose_STL_Container<SgNode *>::iterator iterator;

   Rose_STL_Container<SgNode *> asmInstructions =
      NodeQuery::querySubTree( node, V_SgAsmInstruction );

   uint64_t offset( 0 );
   uint64_t block_offset( 0 );
   uint64_t function_offset( 0 );

   for( iterator i = asmInstructions.begin(); i != asmInstructions.end(); ++i )
   {
      SgAsmx86Instruction *instr = isSgAsmx86Instruction( *i );
      assert( instr );

      uint64_t curAddress( instr->get_address() );
      uint64_t size ( instr->get_raw_bytes().size() );

      cout << "  size:" << instr->get_raw_bytes().size() << endl;
      cout << "offset: " << offset << endl;

      uint64_t width = size * 20;

      scene.addRect( static_cast<float>(offset), 0.0f,
                     static_cast<float>(width) , 20.0f );

      SgAsmBlock *parentBlock( isSgAsmBlock( instr->get_parent() ) );
      assert( parentBlock );

      uint64_t instr_index = parentBlock->get_childIndex( instr );
      if( instr_index == 0 )
      {
         block_offset = offset;
      }
      if( instr_index == parentBlock->get_numberOfTraversalSuccessors()-1 )
      {
         scene.addRect( static_cast<float>(block_offset)             , -20.0f,
                        static_cast<float>(offset-block_offset+width), 20.0f );
      }

      SgAsmFunctionDeclaration *parentFunction( isSgAsmFunctionDeclaration( parentBlock->get_parent() ) );
      assert( parentFunction );

      uint64_t block_index = parentFunction->get_childIndex( parentBlock );

      if( block_index == 0 && instr_index == 0 )
         cout << "start of new block and function ..." << endl;

      if( block_index == 0 )
      {
         function_offset = offset;
      }
      if( block_index == parentFunction->get_numberOfTraversalSuccessors()-1 )
      {
         scene.addRect( static_cast<float>(function_offset)             , -40.0f,
                        static_cast<float>(offset-function_offset+width), 20.0f );
      }

      offset += width;
   }
}
