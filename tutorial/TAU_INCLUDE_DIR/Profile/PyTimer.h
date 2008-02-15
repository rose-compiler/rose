// -*- C++ -*-
//
//-----------------------------------------------------------------------------
//
//                       TAU Development Team
//                       University of Oregon, Los Alamos National Laboratory,
//                       FZJ Germany
//                       (C) 2003 All Rights Reserved
//
// <LicenseText>
//
//-----------------------------------------------------------------------------
//
// $Log: PyTimer.h,v $
// Revision 1.1  2006/04/24 00:22:01  dquinlan
// See ChangeLog
//
// Revision 1.1  2003/02/28 23:29:09  sameer
// Added Python Bindings  headers to TAU.
//
// 

#if !defined(pytau_timer_h)
#define pytau_timer_h


extern char pytau_profileTimer__name__[];
extern char pytau_profileTimer__doc__[];
extern "C"
PyObject * pytau_profileTimer(PyObject *, PyObject *);

extern char pytau_start__name__[];
extern char pytau_start__doc__[];
extern "C"
PyObject * pytau_start(PyObject *, PyObject *);

extern char pytau_stop__name__[];
extern char pytau_stop__doc__[];
extern "C"
PyObject * pytau_stop(PyObject *, PyObject *);

#endif // pytau_timer_h

// version
// $Id: PyTimer.h,v 1.1 2006/04/24 00:22:01 dquinlan Exp $

// End of file
