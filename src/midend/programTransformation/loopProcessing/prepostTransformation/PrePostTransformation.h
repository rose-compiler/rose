#ifndef PRE_POST_TRANSFORMATION_H
#define PRE_POST_TRANSFORMATION_H

#include <LoopTransformInterface.h>

class PrePostTransformation : public TransformAstTree
{
 protected:
  virtual bool operator()( AstInterface& fa, const AstNodePtr& n,
                           AstNodePtr& result) = 0;
  static bool cmdline_find(const std::vector<std::string>& argv, 
                         unsigned& index, const std::string& opt,
                                std::vector<std::string>* unknown_args) 
    {
       for (; index < argv.size(); ++index)
       {
        if (argv[index] != opt)
        {
            if (unknown_args != 0) unknown_args->push_back(argv[index]);
        }
        else return true;
       }
       return false;
    }
  static void append_args(const std::vector<std::string>& argv,
                      unsigned& index, std::vector<std::string>& unknown_args)
   {
       for (; index < argv.size(); ++index)
            unknown_args.push_back(argv[index]);
   }
 public:
  AstNodePtr operator()(const AstNodePtr& n, AstInterface::TraversalVisitType t)
    { 
       AstInterface& fa = LoopTransformInterface::getAstInterface();
       return TransformAstTraverse( fa, n, *this, t); 
    }
};

#endif
