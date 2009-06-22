
#ifndef ASM_TO_SOURCE_MAPPER_H
#define ASM_TO_SOURCE_MAPPER_H

#include <string>
#include <set>

class SgNode;

struct FileInfo
{
    FileInfo( const FileInfo& f )
        : fileId( f.fileId ),
          line( f.line ),
          column( f.column ),
          node( f.node )
    {}

    FileInfo( const std::string& fileId_, int line_, int column_, SgNode *node_ = NULL )
        : fileId( fileId_ ),
          line( line_ ),
          column( column_ ),
          node( node_ )
    {}

    bool operator< ( const FileInfo& f ) const
    {
        if( fileId == f.fileId )
            return line < f.line;

        return fileId < f.fileId;
    }

    bool operator> ( const FileInfo& f ) const
    {
        return !operator<( f );
    }

    bool operator== ( const FileInfo& f ) const
    {
        if( fileId == f.fileId &&
            line == f.line )
            return true;

        return false;
    }

    bool operator!= ( const FileInfo& f ) const
    {
        return !operator==( f );
    }

    std::string fileId;
    int line;
    int column;

    SgNode *node;
};

class AstNodeLinkAttribute
    : public AstAttribute
{
    public:
        AstNodeLinkAttribute( SgNode *node_ )
            : node( node_ )
        {}

        virtual ~AstNodeLinkAttribute()
        {}

        virtual SgNode *getNode()
        {
            return node;
        }

        virtual AstNodeLinkAttribute *copy()
        {
            return new AstNodeLinkAttribute( *this );
        }

    protected:
        SgNode *node;
};

class AstSourceNodeLink
    : public AstNodeLinkAttribute
{
    public:
        AstSourceNodeLink( SgNode *node )
            : AstNodeLinkAttribute( node )
        {}

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
        AstBinaryNodeLink( SgNode *node )
            : AstNodeLinkAttribute( node )
        {}

        virtual ~AstBinaryNodeLink()
        {}

        virtual AstBinaryNodeLink *copy()
        {
            return new AstBinaryNodeLink( *this );
        }
};

class AsmFunctions
{
   public:
       // expects a node of a binary AST
      AsmFunctions( SgBinaryFile *node );
      void addFile( SgBinaryFile *node );

      // expects a node of a source AST
      void annotate( SgNode *node ) const;

   private:
      typedef Rose_STL_Container<SgNode *> SgNodeContainer;
      typedef Rose_STL_Container<SgNode *>::const_iterator const_iterator;
      
      void matchDwarfLine( SgAsmStatement *node );

      std::set<FileInfo> addressToFileInfo;
      SgNodeContainer dwarfLines;
      std::map<int, std::string> binaryFileIdToName;
};

#endif
