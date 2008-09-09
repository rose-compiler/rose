/*
 * upcr_proxy.h:  definitions/macros needed by the UPC compiler
 *
 * This file contains any definitions and/or macros which are not in the UPC
 * Runtime specification, but are needed to compile the C code emitted by our
 * sgiupc compiler.  
 *
 * $Id: upcr_proxy.h,v 1.29 2006/03/21 08:35:21 bonachea Exp $
 */

#ifndef UPCR_PROXY_H
#define UPCR_PROXY_H

/*#define RET_handle_t       _INT64 */
#define RET_pshared_ptr_t(call)	(call)
#define ARG_pshared_ptr_t(arg)  (arg)
#define RET_shared_ptr_t(call)  (call)
#define ARG_shared_ptr_t(arg)	(arg)
#define RET_handle_t(call)	(call)
#define ARG_handle_t(arg)	(*((upcr_handle_t*) &(arg)))

#define RET_val_handle_t(call)  (call)
#define ARG_val_handle_t(arg)	(*(upcr_valget_handle_t*)&(arg))

/* #define ARG_handle_t       upcr_handle_t */

#define RET_reg_value_t(call)   ((upcr_register_value_t)(call))
#define ARG_reg_value_t(arg)	((upcr_register_value_t)(arg))

/* memory allocation routines */

#define UPCR_ALLOC(x) \
             RET_shared_ptr_t(upcr_local_alloc(1, (x)))   

#define UPCR_LOCAL_ALLOC(x,y) \
             RET_shared_ptr_t(upcr_local_alloc((x), (y)))


#define UPCR_GLOBAL_ALLOC(x, y) \
             RET_shared_ptr_t(upcr_global_alloc((x), (y)))

#define UPCR_ALL_ALLOC(x, y) \
             upcr_all_alloc((x), (y))

#define UPCR_FREE(x)\
              upcr_free(ARG_shared_ptr_t(x))


/* locks */ 

#define UPCR_GLOBAL_LOCK_ALLOC() \
              RET_shared_ptr_t(upcr_global_lock_alloc())


#define UPCR_ALL_LOCK_ALLOC() \
              RET_shared_ptr_t(upcr_all_lock_alloc())

#define UPCR_LOCK(x) \
               upcr_lock(ARG_shared_ptr_t(x))

#define UPCR_LOCK_ATTEMPT(x) \
               upcr_lock_attempt(ARG_shared_ptr_t(x))

#define UPCR_UNLOCK(x) \
               upcr_unlock(ARG_shared_ptr_t(x))

#define UPCR_LOCK_FREE(x) \
               upcr_lock_free(ARG_shared_ptr_t(x))


/* relaxed memory get/put */

#define UPCR_PUT_SHARED(x,y,z,w) \
              upcr_put_shared(ARG_shared_ptr_t(x), (y), (z), (w))

#define UPCR_PUT_PSHARED(x,y,z,w) \
              upcr_put_pshared(ARG_pshared_ptr_t(x), (y), (z), (w))

#define UPCR_PUT_NB_SHARED(x,y,z,w) \
              RET_handle_t(upcr_put_nb_shared(ARG_shared_ptr_t(x), (y), (z), (w)))

#define UPCR_PUT_NBI_SHARED(x,y,z,w) \
              upcr_put_nbi_shared(ARG_shared_ptr_t(x), (y), (z), (w))

#define UPCR_PUT_NB_PSHARED(x,y,z,w) \
              RET_handle_t(upcr_put_nb_pshared(ARG_pshared_ptr_t(x), (y), (z), (w)))

#define UPCR_PUT_NBI_PSHARED(x,y,z,w) \
              upcr_put_nbi_pshared(ARG_pshared_ptr_t(x), (y), (z), (w))

#define UPCR_GET_SHARED(x,y,z,w) \
              upcr_get_shared((x), ARG_shared_ptr_t(y), (z), (w))

#define UPCR_GET_NB_SHARED(x,y,z,w) \
              RET_handle_t(upcr_get_nb_shared((x), ARG_shared_ptr_t(y), (z), (w)))

#define UPCR_GET_NBI_SHARED(x,y,z,w) \
              upcr_get_nbi_shared((x), ARG_shared_ptr_t(y), (z), (w))

#define UPCR_GET_PSHARED(x,y,z,w) \
              upcr_get_pshared((x), ARG_pshared_ptr_t(y), (z), (w))

#define UPCR_GET_NB_PSHARED(x,y,z,w) \
              RET_handle_t(upcr_get_nb_pshared((x), ARG_pshared_ptr_t(y), (z), (w)))

#define UPCR_GET_NBI_PSHARED(x,y,z,w) \
              upcr_get_nbi_pshared((x), ARG_pshared_ptr_t(y), (z), (w))

/* strict memory get/put */

#define UPCR_PUT_SHARED_STRICT(x,y,z,w) \
              upcr_put_shared_strict(ARG_shared_ptr_t(x), (y), (z), (w))

#define UPCR_PUT_PSHARED_STRICT(x,y,z,w) \
              upcr_put_pshared_strict(ARG_pshared_ptr_t(x), (y), (z), (w))

#define UPCR_PUT_NB_SHARED_STRICT(x,y,z,w) \
              RET_handle_t(upcr_put_nb_shared_strict(ARG_shared_ptr_t(x), (y), (z), (w)))

#define UPCR_PUT_NB_PSHARED_STRICT(x,y,z,w) \
              RET_handle_t(upcr_put_nb_pshared_strict(ARG_pshared_ptr_t(x), (y), (z), (w)))

#define UPCR_GET_SHARED_STRICT(x,y,z,w) \
              upcr_get_shared_strict((x), ARG_shared_ptr_t(y), (z), (w))

#define UPCR_GET_NB_SHARED_STRICT(x,y,z,w) \
              RET_handle_t(upcr_get_nb_shared_strict((x), ARG_shared_ptr_t(y), (z), (w)))

#define UPCR_GET_PSHARED_STRICT(x,y,z,w) \
              upcr_get_pshared_strict((x), ARG_pshared_ptr_t(y), (z), (w))

#define UPCR_GET_NB_PSHARED_STRICT(x,y,z,w) \
              RET_handle_t(upcr_get_nb_pshared_strict((x), ARG_pshared_ptr_t(y), (z), (w)))

/* synchronization */

#define UPCR_WAIT_SYNCNB(x) \
               upcr_wait_syncnb(ARG_handle_t(x))

#define UPCR_WAIT_SYNCNB_STRICT(x) \
               upcr_wait_syncnb_strict(ARG_handle_t(x))


#define UPCR_TRY_SYNCNB(x) \
               upcr_try_syncnb(ARG_handle_t(x))

#define UPCR_TRY_SYNCNB_STRICT(x) \
               upcr_try_syncnb_strict(ARG_handle_t(x))


/* relaxed register get/put */

#define UPCR_PUT_SHARED_VAL(x,y,z,w) \
           upcr_put_shared_val(ARG_shared_ptr_t(x), (y), ARG_reg_value_t(z), (w))   

#define UPCR_PUT_NB_SHARED_VAL(x,y,z,w) \
           RET_handle_t(upcr_put_nb_shared_val(ARG_shared_ptr_t(x),(y), ARG_reg_value_t(z), (w)))

#define UPCR_PUT_PSHARED_VAL(x,y,z,w) \
           upcr_put_pshared_val(ARG_pshared_ptr_t(x), (y), ARG_reg_value_t(z), (w)) 

#define UPCR_PUT_NB_PSHARED_VAL(x,y,z,w) \
           RET_handle_t(upcr_put_nb_pshared_val(ARG_pshared_ptr_t(x), (y), ARG_reg_value_t(z), (w)))


#define UPCR_GET_SHARED_VAL(x,y,z) \
           RET_reg_value_t(upcr_get_shared_val(ARG_shared_ptr_t(x), (y), (z))) 

#define UPCR_GET_NB_SHARED_VAL(x,y,z) \
           RET_val_handle_t(upcr_get_nb_shared_val(ARG_shared_ptr_t(x), (y), (z))) 

#define UPCR_GET_PSHARED_VAL(x,y,z) \
           RET_reg_value_t(upcr_get_pshared_val(ARG_pshared_ptr_t(x), (y), (z))) 

#define UPCR_GET_NB_PSHARED_VAL(x,y,z) \
           RET_val_handle_t(upcr_get_nb_pshared_val(ARG_pshared_ptr_t(x), (y), (z))) 


#define UPCR_WAIT_SYNCNB_VALGET(x) \
           RET_reg_value_t(upcr_wait_syncnb_valget(ARG_val_handle_t(x)))


/* strict register get/put */

#define UPCR_PUT_SHARED_VAL_STRICT(x,y,z,w) \
           upcr_put_shared_val_strict(ARG_shared_ptr_t(x), (y), ARG_reg_value_t(z), (w))   

#define UPCR_PUT_NB_SHARED_VAL_STRICT(x,y,z,w) \
           RET_handle_t(upcr_put_nb_shared_val_strict(ARG_shared_ptr_t(x),(y), ARG_reg_value_t(z), (w)))

#define UPCR_PUT_PSHARED_VAL_STRICT(x,y,z,w) \
           upcr_put_pshared_val_strict(ARG_pshared_ptr_t(x), (y), ARG_reg_value_t(z), (w)) 

#define UPCR_PUT_NB_PSHARED_VAL_STRICT(x,y,z,w) \
           RET_handle_t(upcr_put_nb_pshared_val_strict(ARG_pshared_ptr_t(x), (y), ARG_reg_value_t(z), (w)))


#define UPCR_GET_SHARED_VAL_STRICT(x,y,z) \
           RET_reg_value_t(upcr_get_shared_val_strict(ARG_shared_ptr_t(x), (y), (z))) 

#define UPCR_GET_PSHARED_VAL_STRICT(x,y,z) \
           RET_reg_value_t(upcr_get_pshared_val_strict(ARG_pshared_ptr_t(x), (y), (z))) 


/* relaxed float put/get */

#define UPCR_PUT_SHARED_FLOATVAL(x,y,z) \
           upcr_put_shared_floatval(ARG_shared_ptr_t(x), (y), (z)) 

#define UPCR_PUT_PSHARED_FLOATVAL(x,y,z) \
           upcr_put_pshared_floatval(ARG_pshared_ptr_t(x), (y), (z))

#define UPCR_PUT_SHARED_DOUBLEVAL(x,y,z) \
           upcr_put_shared_doubleval(ARG_shared_ptr_t(x), (y), (z))

#define UPCR_PUT_PSHARED_DOUBLEVAL(x,y,z) \
           upcr_put_pshared_doubleval(ARG_pshared_ptr_t(x), (y), (z))


#define UPCR_GET_SHARED_FLOATVAL(x,y) \
           upcr_get_shared_floatval(ARG_shared_ptr_t(x), (y)) 

#define UPCR_GET_PSHARED_FLOATVAL(x,y) \
           upcr_get_pshared_floatval(ARG_pshared_ptr_t(x), (y)) 

#define UPCR_GET_SHARED_DOUBLEVAL(x,y) \
           upcr_get_shared_doubleval(ARG_shared_ptr_t(x), (y))

#define UPCR_GET_PSHARED_DOUBLEVAL(x,y) \
           upcr_get_pshared_doubleval(ARG_pshared_ptr_t(x), (y))


/* strict float put/get */

#define UPCR_PUT_SHARED_FLOATVAL_STRICT(x,y,z) \
           upcr_put_shared_floatval_strict(ARG_shared_ptr_t(x), (y), (z)) 

#define UPCR_PUT_PSHARED_FLOATVAL_STRICT(x,y,z) \
           upcr_put_pshared_floatval_strict(ARG_pshared_ptr_t(x), (y), (z))

#define UPCR_PUT_SHARED_DOUBLEVAL_STRICT(x,y,z) \
           upcr_put_shared_doubleval_strict(ARG_shared_ptr_t(x), (y), (z))

#define UPCR_PUT_PSHARED_DOUBLEVAL_STRICT(x,y,z) \
           upcr_put_pshared_doubleval_strict(ARG_pshared_ptr_t(x), (y), (z))


#define UPCR_GET_SHARED_FLOATVAL_STRICT(x,y) \
           upcr_get_shared_floatval_strict(ARG_shared_ptr_t(x), (y)) 

#define UPCR_GET_PSHARED_FLOATVAL_STRICT(x,y) \
           upcr_get_pshared_floatval_strict(ARG_pshared_ptr_t(x), (y)) 

#define UPCR_GET_SHARED_DOUBLEVAL_STRICT(x,y) \
           upcr_get_shared_doubleval_strict(ARG_shared_ptr_t(x), (y))

#define UPCR_GET_PSHARED_DOUBLEVAL_STRICT(x,y) \
           upcr_get_pshared_doubleval_strict(ARG_pshared_ptr_t(x), (y))


/* bulk memory operations */

#define UPCR_MEMGET(x,y,z) \
            upcr_memget((x), ARG_shared_ptr_t(y), (z))

  /* (void *) cast removes const qualifier on src argument to prevent warnings */
#define UPCR_MEMPUT(x,y,z) \
           upcr_memput(ARG_shared_ptr_t(x), (void *)(y), (z)) 

#define UPCR_MEMSET(x,y,z) \
	upcr_memset(ARG_shared_ptr_t(x), (y), (z)) 			     
 


#define UPCR_NB_MEMGET(x,y,z) \
            RET_handle_t(upcr_nb_memget((x), ARG_shared_ptr_t(y), (z)))

  /* (void *) cast removes const qualifier on src argument to prevent warnings */
#define UPCR_NB_MEMPUT(x,y,z) \
           RET_handle_t(upcr_nb_memput(ARG_shared_ptr_t(x), (void *)(y), (z)))

#define UPCR_NB_MEMSET(x,y,z) \
	RET_handle_t(upcr_nb_memset(ARG_shared_ptr_t(x), (y), (z)))


#define UPCR_MEMCPY(x,y,z) \
         upcr_memcpy(ARG_shared_ptr_t(x), ARG_shared_ptr_t(y), (z))


#define UPCR_NB_MEMCPY(x,y,z) \
         upcr_nb_memcpy(ARG_shared_ptr_t(x), ARG_shared_ptr_t(y), (z))




/* shared pointer ops */

#define UPCR_ISVALID_SHARED(x) \
          upcr_isvalid_shared(ARG_shared_ptr_t(x))

#define UPCR_ISVALID_PSHARED(x) \
          upcr_isvalid_pshared(ARG_pshared_ptr_t(x))

#define UPCR_ISNULL_SHARED(x) \
          upcr_isnull_shared(ARG_shared_ptr_t(x))

#define UPCR_ISNULL_PSHARED(x) \
          upcr_isnull_pshared(ARG_pshared_ptr_t(x))

#define UPCR_SHARED_TO_LOCAL(x) \
          upcr_shared_to_local(ARG_shared_ptr_t(x))

#define UPCR_PSHARED_TO_LOCAL(x) \
          upcr_pshared_to_local(ARG_pshared_ptr_t(x))

#define UPCR_SHARED_TO_PSHARED(x) \
          RET_pshared_ptr_t(upcr_shared_to_pshared(ARG_shared_ptr_t(x)))

#define UPCR_PSHARED_TO_SHARED(x) \
          RET_shared_ptr_t(upcr_pshared_to_shared(ARG_pshared_ptr_t(x)))


#define UPCR_SHARED_RESETPHASE(x) \
         RET_shared_ptr_t(upcr_shared_resetphase(ARG_shared_ptr_t(x)))

#define UPCR_THREADOF_SHARED(x) \
          upcr_threadof_shared(ARG_shared_ptr_t(x))

#define UPCR_THREADOF_PSHARED(x) \
          upcr_threadof_pshared(ARG_pshared_ptr_t(x))

#define UPCR_PHASEOF_SHARED(x) \
          upcr_phaseof_shared(ARG_shared_ptr_t(x))

#define UPCR_PHASEOF_PSHARED(x) \
          upcr_phaseof_pshared(ARG_pshared_ptr_t(x))

#define UPCR_ADDRFIELD_SHARED(x) \
         upcr_addrfield_shared(ARG_shared_ptr_t(x))

#define UPCR_ADDRFIELD_PSHARED(x) \
         upcr_addrfield_pshared(ARG_pshared_ptr_t(x))

#define UPCR_SETNULL_SHARED(x) \
         upcr_setnull_shared((upcr_shared_ptr_t*)(x))

#define UPCR_SETNULL_PSHARED(x) \
        upcr_setnull_pshared((upcr_pshared_ptr_t*)(x))

/* BUG?  These macros are casting to the wrong type (shared_ptr, not
 * 'shared_ptr *'.  I'm commenting them out until this gets sorted out
 *	- JCD
#define UPCR_SETNULL_RSHARED(x) \
         upcr_setnull_shared(ARG_shared_ptr_t(x))

#define UPCR_SETNULL_RPSHARED(x) \
        upcr_setnull_pshared(ARG_shared_ptr_t(x))
*/

#define UPCR_ADD_SHARED(x,y,z,w) \
        RET_shared_ptr_t(upcr_add_shared(ARG_shared_ptr_t(x), (y), (z), (w)))

#define UPCR_ADD_PSHAREDI(x,y,z) \
         RET_pshared_ptr_t(upcr_add_psharedI(ARG_pshared_ptr_t(x), (y), (z)))

#define UPCR_ADD_PSHARED1(x,y,z) \
         RET_pshared_ptr_t(upcr_add_pshared1(ARG_pshared_ptr_t(x), (y), (z)))


#define UPCR_ISEQUAL_SHARED_SHARED(x,y) \
         upcr_isequal_shared_shared(ARG_shared_ptr_t(x), ARG_shared_ptr_t(y))

#define UPCR_ISEQUAL_SHARED_PSHARED(x,y) \
         upcr_isequal_shared_pshared(ARG_shared_ptr_t(x), ARG_pshared_ptr_t(y))


#define UPCR_ISEQUAL_PSHARED_PSHARED(x,y) \
         upcr_isequal_pshared_pshared(ARG_pshared_ptr_t(x), ARG_pshared_ptr_t(y))

#define UPCR_SUB_SHARED(x,y,z,w) \
         upcr_sub_shared(ARG_shared_ptr_t(x), ARG_shared_ptr_t(y), (z), (w))

#define UPCR_SUB_PSHAREDI(x,y,z) \
         upcr_sub_psharedI(ARG_pshared_ptr_t(x), ARG_pshared_ptr_t(y), (z))

#define UPCR_SUB_PSHARED1(x,y,z) \
         upcr_sub_pshared1(ARG_pshared_ptr_t(x), ARG_pshared_ptr_t(y), (z))

#define MYTHREAD upcr_mythread()
#ifndef __UPC_STATIC_THREADS__
# define THREADS upcr_threads()
#endif
#define upcr_add_shared_ptr upcr_add_shared
#define upcr_barrier(x,y) do {  \
        int _x = (x), _y = (y); \
        upcr_notify(_x,_y);     \
        upcr_wait(_x,_y);       \
        } while (0)

#define bupc_poll() upcr_poll_nofence()
#define upc_poll()  upcr_poll_nofence()	    /* for backward-compatibility */

#define UPCR_SHARED_SIZE    sizeof(upcr_shared_ptr_t)
#define UPCR_PSHARED_SIZE   sizeof(upcr_pshared_ptr_t)


/* implicit handle synchronization */
#define UPCR_WAIT_SYNCNBI_GETS upcr_wait_syncnbi_gets
#define UPCR_WAIT_SYNCNBI_PUTS upcr_wait_syncnbi_puts
#define UPCR_WAIT_SYNCNBI_ALL upcr_wait_syncnbi_all
#define UPCR_BEGIN_NBI_ACCESSREGION upcr_begin_nbi_accessregion
#define UPCR_END_NBI_ACCESSREGION upcr_end_nbi_accessregion

/* new spelling for inline modifier */
#define UPCRI_INLINE GASNETT_INLINE

/* bug 1109: provide definitions with the upc_* prefix for library-like functions 
 * defined in the runtime spec with a upcr_* prefix. */

#define upc_global_exit         upcr_global_exit

#define upc_global_alloc        upcr_global_alloc
#define upc_all_alloc           upcr_all_alloc
#define upc_alloc(sz)           upcr_local_alloc(1,(sz))
#define upc_local_alloc         upcr_local_alloc
#define upc_free                upcr_free

#define upc_threadof            upcr_threadof_shared
#define upc_phaseof             upcr_phaseof_shared
#define upc_addrfield           upcr_addrfield_shared
#define upc_affinitysize        upcr_affinitysize
#define upc_resetphase          upcr_shared_resetphase
#define upc_global_lock_alloc   upcr_global_lock_alloc
#define upc_all_lock_alloc      upcr_all_lock_alloc
#define upc_lock_free           upcr_lock_free
#define upc_lock                upcr_lock
#define upc_lock_attempt        upcr_lock_attempt
#define upc_unlock              upcr_unlock

#define upc_memcpy              upcr_memcpy
#define upc_memput              upcr_memput
#define upc_memget              upcr_memget
#define upc_memset              upcr_memset

#endif /* UPCR_PROXY_H */
