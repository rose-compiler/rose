
// New file for include file support code



namespace IncludeFileSupport {

     class InheritedAttribute
        {
          public:
               std::string path_prefix;

               InheritedAttribute ();
               InheritedAttribute ( const InheritedAttribute & X);
        };

     class SynthesizedAttribute
        {
          public:
               std::string path_prefix;
               SgIncludeFile* include_file;

            // std::set<std::string> added_include_path_set;

               SynthesizedAttribute ();
               SynthesizedAttribute (SgIncludeFile* include_file);
               SynthesizedAttribute ( const SynthesizedAttribute & X);
        };

     class PrefixTraversal : public AstTopDownBottomUpProcessing<InheritedAttribute,SynthesizedAttribute>
        {
          public:
               std::set<std::string> added_include_path_set;

               InheritedAttribute evaluateInheritedAttribute ( SgNode* node, InheritedAttribute inheritedAttribute );
               SynthesizedAttribute evaluateSynthesizedAttribute ( SgNode* node, InheritedAttribute inheritedAttribute, SubTreeSynthesizedAttributes synthesizedAttributeList );
        };


  // void headerFilePrefix ( SgIncludeFile* includeFile );
     std::set<std::string> headerFilePrefix ( SgIncludeFile* includeFile );

};


