
#ifndef ASM_TO_SOURCE_MAPPER_H
#define ASM_TO_SOURCE_MAPPER_H

#include <string>
#include <set>

class SgNode;

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

        AsmMappingInfo( SgAsmFunctionDeclaration *relFun,
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

        SgAsmFunctionDeclaration *&relFun()
        { return relFun_; }
        SgAsmFunctionDeclaration * const &relFun() const
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
        SgAsmFunctionDeclaration *relFun_;

        SgAsmInstruction *start_;
        SgAsmInstruction *end_;

        std::string fileName_;
        int line_;
};

class AstNodeLinkAttribute
    : public AstAttribute,
      public std::vector< std::pair<SgNode *, SgNode *> >
{
    public:
        /*AstNodeLinkAttribute( SgNode *node_ )
            : node( node_ )
        {}*/

        virtual ~AstNodeLinkAttribute()
        {}

        virtual AstNodeLinkAttribute *copy()
        {
            return new AstNodeLinkAttribute( *this );
        }
};

class AstSourceNodeLink
    : public AstNodeLinkAttribute
{
    public:
        /*AstSourceNodeLink( SgNode *node )
            : AstNodeLinkAttribute( node )
        {}*/

        virtual ~AstSourceNodeLink()
        {}

        virtual AstSourceNodeLink *copy()
        {
            return new AstSourceNodeLink( *this );
        }
};

class AstBinaryNodeLink
    : public AstNodeLinkAttribute
{
    public:
        /*AstBinaryNodeLink( SgNode *node )
            : AstNodeLinkAttribute( node )
        {}*/

        virtual ~AstBinaryNodeLink()
        {}

        virtual AstBinaryNodeLink *copy()
        {
            return new AstBinaryNodeLink( *this );
        }
};

class AsmToSourceMapper
{
    public:

        AsmToSourceMapper( SgBinaryFile *file );
        
        void annotate( SgSourceFile *file );

    private:

        /// holds mapping data
        //  order by fileId and line number.
        //  needs to be a multiset because every source file line could correspond
        //  to more than one dwarf line information
        std::multiset<AsmMappingInfo> asmMappingInfos;

        SgBinaryFile *file;

        void addFile();

        void annotate( SgNode *node, const std::string &srcId, const std::string &binId );

        template< typename NodeLink >
        void addAttribute( SgNode *node, const std::string &id, SgNode *start, SgNode *end );
};

#endif
