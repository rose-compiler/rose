{-|
The 'DataMc' class is similar to the standard scrap-your-boilerplate
'Data.Data.Data' class, but with two additional constraints:

 1. All data must be constructed using a particular user-defined data type with a single
    arbitrary argument.

 2. The data structure must be traversed using a specific monad.

Other than this, the class behaves very much like the standard SYB design pattern.
-}
module Data.DataMc (DataMc(..), GenericMc, GenericMqc, everywhereMc, everythingMc) where

{- TODO: investigate whether the SYB with class technique can be used to supplant some of this -}

import Data.Typeable
import ROSE.Sage3.Classes

{-| The class itself.  Here m is the monad, c the data type and a the type arguemnt -}
class Typeable a => DataMc m c a where
	{-| Generic mapping function for transformations. -}
	gmapMc :: (forall d. (DataMc m c d) => c d -> m (c d)) -> c a -> m (c a)
	{-| Generic mapping function for queries. -}
	gmapMqc :: (forall d. (DataMc m c d) => c d -> m r) -> c a -> m [r]

{- Generic transformation type -}
type GenericMc m c = forall a. (DataMc m c a) => c a -> m (c a)
{- Generic query type -}
type GenericMqc m r c = forall a. (DataMc m c a) => c a -> m r

{-| Type extension for transformations.  Acts similarly to 'Data.Generics.Schemes.everywhere' -}
everywhereMc :: Monad m => GenericMc m c -> GenericMc m c
everywhereMc f x = do
	x' <- gmapMc (everywhereMc f) x
	f x'

{-| Type extension for queries.  Acts similarly to 'Data.Generics.Schemes.everything' -}
everythingMc :: Monad m => (r -> r -> r) -> GenericMqc m r c -> GenericMqc m r c
everythingMc k f x = do
	rs <- gmapMqc (everythingMc k f) x
	fx <- f x
	return (foldl k fx rs) 
