#ifndef DOXYGEN_GRAMMAR_H
#define DOXYGEN_GRAMMAR_H

#include <string>
#include <list>

class DxNode 
   {

     public:
          DxNode();
          virtual ~DxNode() 
             {
             }
          std::string body;
          virtual std::string commandName() = 0;
          virtual std::string unparse();

   };

class DxText : public DxNode 
   {

     public:
          std::string commandName();

   };

class DxBrief : public DxNode 
   {

     public:
          std::string commandName();

   };

class DxSeeAlso : public DxNode 
   {

     public:
          std::string commandName();

   };

class DxParameter : public DxNode 
   {

     public:
          DxParameter(std::string name);
          std::string name;
          std::string commandName();
          std::string unparse();

   };

class DxReturn : public DxNode 
   {

     public:
          std::string commandName();

   };

class DxDeprecated : public DxNode 
   {

     public:
          std::string commandName();

   };

class DxName : public DxNode 
   {

     public:
          std::string commandName();

   };

class DxDeclarationStatement : public DxNode 
   {
   };

class DxFunctionDeclaration : public DxDeclarationStatement 
   {

     public:
          std::string commandName();

   };

class DxVariableDeclaration : public DxDeclarationStatement 
   {

     public:
          std::string commandName();

   };

class DxClassDeclaration : public DxDeclarationStatement 
   {

     public:
          std::string commandName();

   };

typedef std::list<DxNode *> DxNodeList;

#endif
