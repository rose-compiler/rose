module ROSE.Sage3.Misc (withVariant, sageDynamicCast) where

import ROSE.Sage3.Classes
import ROSE.Sage3.Methods.SgNode
import Data.Typeable

withVariant :: Typeable a => SgNode a -> (VariantT -> b) -> IO b
withVariant n f = do
	vnt <- nodeVariantT n
	return (f vnt)

{-| Performs a dynamic cast of the given 'SgNode' to its true type -}
sageDynamicCast :: Typeable a => SgNode a -> IO SomeSgNode
sageDynamicCast n@(SgNode p) = withVariant n (sageStaticCast p)
