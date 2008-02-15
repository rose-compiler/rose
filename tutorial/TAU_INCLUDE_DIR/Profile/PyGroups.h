// -*- C++ -*-
//
//-----------------------------------------------------------------------------
//
//                       VTF Development Team
//                       California Institute of Technology
//                       (C) 2002 All Rights Reserved
//
// <LicenseText>
//
//-----------------------------------------------------------------------------
//
// $Log: PyGroups.h,v $
// Revision 1.1  2006/04/24 00:22:01  dquinlan
// See ChangeLog
//
// Revision 1.1  2003/02/28 23:29:09  sameer
// Added Python Bindings  headers to TAU.
//
// Revision 1.2  2002/01/23 02:47:38  cummings
// Added Python wrappers for new Tau functions enableAllGroups() and
// disableAllGroups(), which will enable or disable profiling for all
// existing profile groups with one function call.  The only exception
// is the group TAU_DEFAULT, which includes main() and cannot be disabled.
//
// Revision 1.1  2002/01/16 02:05:07  cummings
// Original source and build procedure files for Python bindings of
// TAU runtime API.  These bindings allow you to do some rudimentary
// things from the Python script, such as enable/disable all Tau
// instrumentation, enable/disable a particular Tau profile group,
// and dump or purge the current Tau statistics.  Still to come are
// bindings for creating and using Tau global timers and user events.
//
// 

#if !defined(pytau_groups_h)
#define pytau_groups_h


extern char pytau_getProfileGroup__name__[];
extern char pytau_getProfileGroup__doc__[];
extern "C"
PyObject * pytau_getProfileGroup(PyObject *, PyObject *);

extern char pytau_enableGroup__name__[];
extern char pytau_enableGroup__doc__[];
extern "C"
PyObject * pytau_enableGroup(PyObject *, PyObject *);

extern char pytau_disableGroup__name__[];
extern char pytau_disableGroup__doc__[];
extern "C"
PyObject * pytau_disableGroup(PyObject *, PyObject *);

extern char pytau_enableGroupName__name__[];
extern char pytau_enableGroupName__doc__[];
extern "C"
PyObject * pytau_enableGroupName(PyObject *, PyObject *);

extern char pytau_disableGroupName__name__[];
extern char pytau_disableGroupName__doc__[];
extern "C"
PyObject * pytau_disableGroupName(PyObject *, PyObject *);

extern char pytau_enableAllGroups__name__[];
extern char pytau_enableAllGroups__doc__[];
extern "C"
PyObject * pytau_enableAllGroups(PyObject *, PyObject *);

extern char pytau_disableAllGroups__name__[];
extern char pytau_disableAllGroups__doc__[];
extern "C"
PyObject * pytau_disableAllGroups(PyObject *, PyObject *);

extern char pytau_enableInstrumentation__name__[];
extern char pytau_enableInstrumentation__doc__[];
extern "C"
PyObject * pytau_enableInstrumentation(PyObject *, PyObject *);

extern char pytau_disableInstrumentation__name__[];
extern char pytau_disableInstrumentation__doc__[];
extern "C"
PyObject * pytau_disableInstrumentation(PyObject *, PyObject *);

#endif // pytau_groups_h

// version
// $Id: PyGroups.h,v 1.1 2006/04/24 00:22:01 dquinlan Exp $

// End of file
