
class CountTraversal : public SgSimpleProcessing
   {
     public:
       // Local Accumulator Attribute
          int count;
          bool previousInstructionWasNop;
          SgAsmInstruction* nopSequenceStart;
          std::vector<std::pair<SgAsmInstruction*,int> > nopSequences;

          CountTraversal() : count(0), previousInstructionWasNop(false) {}
          void visit ( SgNode* n );
   };

