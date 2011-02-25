
#ifndef ITEMTREENODE_H
#define ITEMTREENODE_H

#include <QList>
#include <QVariant>
#include <QStringList>
#include <QModelIndex>

class ItemTreeModel;

/**
 * \brief Node for building a tree which can be used to implement QAbstractItemModel
 * For usage example see on of the many derived classes. \n
 * A similar class is provided now by Qt in the QTreeWidget - consider using QTreeWidget instead of QTreeView
 */
class ItemTreeNode
{
    public:
        // Constructor to use for root-node
        // to construct tree use addChild instead
        ItemTreeNode();
        virtual ~ItemTreeNode();

        /// Returns the data of this node i.e. what is displayed, has to be reimplemented!
        /// @param role a combination of enum Qt::ItemDataRole
        /// @param column the column where this QVariant is displayed
        virtual QVariant data(int role, int column=0) const;

        virtual ItemTreeNode * getParent() const           { return parent; }

        virtual Qt::ItemFlags flags (int column) const;

        virtual ItemTreeNode * child(int id)
        {
            return id >= children.size() ? NULL : children[id];
        }

        virtual int childrenCount() const    { return children.size(); }

        virtual int getChildId(ItemTreeNode * child);

        /// Add child, notifies model if setNotifiyModel was called
        virtual int addChild(ItemTreeNode * c);

        /// Adds child at a specified position, id: id>=0 and id<=childrenCount
        virtual void addChildAt(ItemTreeNode * c, int id);


        /// Adds a list of children
        virtual void addChildren(const QList<ItemTreeNode*> & list);

        /// Removes all children and deletes them
        virtual void removeAllChildren();

        /// Removes all children from the tree, but does not delete them
        /// the ownership is taken by the caller
        /// normally the taken children are added at another node
        void takeAllChildren(QList<ItemTreeNode * > & output);

        /// Removes child, notifies model if setNotifiyModel was called
        virtual void removeChild(int id);



        /// Returns the row of this Node
        /// goes to the parent and returns the id it has in its children list
        virtual int getRow() const ;

        /// List of Column-Header Captions
        virtual QStringList sectionHeader() const;


        /// If true the item spanns all columns in a row
        virtual bool isFirstColumnSpanned () const    { return false;}

        /// If you modify the ItemTree after using it in a model (via addChild)
        /// you have to call this function on the root node to set the model
        /// (such that model->beginInsertRows is called)
        virtual void setNotifyModel(ItemTreeModel * m);


        template <typename LessThan>
        inline void sortChildren(LessThan compareFunction)  {
            qSort(children.begin(),children.end(), compareFunction );
        }

        template <typename LessThan>
        inline void sortChildrenStable(LessThan compareFunction)  {
            qStableSort(children.begin(),children.end(), compareFunction );
        }

    protected:
        ItemTreeNode * parent;
        QList<ItemTreeNode*> children;

        /// The model pointer is used to call beginInsertRows etc.
        /// if this mechanism is not used (tree not changed when shown)
        /// just leave at default NULL
        ItemTreeModel * notifyModel;
};


#include <QIcon>

/**
 * \brief Node which displays a Property-Value pair with optional icon
 */
class PropertyValueNode : public ItemTreeNode
{
    public:
        PropertyValueNode(const QString & property, const QVariant & value);

         virtual QVariant data(int role, int column=0) const;
         virtual QStringList sectionHeader() const;

         virtual bool isFirstColumnSpanned () const    { return firstColumnSpanned; }
         virtual void setFirstColumnSpanned(bool b)    { firstColumnSpanned = b;    }

         const QIcon & getIcon() const { return icon; }
         void setIcon(const QIcon & i) { icon=i; }

     protected:
         QString prop;
         QVariant val;

         QIcon icon;

         bool firstColumnSpanned;
};



#endif
