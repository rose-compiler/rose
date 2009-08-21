module ROSE.CxxContainers (CxxList, CxxVector, newContainer, getElems, delete) where

import Ptr
import MarshalAlloc
import MarshalArray
import CTypes
import Storable
import Data.Typeable

data CxxList et = CxxList (Ptr (CxxList et)) deriving (Typeable)
data CxxVector et = CxxVector (Ptr (CxxList et)) deriving (Typeable)

foreign import ccall "stdlib.h free" cFree :: Ptr a -> IO ()

class CxxContainer ctr et where
	cNewContainer :: Ptr et -> CSize -> IO (Ptr (ctr et))
	cGetElems :: Ptr (ctr et) -> Ptr CSize -> IO (Ptr et)
	cDelete ::  Ptr (ctr et) -> IO ()

foreign import ccall "WrapCxxContainers.h _haskell_new_ptr_list" cNewPtrList :: Ptr (Ptr et) -> CSize -> IO (Ptr (CxxList (Ptr et)))
foreign import ccall "WrapCxxContainers.h _haskell_get_ptr_list_elems" cGetPtrListElems :: Ptr (CxxList (Ptr et)) -> Ptr CSize -> IO (Ptr (Ptr et))
foreign import ccall "WrapCxxContainers.h _haskell_delete_ptr_list" cDeletePtrList :: Ptr (CxxList (Ptr et)) -> IO ()

instance CxxContainer CxxList (Ptr et) where
	cNewContainer = cNewPtrList
	cGetElems = cGetPtrListElems
	cDelete = cDeletePtrList

foreign import ccall "WrapCxxContainers.h _haskell_new_ptr_vector" cNewPtrVector :: Ptr (Ptr et) -> CSize -> IO (Ptr (CxxVector (Ptr et)))
foreign import ccall "WrapCxxContainers.h _haskell_get_ptr_vector_elems" cGetPtrVectorElems :: Ptr (CxxVector (Ptr et)) -> Ptr CSize -> IO (Ptr (Ptr et))
foreign import ccall "WrapCxxContainers.h _haskell_delete_ptr_vector" cDeletePtrVector :: Ptr (CxxVector (Ptr et)) -> IO ()

instance CxxContainer CxxVector (Ptr et) where
	cNewContainer = cNewPtrVector
	cGetElems = cGetPtrVectorElems
	cDelete = cDeletePtrVector

foreign import ccall "WrapCxxContainers.h _haskell_new_int_vector" cNewIntVector :: Ptr CInt -> CSize -> IO (Ptr (CxxVector CInt))
foreign import ccall "WrapCxxContainers.h _haskell_get_int_vector_elems" cGetIntVectorElems :: Ptr (CxxVector CInt) -> Ptr CSize -> IO (Ptr CInt)
foreign import ccall "WrapCxxContainers.h _haskell_delete_int_vector" cDeleteIntVector :: Ptr (CxxVector CInt) -> IO ()

instance CxxContainer CxxVector CInt where
	cNewContainer = cNewIntVector
	cGetElems = cGetIntVectorElems
	cDelete = cDeleteIntVector

foreign import ccall "WrapCxxContainers.h _haskell_new_ulong_vector" cNewULongVector :: Ptr CULong -> CSize -> IO (Ptr (CxxVector CULong))
foreign import ccall "WrapCxxContainers.h _haskell_get_ulong_vector_elems" cGetULongVectorElems :: Ptr (CxxVector CULong) -> Ptr CSize -> IO (Ptr CULong)
foreign import ccall "WrapCxxContainers.h _haskell_delete_ulong_vector" cDeleteULongVector :: Ptr (CxxVector CULong) -> IO ()

instance CxxContainer CxxVector CULong where
	cNewContainer = cNewULongVector
	cGetElems = cGetULongVectorElems
	cDelete = cDeleteULongVector

foreign import ccall "WrapCxxContainers.h _haskell_new_ull_vector" cNewULLongVector :: Ptr CULLong -> CSize -> IO (Ptr (CxxVector CULLong))
foreign import ccall "WrapCxxContainers.h _haskell_get_ull_vector_elems" cGetULLongVectorElems :: Ptr (CxxVector CULLong) -> Ptr CSize -> IO (Ptr CULLong)
foreign import ccall "WrapCxxContainers.h _haskell_delete_ull_vector" cDeleteULLongVector :: Ptr (CxxVector CULLong) -> IO ()

instance CxxContainer CxxVector CULLong where
	cNewContainer = cNewULLongVector
	cGetElems = cGetULLongVectorElems
	cDelete = cDeleteULLongVector

foreign import ccall "WrapCxxContainers.h _haskell_new_uchar_vector" cNewUCharVector :: Ptr CUChar -> CSize -> IO (Ptr (CxxVector CUChar))
foreign import ccall "WrapCxxContainers.h _haskell_get_uchar_vector_elems" cGetUCharVectorElems :: Ptr (CxxVector CUChar) -> Ptr CSize -> IO (Ptr CUChar)
foreign import ccall "WrapCxxContainers.h _haskell_delete_uchar_vector" cDeleteUCharVector :: Ptr (CxxVector CUChar) -> IO ()

instance CxxContainer CxxVector CUChar where
	cNewContainer = cNewUCharVector
	cGetElems = cGetUCharVectorElems
	cDelete = cDeleteUCharVector

foreign import ccall "WrapCxxContainers.h _haskell_new_uint_vector" cNewUIntVector :: Ptr CUInt -> CSize -> IO (Ptr (CxxVector CUInt))
foreign import ccall "WrapCxxContainers.h _haskell_get_uint_vector_elems" cGetUIntVectorElems :: Ptr (CxxVector CUInt) -> Ptr CSize -> IO (Ptr CUInt)
foreign import ccall "WrapCxxContainers.h _haskell_delete_uint_vector" cDeleteUIntVector :: Ptr (CxxVector CUInt) -> IO ()

instance CxxContainer CxxVector CUInt where
	cNewContainer = cNewUIntVector
	cGetElems = cGetUIntVectorElems
	cDelete = cDeleteUIntVector

newContainer elems = withArray elems (\pElems -> cNewContainer pElems (fromIntegral (length elems)))

getElems pCtr = do
	alloca (\pSize -> do
	pElems <- cGetElems pCtr pSize
	size <- peek pSize
	elems <- peekArray (fromIntegral size) pElems
	cFree pElems
	return elems)

delete :: CxxContainer ctr et => Ptr (ctr et) -> IO ()
delete = cDelete
