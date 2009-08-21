{-| Provides DataMc instance for 'ROSE.Sage3.Classes.SgNode' and subclasses, and type extension functions
    for queries and transformations for the 'ROSE.Sage3.Classes.SgNode' 'Data.DataMc.DataMc' instance. -}
module ROSE.Sage3.SYB (mkMn, mkMqn, extMn, extMqn) where

import Ptr
import Data.Generics
import Data.DataMc
import Control.Monad
import Control.Monad.Trans
import Storable
import MarshalAlloc
import MarshalArray
import CTypes

import ROSE.Sage3.Classes
import ROSE.Sage3.Methods

foreign import ccall "stdlib.h free" cFree :: Ptr a -> IO ()

foreign import ccall "WrapSage3.h _haskell_get_traversal_successor_ptrs" cGetTraversalSuccessorPtrs :: Ptr (SgNode a) -> Ptr (Ptr (Ptr (Ptr ()))) -> Ptr (Ptr CInt) -> IO CSize

getTraversalSuccessorPtrs :: Typeable a => SgNode a -> IO [(Ptr (Ptr ()), VariantT)]
getTraversalSuccessorPtrs n@(SgNode p) = do
	alloca (\ppPtrs -> do
	alloca (\ppVariants -> do
	size <- cGetTraversalSuccessorPtrs p ppPtrs ppVariants
	pPtrs <- peek ppPtrs
	pVariants <- peek ppVariants
	let sizeInt = fromIntegral size
	ptrs <- peekArray sizeInt pPtrs
	variantInts <- peekArray sizeInt pVariants
	cFree pPtrs
	cFree pVariants
	let variants = map variantTFromInt variantInts
	return (zip ptrs variants)))

instance (Typeable a, MonadIO m) => DataMc m SgNodeT a where
	gmapMc f x@(SgNode p) | p == nullPtr = return x
	                      | otherwise    = do
		subnodePtrs <- liftIO (getTraversalSuccessorPtrs x)
--		liftIO (putStrLn (show subnodePtrs))
		forM_ subnodePtrs (\(ppNode, variant) -> do
			pNode <- liftIO (peek ppNode)
			case sageStaticCast pNode variant of
			 SomeSgNode n -> do
				n' <- f n
				let SgNode pNode' = n'
				liftIO (poke ppNode (castPtr pNode')))
		return x
	gmapMqc f x@(SgNode p) | p == nullPtr = return []
	                       | otherwise    = do
		subnodePtrs <- liftIO (getTraversalSuccessorPtrs x)
--		liftIO (putStrLn (show subnodePtrs))
		forM subnodePtrs (\(ppNode, variant) -> do
			pNode <- liftIO (peek ppNode)
			case sageStaticCast pNode variant of
				SomeSgNode n -> f n)

{-| Extends a transformation type extension. -}
extMn :: (MonadIO m, Typeable n, Typeable n', Typeable sn) => (SgNode sn -> m (SgNode sn)) -> (SgNode n -> m (SgNode n')) -> SgNode sn -> m (SgNode sn)
extMn = extMn' undefined undefined undefined
 where
	extMn' :: (MonadIO m, Typeable n, Typeable n', Typeable sn) => SgNode n -> SgNode n' -> SgNode sn -> (SgNode sn -> m (SgNode sn)) -> (SgNode n -> m (SgNode n')) -> SgNode sn -> m (SgNode sn)
	--                                      Are we guaranteed to return a subtype of the field type?
	--                                                         If n and sn are in disjoint paths of the class hierarchy, they are definitely incompatible
	extMn' tv_n tv_n' tv_sn def ext n@(SgNode p) | sv_sn <= sv_n' && (sv_sn <= sv_n || sv_n <= sv_sn) = do
		--             A null pointer may be of any type
		--                             sn is guaranteed to be a subtype
		callSafe <- if p == nullPtr || sv_n <= sv_sn then return True else do
			dv_sn <- liftIO (nodeVariantT n)
			-- Can we cast the dynamic type of the field value to the input value regardless of static type?
			return (sv_n <= dv_sn)
		if callSafe then ext (SgNode (castPtr p)) >>= (\(SgNode p) -> return (SgNode (castPtr p)))
			    else def n
	                                       | otherwise = def n
	 where
		sv_n = staticVariant tv_n
		sv_n' = staticVariant tv_n'
		sv_sn = staticVariant tv_sn

{-| Creates a transformation type extension. -}
mkMn :: (MonadIO m, Typeable n, Typeable n', Typeable sn) => (SgNode n -> m (SgNode n')) -> SgNode sn -> m (SgNode sn)
mkMn = extMn return

{-| Extends a query type extension. -}
extMqn :: (MonadIO m, Typeable n, Typeable sn) => (SgNode sn -> m r) -> (SgNode n -> m r) -> SgNode sn -> m r
extMqn = extMqn' undefined undefined
 where
	extMqn' :: (MonadIO m, Typeable n, Typeable sn) => SgNode n -> SgNode sn -> (SgNode sn -> m r) -> (SgNode n -> m r) -> SgNode sn -> m r
	--                                   If n and sn are in disjoint paths of the class hierarchy, they are definitely incompatible
	extMqn' tv_n tv_sn def ext n@(SgNode p) | sv_sn <= sv_n || sv_n <= sv_sn = do
		--             A null pointer may be of any type
		--                             sn is guaranteed to be a subtype
		callSafe <- if p == nullPtr || sv_n <= sv_sn then return True else do
			nDynVariant <- liftIO (nodeVariantT n)
			-- Can we cast the dynamic type of the field value to the input value regardless of static type?
			return (sv_n <= nDynVariant)
		if callSafe then ext (SgNode (castPtr p))
		            else def n
	                                   | otherwise = def n
	 where
		sv_n = staticVariant tv_n
		sv_sn = staticVariant tv_sn

{-| Creates a query type extension. -}
mkMqn :: (MonadIO m, Typeable n, Typeable sn) => r -> (SgNode n -> m r) -> SgNode sn -> m r
mkMqn r = extMqn (const (return r))

   {-
staticVariant :: (Typeable n) => SgNode n -> VariantT
staticVariant n = staticVariant' (typeOf n)
 where
	staticVariant' :: TypeRep -> VariantT
	staticVariant' tr | arg == typeOf () = read ("V_" ++ ctorSimpleName)
	                  | otherwise = staticVariant' arg
	 where
		[arg] = typeRepArgs tr
		ctorName = tyConString (typeRepTyCon tr)
		-- unqualify the name and remove the final 'T'
		ctorSimpleName = reverse (takeWhile ('.' /=) (tail (reverse ctorName)))
		-}
