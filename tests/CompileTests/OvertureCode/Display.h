/*  -*-Mode: c++; -*-  */
#ifndef DISPLAY
#define DISPLAY
#include <assert.h>
#include <CompositeGrid.h>
#include <CompositeGridFunction.h>
#include <Oges.h>
#include <NameList.h>
#include <Interpolant.h>
#include <OGgetIndex.h>
#include "davidsReal.h"
#include "axisDefs.h"
#include "loops.h"

class Display
	//========================================
	// Author:		D.L.Brown
	// Date Created:	941201
	// Date Modified:	950420
	//
	// Purpose:
	//  This is an alternate Display class for A++
	//  and CompositeGridFunction class members.
	//  Still under construction. Nicer formatting,
	//  and an interactive option, along with the possibility
	//  to shut off display output either entirely, or 
	//  selectively. 
	//
	// Interface: (inputs)
	//   The variables allDisplayOff and thisDisplayOff are controlled
	//   interactively according to the menu. The menu choice
	//   "o[ff]" will set the global static variable allDisplayOff
	//   to TRUE, and halt output from all instances of the Display
	//   class. The menu choice "t[hisoff]" will only halt output
	//   from the current instance of the Display class. Thus to
	//   selectively control output from, say, different routines,
	//   separate instantiations of the display class should be used.
	//   A recommended approach is to use one instantiation in each
	//   routine in your code.
	//
	//   Alternatively, the interactive feature can be eliminated altogether
	//   by calling setInteractiveDisplay (FALSE) at some point in the code.
	//   The default value if interactiveDisplay is TRUE.
	//
	// Interface: (output)
	//
	// Status and Warnings:
	//  There are no known bugs, nor will there ever be.
	// 
	//========================================
{
  protected: 
  bool thisDisplayOff;
  bool interactiveInitializationCalled;

  public:
  static bool allDisplayOff;
  static bool interactiveDisplay;
  static bool cellCenteredDisplayOption;

  Display ();
  ~Display ();

  void setInteractiveDisplay (bool & trueOrFalse){interactiveDisplay = trueOrFalse;}

  void interactivelySetInteractiveDisplay ();
  void interactivelySetInteractiveDisplay (const aString &label);

  void display (const floatGridCollectionFunction &cgf, 	const aString &label);
  void display (const doubleGridCollectionFunction &cgf, 	const aString &label);

  void display (const intGridCollectionFunction &cgf,  	const aString &label);

  void display (const floatMappedGridFunction &mgf,    	const aString &label);
  void display (const doubleMappedGridFunction &mgf,    	const aString &label);

  void display (const intMappedGridFunction &mgf,		const aString &label);

  void display (const floatArray &x,				const aString &label);
  void display (const doubleArray &x,				const aString &label);

  void display (const intArray &i,				const aString &label);

  void display (const Index &I,				const aString &label);

  void display (const floatMappedGridFunction &mgf);
  void display (const doubleMappedGridFunction &mgf);
  void display (const intMappedGridFunction &mgf);
  void display (const floatArray &x);
  void display (const doubleArray &x);
  void display (const intArray &i);
  void display (const Index &I);
};
#endif
