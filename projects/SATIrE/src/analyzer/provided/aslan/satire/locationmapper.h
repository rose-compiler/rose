#ifndef LOCATIONMAPPER_H
#define LOCATIONMAPPER_H

#include <satire_rose.h>
#include <iostream>
#include <vector>
#include <map>
#include<utility>

// a local position is line and column number
typedef long LineNumber;
typedef long ColumnNumber;
typedef std::pair<LineNumber,ColumnNumber> LocalPosition;

// a local site is a local position and a pointer to corresponding AST node
typedef std::pair<LocalPosition,SgNode*> LocalSite;
typedef std::map<std::string,std::vector<LocalSite>* > AbstractSites;



bool operator<(LocalPosition lhs,LocalPosition rhs);
bool operator<(LocalSite lhs,LocalSite rhs);

#endif
