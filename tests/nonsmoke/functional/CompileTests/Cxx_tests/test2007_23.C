/*

Hi Dan,

Thought I'd let you know I'm getting errors while compiling a C++
program with ROSE (groff).  I'm using version  4.1.1 of gcc and
version 0.8.9a of ROSE.  The following program reconstructs the error:

$ cat test2.cpp
class A {
 friend class B;
};

static class B {
public:
   B() { }
} _B;

int main() {
   return 1;
}


stonea@monk ~/ma/rose/friends
$ rosec test2.cpp
Case of locatedNode = 0xb7daf008 = SgMemberFunctionDeclaration parent
= (nil) = Null
Error: NULL parent found
Inside of Sg_File_Info::display(Error: NULL parent found: debug)
    isTransformation         = false
    isCompilerGenerated      = false
    isOutputInCodeGeneration = false
    isShared                 = false
    isFrontendSpecific       = false
    filename = /s/bach/l/under/stonea/ma/rose/friends/test2.cpp
    line     = 7  column = 5
    file_id  = 0
    filename = /s/bach/l/under/stonea/ma/rose/friends/test2.cpp
    line     = 7  column   = 5
function name = B definingDeclaration = 0xb7daf008
nondefiningDeclaration = 0xb7daf008 parent = (nil)
rosec: /s/bach/e/proj/oadev/ROSE/rose-0.8.9a/ROSE-0.8.9a/src/frontend/SageIII/astPostProcessing/resetParentPointers.C:1840:
virtual void ResetParentPointersInMemoryPool::visit(SgNode*):
Assertion `definingDeclaration->get_parent() != __null' failed.
Aborted

stonea@monk ~/ma/rose/friends
$ g++ test2.cpp

rosec is the identity translator, it simply runs things though
frontend() and backend().

Thanks,
-Andy 

*/

class A
   {
     friend class B;
   };

static class B
   {
     public:
          B() { }
   } _B;

int main()
   {
     return 1;
   }
