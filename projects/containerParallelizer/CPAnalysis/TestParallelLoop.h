#ifndef TEST_PARALLEL_LOOP
#define TEST_PARALLEL_LOOP

#include <AnnotCollect.h>
#include <OperatorAnnotation.h>
#include <set>

class AstNodePtr;

class ContainerCollection : public TypeAnnotCollection <BoolDescriptor>
{
  virtual bool read_annot_name( const string& abname) const
    { return (abname == "container"); }
 public:
  void Dump() const
    { cerr << "containers: \n"; TypeAnnotCollection<BoolDescriptor>::Dump(); }
};

class TestParallelLoop  {
  ContainerCollection containers;
  OperatorModInfoCollection modInfo;
  map <string, OperatorDeclaration> decl;

 public:
  TestParallelLoop( int argc, char** argv); 
  
  bool LoopParallelizable( const AstNodePtr& loop);
  static void DumpOptions( ostream& os);

  bool known_operator( string op, OperatorSideEffectDescriptor& info) 
    { return modInfo.known_type(op, &info); }
  bool known_operator( string op) 
         { return modInfo.known_type(op); }
  void Dump() const 
     { 
        containers.Dump(); 
        modInfo.Dump(); 
      }
};

#endif
