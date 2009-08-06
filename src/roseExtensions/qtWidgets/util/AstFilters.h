#ifndef ASTFILTERS_H
#define ASTFILTERS_H

#include <QString>
#include <QObject>

#include "MetricsInfo.h"

class SgNode;

/**
 * \brief Abstract Base class for all AstFilters\
 *
 * A  \c AstFilter can determine which SgNode's should be displayed. Therefore the \c displayNode() has to be
 * implemented in each subclass. This function is called by the view's. If it returns false this node and all it's
 * children are not displayed.
 */
class AstFilterInterface : public QObject
{
    public:
        virtual                    ~AstFilterInterface() {}

        /// Determines if a SgNode should be displayed
        /// if it returns false the children of this node are hidden too (traversion stops at this point)
        virtual bool                displayNode(SgNode * n) const =0;
        virtual AstFilterInterface *copy() const =0;
};



/**
 * \brief AstFilter which always displays all node's (no-filter)
 */
class AstFilterAll : public AstFilterInterface
{
    public:
        virtual                    ~AstFilterAll() {}
        virtual bool                displayNode(SgNode *) const  { return true; }
        virtual AstFilterInterface *copy() const { return new AstFilterAll( ); }
};

/**
 * \brief Class to combine two AstFilter's with logical AND
 *
 * Pass the two arguments of the AND operation in the constructor
 * this class takes ownership of them, and deletes them when they are not needed any more
 */
class AstFilterAnd: public AstFilterInterface
{
    public:
        AstFilterAnd( AstFilterInterface *f1, AstFilterInterface *f2 )
            : filter1( f1 ),
              filter2( f2 )
        {}

        virtual ~AstFilterAnd() { delete filter1; delete filter2; }

        virtual bool displayNode( SgNode *node ) const
        {
            return  filter1->displayNode( node ) && filter2->displayNode( node );
        }

        virtual AstFilterInterface *copy() const { return new AstFilterAnd( filter1->copy(), filter2->copy() ); }

    private:
        AstFilterInterface *filter1;
        AstFilterInterface *filter2;
};

/**
 * \brief Filters an AST by FileName
 *
 *  Filters out all SgNode's which are not in given file
 *  if possible use AstFilterFileById, it uses an int-id instead of the string (faster)
 */
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


/**
 * \brief Filters an AST by FileId
 *
 * Filters out all SgNode's which have a different file-id than the given one
 */
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


/**
 * \brief Filtering by MetricAttribute
 */
class AstFilterMetricAttributeByThreshold : public AstFilterInterface
{
    public:
        /**
         * Constructor
         * @param name_       Name of metric attribute
         * @param info_       MetricsInfo which stores the normalization of the attribute
         * @param threshold_  integer between 0 and 100, displays only nodes which have normalized attribute value greater than threshold in %
         */
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
