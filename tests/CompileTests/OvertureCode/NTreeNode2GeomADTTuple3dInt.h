#ifndef __NTREENODE_GEOMETRIC_ADT_3D_INT__
#define __NTREENODE_GEOMETRIC_ADT_3D_INT__


// *wdh* turn off assertions.
// define ASSERTIONS_ON
#undef ASSERTIONS_ON

#include <iostream.h>
#include "NTreeNodeExceptions.h"

// NTreeNode2GeomADTTuple3dInt is a class used to build fixed 2 tree container classes,it is not intended for use all by itself

#include "GeomADTTuple3dInt.h"

class NTreeNode2GeomADTTuple3dInt
{
public:

  typedef GeomADTTuple3dInt DataType;
  inline NTreeNode2GeomADTTuple3dInt();
  inline NTreeNode2GeomADTTuple3dInt(GeomADTTuple3dInt &data_);
  inline NTreeNode2GeomADTTuple3dInt(GeomADTTuple3dInt &data_, NTreeNode2GeomADTTuple3dInt *trunk_);
  inline NTreeNode2GeomADTTuple3dInt(NTreeNode2GeomADTTuple3dInt *trunk_);
  inline NTreeNode2GeomADTTuple3dInt(NTreeNode2GeomADTTuple3dInt &node_) : data(node_.data) 
    {
      for (int i=0; i<2; i++) leaves[i] = node_.leaves[i];
      trunk = node_.trunk;
    }
  
  inline ~NTreeNode2GeomADTTuple3dInt();
  
  inline int add(GeomADTTuple3dInt &data_);
  inline int add(int d, GeomADTTuple3dInt &data_);
  inline int add(int d);  // add an empty node
  inline int del(int nDel);
  inline int change(NTreeNode2GeomADTTuple3dInt *nPtr);
  inline int change(int d, NTreeNode2GeomADTTuple3dInt *nPtr);
  inline bool querry(int d); // returns false for NULL and true for alive
  inline bool querry(); // returns false if there is no trunk, true otherwise
  inline int nullify();

  inline NTreeNode2GeomADTTuple3dInt & getTrunk();
  inline const NTreeNode2GeomADTTuple3dInt & getTrunk() const;
  inline NTreeNode2GeomADTTuple3dInt & getLeaf(int d);
  inline const NTreeNode2GeomADTTuple3dInt & getLeaf(int d) const;
  inline GeomADTTuple3dInt & getData() { return data; }
  inline const GeomADTTuple3dInt & getData() const { return data; }
  
private:
  int isValid;
  GeomADTTuple3dInt data;
  
  NTreeNode2GeomADTTuple3dInt *trunk;
  NTreeNode2GeomADTTuple3dInt *leaves[2];
};

//
// inlined methods for NTreeNode2GeomADTTuple3dInt
//
//\begin{>NTreeNode2GeomADTTuple3dInt.tex}{\subsection{Default Constructor}}

inline
NTreeNode2GeomADTTuple3dInt::
NTreeNode2GeomADTTuple3dInt()
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

//\begin{>>NTreeNode2GeomADTTuple3dInt.tex}{\subsection{Constructor}}

inline
NTreeNode2GeomADTTuple3dInt::
NTreeNode2GeomADTTuple3dInt(GeomADTTuple3dInt &data_) 
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

//\begin{>>NTreeNode2GeomADTTuple3dInt.tex}{\subsection{Constructor}}

inline
NTreeNode2GeomADTTuple3dInt::
NTreeNode2GeomADTTuple3dInt(GeomADTTuple3dInt &data_, NTreeNode2GeomADTTuple3dInt *trunk_) 
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

//\begin{>>NTreeNode2GeomADTTuple3dInt.tex}{\subsection{Constructor}}

inline
NTreeNode2GeomADTTuple3dInt::
NTreeNode2GeomADTTuple3dInt(NTreeNode2GeomADTTuple3dInt *trunk_) 
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

//\begin{>>NTreeNode2GeomADTTuple3dInt.tex}{\subsection{Destructor}}

inline
NTreeNode2GeomADTTuple3dInt::
~NTreeNode2GeomADTTuple3dInt()
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
  for (int d=0; d<2; d++) 
    if (leaves[d]!=NULL) del(d); 

  //
  // make sure the node is invalid before destruction is complete
  //
  isValid = -1;

}

//\begin{>>NTreeNode2GeomADTTuple3dInt.tex}{\subsection{add}}

inline
int 
NTreeNode2GeomADTTuple3dInt::
add(GeomADTTuple3dInt &data)
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
  while(d<2 && leaves[d]!=NULL) 
    d++; 

#ifdef ASSERTIONS_ON
#if 1
  AssertException (d<2 && d>=0, TreeDegreeViolation());
  AssertException (leaves[d]==NULL, NodeFullError());
#else
  AssertException<TreeDegreeViolation> (d<2 && d>=0);
  AssertException<NodeFullError> (leaves[d]==NULL);
#endif
#endif

    //assert(d<2 && leaves[d]==NULL); 
  leaves[d] = new NTreeNode2GeomADTTuple3dInt( data, this);
  return 0;
}

//\begin{>>NTreeNode2GeomADTTuple3dInt.tex}{\subsection{add}}

inline
int
NTreeNode2GeomADTTuple3dInt::
add(int d, GeomADTTuple3dInt &data)
//======================================================
// /Purpose : add a data leaf to a specified leaf
// /d (input) : leaf to store data
// /data (input) : data item to be stored
// /Returns : 0 on success
// /Throws : 
// \begin{itemize}
//  \item TreeDegreeViolation : if d is not a valid leaf number (if d is greater than the 2 of the tree or less than zero
//  \item NodeFullError : if d is already used up
// \end{itemize} 
//\end
//======================================================
{ 
  //  assert(d<2 && leaves[d]==NULL); 
#ifdef ASSERTIONS_ON
#if 1
  AssertException (d<2 && d>=0, TreeDegreeViolation());
  AssertException (leaves[d]==NULL, NodeFullError());
#else
  AssertException<TreeDegreeViolation> (d<2 && d>=0);
  AssertException<NodeFullError> (leaves[d]==NULL);
#endif
#endif
  leaves[d] = new NTreeNode2GeomADTTuple3dInt( data, this);
  return 0;
}

//\begin{>>NTreeNode2GeomADTTuple3dInt.tex}{\subsection{add}}

inline
int
NTreeNode2GeomADTTuple3dInt::
add(int d)
//======================================================
// /Purpose : add a data leaf to a specified leaf
// /d (input) : leaf to store data
// /data (input) : data item to be stored
// /Returns : 0 on success
// /Throws : 
// \begin{itemize}
//  \item TreeDegreeViolation : if d is not a valid leaf number (if d is greater than the 2 of the tree or less than zero
//  \item NodeFullError : if d is already used up
// \end{itemize} 
//\end
//======================================================
{ 
  //  assert(d<2 && leaves[d]==NULL); 
#ifdef ASSERTIONS_ON
#if 1
  AssertException (d<2 && d>=0, TreeDegreeViolation());
  AssertException (leaves[d]==NULL, NodeFullError());
#else
  AssertException<TreeDegreeViolation> (d<2 && d>=0);
  AssertException<NodeFullError> (leaves[d]==NULL);
#endif
#endif
  leaves[d] = new NTreeNode2GeomADTTuple3dInt(this);
  return 0;
}

//\begin{>>NTreeNode2GeomADTTuple3dInt.tex}{\subsection{del}}

inline
int
NTreeNode2GeomADTTuple3dInt::
del(int nDel) 
//======================================================
// /Purpose : delete a specified leaf
// /nDel (input) : leaf to delete
// /Returns : 0 on success
// /Throws : 
// \begin{itemize}
//  \item TreeDegreeViolation : if d is not a valid leaf number (if d is greater than the 2 of the tree or less than zero
// \end{itemize}
//\end
//======================================================
{ 
  //cout << "inside del"<<endl;
#ifdef ASSERTIONS_ON
#if 1
  AssertException (nDel>=0 && nDel<2, TreeDegreeViolation());
#else
  AssertException<TreeDegreeViolation> (nDel>=0 && nDel<2);
#endif
#endif
  //assert(nDel>=0 && nDel<2); 
  if (leaves[nDel]!=NULL) {
    delete leaves[nDel]; 
    leaves[nDel] = NULL;
  }
  //  trunk = NULL;
  return 0;
}

//\begin{>>NTreeNode2GeomADTTuple3dInt.tex}{\subsection{change}}

inline
int
NTreeNode2GeomADTTuple3dInt::
change(NTreeNode2GeomADTTuple3dInt *nPtr)
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

//\begin{>>NTreeNode2GeomADTTuple3dInt.tex}{\subsection{change}}

inline
int
NTreeNode2GeomADTTuple3dInt::
change(int d, NTreeNode2GeomADTTuple3dInt *nPtr) 
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
  AssertException (d<2 && d>=0, TreeDegreeViolation());
#else
  AssertException<TreeDegreeViolation> (d<2 && d>=0);
#endif
#endif
  //assert(d<2 && d>=0); 
  leaves[d] = nPtr; 
  return 0;
}

//\begin{>>NTreeNode2GeomADTTuple3dInt.tex}{\subsection{change}}

inline
bool
NTreeNode2GeomADTTuple3dInt::
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
  AssertException (d<2 && d>=0, TreeDegreeViolation());
#else
  AssertException<TreeDegreeViolation> (d<2 && d>=0);
#endif
#endif
  return leaves[d] != NULL;
//   if (leaves[d] == NULL) 
//     return false;
//   else
//     return true;
}

//\begin{>>NTreeNode2GeomADTTuple3dInt.tex}{\subsection{change}}

inline
bool
NTreeNode2GeomADTTuple3dInt::
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
			 

inline
int
NTreeNode2GeomADTTuple3dInt::
nullify()
{ 
  trunk = NULL; 
  for (int d=0; d<2; d++) 
    leaves[d] = NULL; 
  return 0;
}

//\begin{>>NTreeNode2GeomADTTuple3dInt.tex}{\subsection{getTrunk}}

inline
NTreeNode2GeomADTTuple3dInt & 
NTreeNode2GeomADTTuple3dInt::
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

//\begin{>>NTreeNode2GeomADTTuple3dInt.tex}{\subsection{const getTrunk}}

inline
const NTreeNode2GeomADTTuple3dInt & 
NTreeNode2GeomADTTuple3dInt::
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

//\begin{>>NTreeNode2GeomADTTuple3dInt.tex}{\subsection{const getLeaf}}

inline
NTreeNode2GeomADTTuple3dInt & 
NTreeNode2GeomADTTuple3dInt::getLeaf(int d) 
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
  AssertException(d<2 && d>=0,TreeDegreeViolation());
#else
  AssertException<TreeDegreeViolation> (d<2 && d>=0);
#endif
#endif

  return *leaves[d]; 
}

//\begin{>>NTreeNode2GeomADTTuple3dInt.tex}{\subsection{const getLeaf}}

inline
const NTreeNode2GeomADTTuple3dInt & 
NTreeNode2GeomADTTuple3dInt::
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
  AssertException(d<2 && d>=0, TreeDegreeViolation());
#else
  AssertException<TreeDegreeViolation> (d<2 && d>=0);
#endif
#endif
  return *leaves[d]; 
}

#if 0

inline
//const GeomADTTuple3dInt & 
GeomADTTuple3dInt & 
NTreeNode2GeomADTTuple3dInt::
getData() 
{ 
  return data; 
}


inline
//const GeomADTTuple3dInt & 
const GeomADTTuple3dInt & 
NTreeNode2GeomADTTuple3dInt::
getData() const 
{ 
  return data; 
}
#endif

#undef ASSERTIONS_ON
#endif
