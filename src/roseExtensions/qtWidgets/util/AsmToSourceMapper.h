
#ifndef ASM_TO_SOURCE_MAPPER_H
#define ASM_TO_SOURCE_MAPPER_H

#include <string>
#include <set>

class SgNode;

/**
 * \brief Mapping Info for AsmToSourceMapper
 * One AsmMappingInfo object represents one line in source-code
 * it stores the line-nr and filename (i.e. a position in source-code)
 * plus a range of SgAsmInstruction 's which where generated out of this source-line 
 */
class AsmMappingInfo
{
    public:
        AsmMappingInfo()
            : relFun_( NULL ),
              start_( NULL ),
              end_( NULL ),
              fileName_( "" ),
              line_( -1 )
        {}

        AsmMappingInfo( SgAsmFunction *relFun,
                        SgAsmInstruction *start,
                        SgAsmInstruction *end,
                        const std::string &fileName, int line )
            : relFun_( relFun ),
              start_( start ),
              end_( end ),
              fileName_( fileName ),
              line_( line )
        {}

        AsmMappingInfo( const std::string &fileName, int line )
            : relFun_( NULL ),
              start_( NULL ),
              end_( NULL ),
              fileName_( fileName ),
              line_( line )
        {}

        AsmMappingInfo( const AsmMappingInfo& other )
            : relFun_( other.relFun_ ),
              start_( other.start_ ),
              end_( other.end_ ),
              fileName_( other.fileName_ ),
              line_( other.line_ )
        {}

        AsmMappingInfo& operator=( const AsmMappingInfo& other )
        {
            relFun_ = other.relFun_;
            start_ = other.start_;
            end_ = other.end_;
            fileName_ = other.fileName_;
            line_ = other.line_;

            return *this;
        }

        bool operator<( const AsmMappingInfo& other ) const
        {
            /*if( fileId_ == other.fileId_ )
                return line_ < other.line_;*/

            /*std::cout << std::dec << fileId_ << " " << other.fileId_ << std::hex << std::endl;*/

            return fileName_ < other.fileName_;
        }

        SgAsmFunction *&relFun()
        { return relFun_; }
        SgAsmFunction * const &relFun() const
        { return relFun_; }

        SgAsmInstruction *&start()
        { return start_; }
        SgAsmInstruction * const &start() const
        { return start_; }
        
        SgAsmInstruction *&end()
        { return end_; }
        SgAsmInstruction * const &end() const
        { return end_; }

        std::string &fileName()
        { return fileName_; }
        const std::string &fileName() const
        { return fileName_; }

        int &line()
        { return line_; }
        const int &line() const
        { return line_; }

    private:
        SgAsmFunction *relFun_;

        SgAsmInstruction *start_;
        SgAsmInstruction *end_;

        std::string fileName_;
        int line_;
};

/**
 * \brief Attribute which represents a link to another AST
 */
class AstNodeLinkAttribute
    : public AstAttribute,
      public std::vector< std::pair<SgNode *, SgNode *> >
{
    public:

        virtual ~AstNodeLinkAttribute()
        {}

        virtual AstNodeLinkAttribute *copy()
        {
            return new AstNodeLinkAttribute( *this );
        }
};

/**
 * \brief Attribute which represents a link to a SourceAST
 *    This class basically stores a set of SgNode's which are associated
 *    with the node, where this class was annotated to.
 */
class AstSourceNodeLink
    : public AstNodeLinkAttribute
{
    public:

        virtual ~AstSourceNodeLink()
        {}

        virtual AstSourceNodeLink *copy()
        {
            return new AstSourceNodeLink( *this );
        }
};

/**
 * \brief Attribute which represents a link to a BinaryAST
 *    This class basically stores a set of SgNode's which are associated
 *    with the node, where this class was annotated to.
 */
class AstBinaryNodeLink
    : public AstNodeLinkAttribute
{
    public:
        virtual ~AstBinaryNodeLink()
        {}

        virtual AstBinaryNodeLink *copy()
        {
            return new AstBinaryNodeLink( *this );
        }
};

/**
 * \brief Creates Annotation with mapping: Source AST <-> Binary AST using dwarf information
 * 
 *  Usage: \n
 *  Create a new AsmToSourceMapper object for every binary file, and add
 *  the sourcefile via addFile(). This class uses dwarf information which is 
 *  included in the binary if it was compiled with the \c -g option.
 *  The annotation is stored in the AST as annotation objects i.e.
 *  AstBinaryNodeLink / AstSourceNodeLink
 */
class AsmToSourceMapper
{
    public:

        /// Constructor, extracts the dwarf information from the binary
        /// and stores them in
        AsmToSourceMapper( SgBinaryComposite *file );
        
        
        /// This function annotates the Source and the binary AST with the
        /// mapping information
        void annotate( SgSourceFile *file );

    private:

        /// holds mapping data
        /// order by fileId and line number.
        /// needs to be a multiset because every source file line could correspond
        /// to more than one dwarf line information
        std::multiset<AsmMappingInfo> asmMappingInfos;

        SgBinaryComposite *file;

        void addFile();

        void annotate( SgNode *node, const std::string &srcId, const std::string &binId );

        template< typename NodeLink >
        void addAttribute( SgNode *node, const std::string &id, SgNode *start, SgNode *end );
};

#endif
