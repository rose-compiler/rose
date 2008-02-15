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
// $Log: PyBindings.h,v $
// Revision 1.1  2006/04/24 00:22:00  dquinlan
// See ChangeLog
//
// Revision 1.1  2003/02/28 23:29:08  sameer
// Added Python Bindings  headers to TAU.
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

#if !defined(pytau_bindings_h)
#define pytau_bindings_h

// the method table

extern struct PyMethodDef pytau_methods[];

#endif // pytau_bindings_h

// version
// $Id: PyBindings.h,v 1.1 2006/04/24 00:22:00 dquinlan Exp $

// End of file
