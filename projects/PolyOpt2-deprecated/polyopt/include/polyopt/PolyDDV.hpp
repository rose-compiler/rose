/*
 * PolyDDV.hpp: This file is part of the PolyOpt project.
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
 * @file: PolyDDV.hpp
 * @author: Louis-Noel Pouchet <pouchet@cse.ohio-state.edu>
 */

#ifndef PLUTO_ROSE_POLYDDV_HPP
# define PLUTO_ROSE_POLYDDV_HPP

#include <vector>
#include <iostream>
#include <ostream>


// Internal enums for the DDV. This is not exposed to the user.
enum poly_ddv_type
{
  poly_ddv_scalar,
  poly_ddv_plus,
  poly_ddv_minus,
  poly_ddv_star,
  poly_ddv_eq
};
typedef enum poly_ddv_type e_poly_ddv_type;

enum poly_ddv_dependence_type
{
  poly_ddv_undef,// Dependence type associated to the DDV is unspecified.
  poly_ddv_raw,	 // Dependence type is Read-After-Write (flow)
  poly_ddv_war,  // Dependence type is Write-After-Read (anti)
  poly_ddv_waw,  // Dependence type is Write-After-Write (output)
  // Optional info:
  poly_ddv_rar   // Dependence type is Read-After-Read (read): not necessary
		 // for semantics preservation.
};
typedef enum poly_ddv_dependence_type e_poly_ddv_dependence_type;


/**
 * PolyDDV class: model a Dependence Distance Vector (DDV) attached to
 * a given loop.
 *
 * A DDV has 'size' components, meaning the loop is surrounded by
 * 'size - 1' loops
 *
 * Each component can be:
 *  '=' (Eq)
 *  '<' (Plus)
 *  '>' (Minus)
 *  '*' (Star)
 *  'x' (Scalar: x)
 *
 * for instance, in the code:
 *
 * for (i = 0; i < N; ++i)
 *  for (j = 0; j < M; ++j)
 *    A[i] = A[i];
 *
 * the ddv is (=,<)
 *
 *
 * There is a getter and a setter for the type of each component, for
 * the pointer to the loop to which the DDV is attached, and for the
 * type of the dependence (RAW, WAW, WAR, RAR).
 *
 * The PolyDDV class is printable in a stream (using operator <<).
 *
 */
class PolyDDV
{
public:
  // Must give a size upon creation.
  PolyDDV(int size);
  PolyDDV(int size, void* loop_ptr);
  ~PolyDDV();

  // Copy constructor.
  PolyDDV(PolyDDV const& other) :
    _dep_type(other._dep_type),
    _values(other._values),
    _types(other._types),
    _loop_ptr(other._loop_ptr) { };

  // Getter/setter for the loop pointer associated to the DDV.
  void setLoopPtr(void* ptr);
  void* getLoopPtr() const;
  int getSize() const;

  // Getter/setter for the dependence type.
  void setRAW();
  void setWAR();
  void setWAW();
  void setRAR();

  bool isRAW() const;
  bool isWAR() const;
  bool isWAW() const;
  bool isRAR() const;

  // Getter/setter for the components of the DDV.
  void setPlus(int pos);
  void setMinus(int pos);
  void setStar(int pos);
  void setEq(int pos);
  void setScalar(int pos, int value);

  bool isPlus(int pos) const;
  bool isMinus(int pos) const;
  bool isStar(int pos) const;
  bool isEq(int pos) const;
  bool isScalar(int pos) const;
  int getScalar(int pos) const;

  // Checking the equality of two DDVs
  // Case 1: only look at identical components in the vector.
  bool isIdenticalVector(PolyDDV& ddv);
  // Case 2: look at identical components in the vector AND same
  // dependence type.
  bool isPureIdentical(PolyDDV& ddv);

  // In-place computation of the hull of two ddvs. Current ddv is
  // hulled with 'ddv'.
  void computeHull(PolyDDV& ddv);


private:
  e_poly_ddv_dependence_type	_dep_type;
  std::vector<e_poly_ddv_type>	_types;
  std::vector<int>		_values;
  void*				_loop_ptr;
};

std::ostream& operator<<(std::ostream& out, PolyDDV const& rhs);


#endif //PLUTO_ROSE_POLYDDV_HPP
