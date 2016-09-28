/* Liao, 5/15/2009
 * A test code extracted from perlbench of spec cpu 2006
 *
 * __GNUG__ should not be defined for C_only mode.
 *
 * */
/* XXX Configure ought to have a test for a boolean type, if I can
   just figure out all the headers such a test needs.
   Andy Dougherty       August 1996
*/
/* bool is built-in for g++-2.6.3 and later, which might be used
   for extensions.  <_G_config.h> defines _G_HAVE_BOOL, but we can't
   be sure _G_config.h will be included before this file.  _G_config.h
   also defines _G_HAVE_BOOL for both gcc and g++, but only g++
   actually has bool.  Hence, _G_HAVE_BOOL is pretty useless for us.
   g++ can be identified by __GNUG__.
   Andy Dougherty       February 2000
*/
#ifdef __GNUG__         /* GNU g++ has bool built-in */
#  ifndef HAS_BOOL
#    define HAS_BOOL 1
#  endif
#endif

#ifndef HAS_BOOL
#  define bool char
# define HAS_BOOL 1
#endif

union any {
  void*       any_ptr;
  bool        any_bool;
};


