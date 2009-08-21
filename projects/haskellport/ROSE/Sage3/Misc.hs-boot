module ROSE.Sage3.Misc (withVariant) where

import ROSE.Sage3.Classes
import Data.Typeable

withVariant :: Typeable a => SgNode a -> (VariantT -> b) -> IO b
