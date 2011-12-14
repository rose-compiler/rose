/*
 * PolyDDV.cpp: This file is part of the PolyOpt project.
 *
 * PolyOpt: a Polyhedral Optimizer for the ROSE compiler
 *
 * Copyright (C) 2011 the Ohio State University
 *
 * This program can be redistributed and/or modified under the terms
 * of the license specified in the LICENSE.txt file at the root of the
 * project.
 *
 * Contact: P. Sadayappan <saday@cse.ohio-state.edu>
 *
 */
/**
 * @file: PolyDDV.cpp
 * @author: Louis-Noel Pouchet <pouchet@cse.ohio-state.edu>
 */

#include <vector>
#include <iostream>
#include <ostream>
#include <polyopt/PolyDDV.hpp>



PolyDDV::PolyDDV(int size)
{
  _dep_type = poly_ddv_undef;
  _types = std::vector<e_poly_ddv_type>(size);
  _values = std::vector<int>(size);
  int i;
  for (i = 0; i < size; ++i)
    {
      _types[i] = poly_ddv_star;
      _values[i] = 0;
    }
  _loop_ptr = NULL;
}

PolyDDV::PolyDDV(int size, void* ptr)
{
  _dep_type = poly_ddv_undef;
  _types = std::vector<e_poly_ddv_type>(size);
  _values = std::vector<int>(size);
  int i;
  for (i = 0; i < size; ++i)
    {
      _types[i] = poly_ddv_star;
      _values[i] = 0;
    }
  _loop_ptr = ptr;
}


PolyDDV::~PolyDDV()
{

}


void PolyDDV::setLoopPtr(void* ptr)
{
  _loop_ptr = ptr;
}

void* PolyDDV::getLoopPtr() const
{
  return _loop_ptr;
}

int PolyDDV::getSize() const
{
  return _types.size();
}


void PolyDDV::setRAW()
{
  _dep_type = poly_ddv_raw;
}

void PolyDDV::setWAR()
{
  _dep_type = poly_ddv_war;
}

void PolyDDV::setWAW()
{
  _dep_type = poly_ddv_waw;
}

void PolyDDV::setRAR()
{
  _dep_type = poly_ddv_rar;
}

bool PolyDDV::isRAW() const
{
  return _dep_type == poly_ddv_raw;
}

bool PolyDDV::isWAR() const
{
  return _dep_type == poly_ddv_war;
}

bool PolyDDV::isWAW() const
{
  return _dep_type == poly_ddv_waw;
}

bool PolyDDV::isRAR() const
{
  return _dep_type == poly_ddv_rar;
}


void PolyDDV::setPlus(int pos)
{
  _types[pos] = poly_ddv_plus;
}

void PolyDDV::setMinus(int pos)
{
  _types[pos] = poly_ddv_minus;
}

void PolyDDV::setStar(int pos)
{
  _types[pos] = poly_ddv_star;
}

void PolyDDV::setEq(int pos)
{
  _types[pos] = poly_ddv_eq;
}

void PolyDDV::setScalar(int pos, int value)
{
  _types[pos] = poly_ddv_scalar;
  _values[pos] = value;
}

bool PolyDDV::isPlus(int pos) const
{
  return _types[pos] == poly_ddv_plus;
}

bool PolyDDV::isMinus(int pos) const
{
  return _types[pos] == poly_ddv_minus;
}

bool PolyDDV::isStar(int pos) const
{
  return _types[pos] == poly_ddv_star;
}

bool PolyDDV::isEq(int pos) const
{
  return _types[pos] == poly_ddv_eq;
}

bool PolyDDV::isScalar(int pos) const
{
  return _types[pos] == poly_ddv_scalar;
}

int PolyDDV::getScalar(int pos) const
{
  return _values[pos];
}

bool PolyDDV::isIdenticalVector(PolyDDV& ddv)
{
  if (ddv.getSize() != _types.size())
    return false;
  int i;
  for (i = 0; i < ddv.getSize(); ++i)
    {
      if (ddv.isPlus(i) && ! isPlus(i))
	return false;
      else if (ddv.isMinus(i) && ! isMinus(i))
	return false;
      else if (ddv.isStar(i) && ! isStar(i))
	return false;
      else if (ddv.isEq(i) && ! isEq(i))
	return false;
      else if (ddv.isScalar(i))
	{
	  if (! isScalar(i) || getScalar(i) != ddv.getScalar(i))
	    return false;
	}
    }

  return true;
}

bool PolyDDV::isPureIdentical(PolyDDV& ddv)
{
  if (! isIdenticalVector(ddv))
    return false;
  if (ddv.isRAW() && ! isRAW())
    return false;
  else if (ddv.isWAR() && ! isWAR())
    return false;
  else if (ddv.isWAW() && ! isWAW())
    return false;
  else if (ddv.isRAR() && ! isRAR())
    return false;

  return true;
}


void PolyDDV::computeHull(PolyDDV& ddv)
{
  if (ddv.getSize() != _types.size())
    return;
  int i;
  for (i = 0; i < ddv.getSize(); ++i)
    {
      if (ddv.isPlus(i))
	{
	  if (isEq(i) || isScalar(i))
	    setPlus(i);
	  else if (isMinus(i))
	    setStar(i);
	}
      else if (ddv.isMinus(i))
	{
	  if (isEq(i) || isScalar(i))
	    setMinus(i);
	  else if (isPlus(i))
	    setStar(i);
	}
      else if (ddv.isStar(i))
	setStar(i);
      else if (ddv.isScalar(i) && ! isStar(i))
	{
	  int s1 = ddv.getScalar(i);
	  if (isScalar(i) || isEq(i))
	    {
	      int s2 = 0;
	      if (isScalar(i))
		s2 = getScalar(i);
	      if (s1 > 0 && s2 < 0 || s1 < 0 && s2 > 0)
		setStar(i);
	      else if (s1 > 0 && s1 != s2)
		setPlus(i);
	      else if (s1 < 0 && s1 != s2)
		setMinus(i);
	    }
	  else
	    {
	      if (s1 > 0 && isMinus(i) || s1 < 0 && isPlus(i))
		setStar(i);
	    }
	}
    }
}



std::ostream& operator<<(std::ostream& out, PolyDDV const& rhs)
{
  int i;
  if (rhs.isRAW())
    out << "RAW: ";
  else if (rhs.isWAR())
    out << "WAR: ";
  else if (rhs.isWAW())
    out << "WAW: ";
  else if (rhs.isRAR())
    out << "RAR: ";
  else
    out << "[undef dep type]: ";
  out << "(";
  for (i = 0; i < rhs.getSize(); ++i)
    {
      if (rhs.isEq(i))
	out << "=";
      else if (rhs.isPlus(i))
	out << "<";
      else if (rhs.isMinus(i))
	out << ">";
      else if (rhs.isStar(i))
	out << "*";
      else if (rhs.isScalar(i))
	out << rhs.getScalar(i);
      if (i < rhs.getSize() - 1)
	out << ",";
    }
  out << ")";

  return out;
}

