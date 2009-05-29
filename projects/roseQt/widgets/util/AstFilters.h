#ifndef ASTFILTERS_H
#define ASTFILTERS_H

#include <QString>

#include "MetricsConfig/MetricsInfo.h"

class SgNode;


class AstFilterInterface
{
    public:
        virtual                    ~AstFilterInterface() {}
        virtual bool                displayNode(SgNode * n) const =0;
        virtual AstFilterInterface *copy() const =0;
};


class AstFilterAll : public AstFilterInterface
{
    public:
        virtual                    ~AstFilterAll() {}
        virtual bool                displayNode(SgNode *) const  { return true; }
        virtual AstFilterInterface *copy() const { return new AstFilterAll( ); }
};

class AstFilterAnd: public AstFilterInterface
{
    public:
        AstFilterAnd( AstFilterInterface *f1, AstFilterInterface *f2 )
            : filter1( f1 ),
              filter2( f2 )
        {}

        virtual ~AstFilterAnd() {}

        virtual bool displayNode( SgNode *node ) const
        {
            return  filter1->displayNode( node ) && filter2->displayNode( node );
        }

        virtual AstFilterInterface *copy() const { return new AstFilterAnd( filter1, filter2 ); }

    private:
        AstFilterInterface *filter1;
        AstFilterInterface *filter2;
};


class AstFilterFileByName : public AstFilterInterface
{
    public:
        AstFilterFileByName(const QString & file) : filename(file) {}
        virtual ~AstFilterFileByName() {}

        virtual bool displayNode(SgNode * node) const;

        virtual AstFilterInterface *copy() const { return new AstFilterFileByName( filename ); }

    protected:
        QString filename;
};


class AstFilterFileById : public AstFilterInterface
{
    public:
        AstFilterFileById(int id) : fileId(id) {}
        virtual ~AstFilterFileById() {}

        virtual bool displayNode(SgNode * node) const;

        virtual AstFilterInterface *copy() const { return new AstFilterFileById( fileId ); }

    protected:
        int fileId;
};

class AstFilterMetricAttributeByThreshold : public AstFilterInterface
{
    public:
        AstFilterMetricAttributeByThreshold( const QString& name_, const MetricsInfo& info_, int threshold_ )
            : name( name_ ),
              info( info_ ),
              threshold( threshold_ )
        {}
        virtual ~AstFilterMetricAttributeByThreshold() {}

        virtual bool displayNode( SgNode *node ) const;

        virtual AstFilterInterface *copy() const { return new AstFilterMetricAttributeByThreshold( name, info, threshold ); }

    private:
        const QString name;
        const MetricsInfo info;
        const int threshold;
};


#endif
