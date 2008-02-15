#include <rose.h>
#include <iostream>

#include "common.hh"
#include "Domain.hh"
#include "DBFactory.hh"
#include "Relation.hh"



/* 
 * Empty Definitions for linking
 */
#define EMPTY_STRUCTOR(Clazz) Clazz::Clazz() {} Clazz::~Clazz() {}

EMPTY_STRUCTOR(Element);
EMPTY_STRUCTOR(DBFactory);
EMPTY_STRUCTOR(Domain);

Relation::~Relation()
{
    delete[] signature;
    delete[] names;
}

