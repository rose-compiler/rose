#include <commentObj.h>
#include <doxygenComment.h>
#include <sstream>
#include <iostream>
#include <stdlib.h>
#include <cstdio>

using namespace std;

DoxygenEntry::DoxygenEntry(string comm) :
        _type(None),
        nodes(parseDoxygenComment(comm)),
        prototypeP(0),
        briefP(0),
        descriptionP(0),
        returnsP(0),
        deprecatedP(0),
        nameP(0) 
   {
     extractParts();
   }

DoxygenEntry::DoxygenEntry(DoxygenEntry::EntryType type) :
        _type(type),
        nodes(new DxNodeList),
        prototypeP(0),
        briefP(0),
        descriptionP(0),
        returnsP(0),
        deprecatedP(0),
        nameP(0),
        afterLastParam(nodes->end())
   {
   }


void
DoxygenEntry::extractParts() 
   {
     unsigned int largestText = 0;
     DxNodeList::iterator lastParam = nodes->end();

     for (DxNodeList::iterator i = nodes->begin(); i != nodes->end(); ++i) 
        {
          DxNode *n = *i;

          DxClassDeclaration *cd = dynamic_cast<DxClassDeclaration *>(n);
          if (cd) 
             {
               _type = Class;
               prototypeP = &(cd->body);
             }

          DxFunctionDeclaration *fd = dynamic_cast<DxFunctionDeclaration *>(n);
          if (fd) 
             {
               _type = Function;
               prototypeP = &(fd->body);
             }

          DxVariableDeclaration *vd = dynamic_cast<DxVariableDeclaration *>(n);
          if (vd) 
             {
               _type = Variable;
               prototypeP = &(vd->body);
             }

          DxBrief *b = dynamic_cast<DxBrief *>(n);
          if (b) 
             {
               briefP = &(b->body);
             }

          DxText *t = dynamic_cast<DxText *>(n);
          if (t) 
             {
            /* A crude way of finding the text segment corresponding to the
             * description.
             */
               if (t->body.size() > largestText) 
                  {
                    descriptionP = &(t->body);
                    largestText = t->body.size();
                  }
             }

          DxParameter *p = dynamic_cast<DxParameter *>(n);
          if (p) 
             {
               paramMap[p->name] = &(p->body);
               lastParam = i;
               ++lastParam;
            // printf("located param %s\n", p->name.c_str());
             }

          DxReturn *r = dynamic_cast<DxReturn *>(n);
          if (r) 
             {
               if (lastParam == nodes->end()) 
                  {
                    lastParam = i;
                  }
               returnsP = &(r->body);
             }

          DxDeprecated *d = dynamic_cast<DxDeprecated *>(n);
          if (d) 
             {
               deprecatedP = &(d->body);
             }

          DxName *na = dynamic_cast<DxName *>(n);
          if (na) 
             {
               nameP = &(na->body);
             }
        }

     afterLastParam = lastParam;
   }

void
DoxygenEntry::addParameter(string name) 
   {
     DxParameter *newParam = new DxParameter(name);
     nodes->insert(afterLastParam, newParam);
     paramMap[name] = &(newParam->body);
   }

/*
 * note: changing the EntryType, does not change the type of an existing entry, only the
 * type of an entry should the prototype attribute be set
 */
DoxygenEntry::EntryType &DoxygenEntry::type() 
   {
     return _type;
   }

bool
DoxygenEntry::hasPrototype() 
   {
     return prototypeP;
   }

string &
DoxygenEntry::prototype() 
   {
     if (!prototypeP) 
        {
          DxDeclarationStatement *d;
          switch (_type) 
             {
               case Class:    d = new DxClassDeclaration;    break;
               case Function: d = new DxFunctionDeclaration; break;
               case Variable: d = new DxVariableDeclaration; break;
               default: puts("invalid type"); abort();
             }
          nodes->insert(nodes->begin(), d);

          prototypeP = &(d->body);
        }
     return *prototypeP;
   }

bool
DoxygenEntry::hasBrief() 
   {
     return briefP;
   }

string &
DoxygenEntry::brief() 
   {
     if (!briefP) 
        {
          DxBrief *newBrief = new DxBrief();
          DxNodeList::iterator i = nodes->begin();
          if (dynamic_cast<DxDeclarationStatement *>(*i)) 
             {
               ++i;
             }
          nodes->insert(i, newBrief);
          briefP = &(newBrief->body);
        }
     return *briefP;
   }


void
DoxygenEntry::set_brief(std::string newString) 
   {
	 if(briefP == NULL)
	   briefP = new std::string();

         (*briefP) = "\\brief " + newString;

   }



bool
DoxygenEntry::hasDescription() 
   {
     return descriptionP;
   }


void
DoxygenEntry::set_description(std::string newString) 
   {
	 if(descriptionP == NULL)
	   descriptionP = new std::string();

         (*descriptionP) = "\\description " + newString;
   }


string &
DoxygenEntry::description() 
   {
     if (!descriptionP) 
        {
          DxText *newDesc = new DxText();
          nodes->push_back(newDesc);
          descriptionP = &(newDesc->body);
        }
     return *descriptionP;
   }

bool
DoxygenEntry::hasName() 
   {
     return descriptionP;
   }

string &
DoxygenEntry::name() 
   {
     if (!nameP) 
        {
          DxName *newName = new DxName();
          nodes->push_front(newName);
          nameP = &(newName->body);
        }
     return *nameP;
   }

bool
DoxygenEntry::hasReturns() 
   {
     return returnsP;
   }

string &
DoxygenEntry::returns() 
   {
     if (!returnsP) 
        {
          DxReturn *newRet = new DxReturn();
          nodes->push_back(newRet);
          returnsP = &(newRet->body);
        }
     return *returnsP;
   }

bool
DoxygenEntry::hasDeprecated() 
   {
     return deprecatedP;
   }

string &
DoxygenEntry::deprecated() 
   {
     if (!deprecatedP) 
        {
          DxDeprecated *newDep = new DxDeprecated();
          nodes->push_back(newDep);
          deprecatedP = &(newDep->body);
        }
     return *deprecatedP;
   }

bool
DoxygenEntry::hasParameter(string name) 
   {
     return paramMap.count(name);
   }

string &
DoxygenEntry::parameter(string name) 
   {
     if (!paramMap.count(name))
        {
          addParameter(name);
          // malloc(100);
        }
     return *(paramMap[name]);
   }

string
DoxygenEntry::unparse() 
   {
     stringstream ss;
     ss << "/*!";
     for (DxNodeList::iterator i = nodes->begin(); i != nodes->end(); ++i) 
        {
          ss << (*i)->unparse();
        }
     ss << "*/";
     return ss.str();
   }

bool
DoxygenEntry::isDoxygenEntry(string comm) 
   {
     return comm[0] == '/' && (comm[1] == '*' || comm[1] == '/') && comm[2] == '!';
   }

/*! Rather naive assumption that name is betwen the first ( and the preceding non-identifier character
  Will not work for funtions that return a function pointer, at least.
  To be called on unmangled name */
string
DoxygenEntry::getProtoName(string name) 
   {
     string::size_type end = name.find('(');
     if (end == string::npos) end = name.size();
     while (name[end-1] == ' ') end--;
     string::size_type begin = end;
     while (begin > 0 &&
           ((name[begin-1] >= 'A' && name[begin-1] <= 'Z')
         || (name[begin-1] >= 'a' && name[begin-1] <= 'z')
         || (name[begin-1] >= '0' && name[begin-1] <= '9')
         || (name[begin-1] == '_')
         || (name[begin-1] == ':'))) begin--;
     return string(name, begin, end-begin);
   }
