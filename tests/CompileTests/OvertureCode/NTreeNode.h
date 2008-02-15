#ifndef __KKC_NTREENODE__
#define __KKC_NTREENODE__


// *wdh* turn off assertions.
// define ASSERTIONS_ON
#undef ASSERTIONS_ON

#include <iostream.h>
#include "Overture.h"
#include "NTreeNodeExceptions.h"

// NTreeNode is a class used to build fixed degree tree container classes,it is not intended for use all by itself

template<int degree, class Data>
class NTreeNode
{
public:

  typedef Data DataType;
  inline NTreeNode();
  inline NTreeNode(Data &data_);
  inline NTreeNode(Data &data_, NTreeNode *trunk_);
  inline NTreeNode(NTreeNode *trunk_);
  inline NTreeNode(NTreeNode<degree, Data> &node_) : data(node_.data) 
    {
      for (int i=0; i<degree; i++) leaves[i] = node_.leaves[i];
      trunk = node_.trunk;
    }
  
  inline ~NTreeNode();
  
  inline int add(Data &data_);
  inline int add(int d, Data &data_);
  inline int add(int d);  // add an empty node
  inline int del(int nDel);
  inline int change(NTreeNode *nPtr);
  inline int change(int d, NTreeNode *nPtr);
  inline bool querry(int d); // returns false for NULL and true for alive
  inline bool querry(); // returns false if there is no trunk, true otherwise
  inline int nullify();

  inline NTreeNode & getTrunk();
  inline const NTreeNode & getTrunk() const;
  inline NTreeNode & getLeaf(int d);
  inline const NTreeNode & getLeaf(int d) const;
  inline Data & getData() { return data; }
  inline const Data & getData() const { return data; }
  
private:
  int isValid;
  Data data;
  
  NTreeNode *trunk;
  NTreeNode *leaves[degree];
};

//
// inlined methods for NTreeNode
//
//\begin{>NTreeNode.tex}{\subsection{Default Constructor}}
template<int degree, class Data>
inline
NTreeNode<degree,Data>::
NTreeNode()
//======================================================
// /Purpose : build and initialize a tree node
//\end 
//======================================================
{
  //
  // set the internal check isValid to 1
  //
  isValid = 1;
  // 
  // nullify all the leaves
  //
  nullify();
}

//\begin{>>NTreeNode.tex}{\subsection{Constructor}}
template<int degree, class Data>
inline
NTreeNode<degree,Data>::
NTreeNode(Data &data_) 
//======================================================
// /Purpose : build and initialize a tree node given the data for the node to contain
// /data (input) : reference to the data to be stored in the node
//\end
//======================================================
{
  //
  // set the internal check isValid to 1
  //
  isValid = 1;
  // 
  // nullify all the leaves
  //
  nullify();
  //
  // assign the internal data reference
  //
  data=data_;
  
}

//\begin{>>NTreeNode.tex}{\subsection{Constructor}}
template<int degree, class Data>
inline
NTreeNode<degree,Data>::
NTreeNode(Data &data_, NTreeNode *trunk_) 
//======================================================
// /Purpose : build and initialize a tree node
// /data\_ (input) : reference to the data to be stored in the node
// /trunk\_ (input) : pointer to the trunk
//\end 
//======================================================
{
  //
  // set the internal check isValid to 1
  //
  isValid = 1;
  // 
  // nullify all the leaves
  //
  nullify();
  //
  // assign data members
  //
  data=data_;
  trunk=trunk_;
}

//\begin{>>NTreeNode.tex}{\subsection{Constructor}}
template<int degree, class Data>
inline
NTreeNode<degree,Data>::
NTreeNode(NTreeNode *trunk_) 
//======================================================
// /Purpose : build and initialize a tree node
// /data\_ (input) : reference to the data to be stored in the node
// /trunk\_ (input) : pointer to the trunk
//\end 
//======================================================
{
  //
  // set the internal check isValid to 1
  //
  isValid = 1;
  // 
  // nullify all the leaves
  //
  nullify();
  trunk=trunk_;
}

//\begin{>>NTreeNode.tex}{\subsection{Destructor}}
template<int degree, class Data>
inline
NTreeNode<degree,Data>::
~NTreeNode()
//======================================================
// /Purpose : destroy a node and all its leaves
//\end
//======================================================
{ 
  //
  // confirm that this node is not corrupt (ie, deallocated elsewhere)
  //
#ifdef ASSERTIONS_ON
#if 1
  AssertException (isValid == 1, InvalidNode());
#else
  AssertException<InvalidNode> (isValid == 1);
#endif
#endif
  //
  // delete the leaves
  //
  for (int d=0; d<degree; d++) 
    if (leaves[d]!=NULL) del(d); 

  //
  // make sure the node is invalid before destruction is complete
  //
  isValid = -1;

}

//\begin{>>NTreeNode.tex}{\subsection{add}}
template<int degree, class Data>
inline
int 
NTreeNode<degree,Data>::
add(Data &data)
//======================================================
// /Purpose : add a data leaf to the next available leaf position
// /data (input) : data item to be stored
// /Returns : 0 on success
// /Throws :  
// \begin{itemize} 
//  \item TreeDegreeViolation : if the tree bookkeeping is corrupt
//  \item NodeFullError : if all the leaves in this node are ful
// \end{itemize}
//\end
//======================================================
{ 
  int d=0; 
  while(d<degree && leaves[d]!=NULL) 
    d++; 

#ifdef ASSERTIONS_ON
#if 1
  AssertException (d<degree && d>=0, TreeDegreeViolation());
  AssertException (leaves[d]==NULL, NodeFullError());
#else
  AssertException<TreeDegreeViolation> (d<degree && d>=0);
  AssertException<NodeFullError> (leaves[d]==NULL);
#endif
#endif

    //assert(d<degree && leaves[d]==NULL); 
  leaves[d] = new NTreeNode( data, this);
  return 0;
}

//\begin{>>NTreeNode.tex}{\subsection{add}}
template<int degree, class Data>
inline
int
NTreeNode<degree,Data>::
add(int d, Data &data)
//======================================================
// /Purpose : add a data leaf to a specified leaf
// /d (input) : leaf to store data
// /data (input) : data item to be stored
// /Returns : 0 on success
// /Throws : 
// \begin{itemize}
//  \item TreeDegreeViolation : if d is not a valid leaf number (if d is greater than the degree of the tree or less than zero
//  \item NodeFullError : if d is already used up
// \end{itemize} 
//\end
//======================================================
{ 
  //  assert(d<degree && leaves[d]==NULL); 
#ifdef ASSERTIONS_ON
#if 1
  AssertException (d<degree && d>=0, TreeDegreeViolation());
  AssertException (leaves[d]==NULL, NodeFullError());
#else
  AssertException<TreeDegreeViolation> (d<degree && d>=0);
  AssertException<NodeFullError> (leaves[d]==NULL);
#endif
#endif
  leaves[d] = new NTreeNode( data, this);
  return 0;
}

//\begin{>>NTreeNode.tex}{\subsection{add}}
template<int degree, class Data>
inline
int
NTreeNode<degree,Data>::
add(int d)
//======================================================
// /Purpose : add a data leaf to a specified leaf
// /d (input) : leaf to store data
// /data (input) : data item to be stored
// /Returns : 0 on success
// /Throws : 
// \begin{itemize}
//  \item TreeDegreeViolation : if d is not a valid leaf number (if d is greater than the degree of the tree or less than zero
//  \item NodeFullError : if d is already used up
// \end{itemize} 
//\end
//======================================================
{ 
  //  assert(d<degree && leaves[d]==NULL); 
#ifdef ASSERTIONS_ON
#if 1
  AssertException (d<degree && d>=0, TreeDegreeViolation());
  AssertException (leaves[d]==NULL, NodeFullError());
#else
  AssertException<TreeDegreeViolation> (d<degree && d>=0);
  AssertException<NodeFullError> (leaves[d]==NULL);
#endif
#endif
  leaves[d] = new NTreeNode(this);
  return 0;
}

//\begin{>>NTreeNode.tex}{\subsection{del}}
template<int degree, class Data>
inline
int
NTreeNode<degree,Data>::
del(int nDel) 
//======================================================
// /Purpose : delete a specified leaf
// /nDel (input) : leaf to delete
// /Returns : 0 on success
// /Throws : 
// \begin{itemize}
//  \item TreeDegreeViolation : if d is not a valid leaf number (if d is greater than the degree of the tree or less than zero
// \end{itemize}
//\end
//======================================================
{ 
  //cout << "inside del"<<endl;
#ifdef ASSERTIONS_ON
#if 1
  AssertException (nDel>=0 && nDel<degree, TreeDegreeViolation());
#else
  AssertException<TreeDegreeViolation> (nDel>=0 && nDel<degree);
#endif
#endif
  //assert(nDel>=0 && nDel<degree); 
  if (leaves[nDel]!=NULL) {
    delete leaves[nDel]; 
    leaves[nDel] = NULL;
  }
  //  trunk = NULL;
  return 0;
}

//\begin{>>NTreeNode.tex}{\subsection{change}}
template<int degree, class Data>
inline
int
NTreeNode<degree,Data>::
change(NTreeNode<degree, Data> *nPtr)
//======================================================
// /Purpose : change a node's trunk
// /nPtr (input) : pointer to the new trunk
// /Returns : 0 on success
// /Throws : nothing 
//\end 
//======================================================
{ 
  trunk = nPtr; 
  return 0;
}

//\begin{>>NTreeNode.tex}{\subsection{change}}
template<int degree, class Data>
inline
int
NTreeNode<degree,Data>::
change(int d, NTreeNode<degree, Data> *nPtr) 
//======================================================
// /Purpose : changes a particular leaf
// /d (input) : leaf to change
// /nPtr (input) : pointer to the new leaf
// /Returns : 0 on success
// /Throws :
// \begin{itemize} 
//  \item TreeDegreeViolation : if d is not a valid leaf number
// \end{itemize}
//\end 
//======================================================
{ 
#ifdef ASSERTIONS_ON
#if 1
  AssertException (d<degree && d>=0, TreeDegreeViolation());
#else
  AssertException<TreeDegreeViolation> (d<degree && d>=0);
#endif
#endif
  //assert(d<degree && d>=0); 
  leaves[d] = nPtr; 
  return 0;
}

//\begin{>>NTreeNode.tex}{\subsection{change}}
template<int degree, class Data>
inline
bool
NTreeNode<degree,Data>::
querry(int d) 
//======================================================
// /Purpose : see if a particular leaf has data
// /d (input) : leaf to querry
// /Returns : false if leaf d is NULL, true otherwise
// /Throws :
// \begin{itemize} 
//  \item TreeDegreeViolation : if d is not a valid leaf number
// \end{itemize}
//\end 
//======================================================
{
#ifdef ASSERTIONS_ON
#if 1
  AssertException (d<degree && d>=0, TreeDegreeViolation());
#else
  AssertException<TreeDegreeViolation> (d<degree && d>=0);
#endif
#endif
  return leaves[d] != NULL;
//   if (leaves[d] == NULL) 
//     return false;
//   else
//     return true;
}

//\begin{>>NTreeNode.tex}{\subsection{change}}
template<int degree, class Data>
inline
bool
NTreeNode<degree,Data>::
querry()
//======================================================
// /Purpose : see if the trunk has data
// /d (input) : leaf to querry
// /Returns : false if the trunk pointer is NULL, true otherwise
// /Throws :
// \begin{itemize} 
//  \item TreeDegreeViolation : if d is not a valid leaf number
// \end{itemize}
//\end 
//======================================================
{
  return trunk!=NULL;
//   if (trunk == NULL) 
//     return false;
//   else
//     return true;
}
			 
template<int degree, class Data>
inline
int
NTreeNode<degree,Data>::
nullify()
{ 
  trunk = NULL; 
  for (int d=0; d<degree; d++) 
    leaves[d] = NULL; 
  return 0;
}

//\begin{>>NTreeNode.tex}{\subsection{getTrunk}}
template<int degree, class Data>
inline
NTreeNode<degree,Data> & 
NTreeNode<degree,Data>::
getTrunk() 
//======================================================
// /Purpose : return a reference to the trunk node
// /Returns : a reference the the trunk
// /Throws : nothing
//\end 
//======================================================
{ 
  return *trunk; 
}

//\begin{>>NTreeNode.tex}{\subsection{const getTrunk}}
template<int degree, class Data>
inline
const NTreeNode<degree,Data> & 
NTreeNode<degree,Data>::
getTrunk() const
//======================================================
// /Purpose : return a const reference to the trunk node
// /Returns : a const reference the the trunk
// /Throws : nothing
//\end
//======================================================
{ 
  return *trunk; 
}

//\begin{>>NTreeNode.tex}{\subsection{const getLeaf}}
template<int degree, class Data>
inline
NTreeNode<degree,Data> & 
NTreeNode<degree,Data>::getLeaf(int d) 
//======================================================
// /Purpose : return a reference to a specific leaf node
// /d (input) : leaf to return
// /Returns : a reference to the requested leaf
// /Throws :
// \begin{itemize} 
//  \item TreeDegreeViolation : if d is not a valid leaf number
// \end{itemize}
//\end
//======================================================
{ 
#ifdef ASSERTIONS_ON
#if 1
  AssertException(d<degree && d>=0,TreeDegreeViolation());
#else
  AssertException<TreeDegreeViolation> (d<degree && d>=0);
#endif
#endif

  return *leaves[d]; 
}

//\begin{>>NTreeNode.tex}{\subsection{const getLeaf}}
template<int degree, class Data>
inline
const NTreeNode<degree,Data> & 
NTreeNode<degree,Data>::
getLeaf(int d) const 
//======================================================
// /Purpose : return a const reference to a specific leaf node
// /d (input) : leaf to return
// /Returns : a reference to the requested leaf
// /Throws :  
// \begin{itemize} 
//  \item TreeDegreeViolation : if d is not a valid leaf number
// \end{itemize}
//\end
//======================================================
{ 
#ifdef ASSERTIONS_ON
#if 1
  AssertException(d<degree && d>=0, TreeDegreeViolation());
#else
  AssertException<TreeDegreeViolation> (d<degree && d>=0);
#endif
#endif
  return *leaves[d]; 
}

#if 0
template<int degree, class Data>
inline
//const Data & 
Data & 
NTreeNode<degree,  Data>::
getData() 
{ 
  return data; 
}

template<int degree, class Data>
inline
//const Data & 
const Data & 
NTreeNode<degree,  Data>::
getData() const 
{ 
  return data; 
}
#endif

#undef ASSERTIONS_ON
#endif
