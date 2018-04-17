/*
The following code doesn't compile with ROSE; the unparsed code is missing a call to the object constructor. It compiles
fine with gcc. I am not sure why it doesn't compile, as the AST graph looks correct. I am including both the original
code and the result produced by the ROSE backend:

defaultConstructorUnparseBug.C
-------------------------------------------------
struct PacketInfo
{
};

void foo()
{
PacketInfo pinfo = PacketInfo();
}
-------------------------------------------------


Result of compiling the above code with ROSE:
-------------------------------------------------
struct PacketInfo
{
}
;

void foo()
{
struct PacketInfo pinfo = ();
}
-------------------------------------------------

Error produced by the backend compiler:

rose_defaultConstructorUnparseBug.C: In function \u2018void foo()\u2019:
rose_defaultConstructorUnparseBug.C:9: error: expected primary-expression before \u2018)\u2019 token 

*/

struct PacketInfo
   {
   };

void foo()
   {
     PacketInfo pinfo = PacketInfo();
   }
