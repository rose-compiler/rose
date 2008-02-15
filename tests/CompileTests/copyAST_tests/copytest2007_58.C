// test2007_90.C 

/*
This is the very important bug in ROSE. Fix this first!

Andreas

---------- Forwarded message ----------
From: Andreas Sæbjørnsen <andreas.saebjoernsen@gmail.com>
Date: Jun 26, 2007 3:27 PM
Subject: bug in ROSE
To: "Daniel J. Quinlan" <dquinlan@llnl.gov>


When compiling the following code in ROSE:

template <class T>
inline
const T&
NS_MIN( const T& a, const T& b )
 {
   return b < a ? b : a;
 }

typedef int size_type;

class nsString
 {
   public:

     size_type mLength;

     void Right( size_type aCount )
       {
         NS_MIN(mLength, aCount);
       }
 };


I get the following error:

lt-identityTranslator:
/home/andreas/REPOSITORY-SRC/ROSE/June-25a-Unsafe/NEW_ROSE/src/frontend/EDG_SAGE_Connection/sage_gen_be.C:27605:
SgStatement* sage_gen_statement(a_statement*): Assertion `sageStmt !=
__null' failed.

*/


template <class T>
inline
const T&
NS_MIN( const T& a, const T& b )
 {
   return b < a ? b : a;
 }

typedef int size_type;

class nsString
 {
   public:

     size_type mLength;

     void Right( size_type aCount )
       {
         NS_MIN(mLength, aCount);
       }
 };

#if 0
void foo( size_type aCount )
   {
     size_type x;
     size_type y;
     NS_MIN(x,y);
   }
#endif

