{-| This module provides an interface to ROSE's frontend and backend -}

module ROSE (frontend, backend) where

import System
import Ptr
import CTypes
import CString
import MarshalArray
import MarshalAlloc
import Data.Typeable

import ROSE.Sage3

foreign import ccall "haskellWrappers.h _haskell_frontend" cFrontend :: CInt -> Ptr CString -> IO (Ptr (SgProject ()))
foreign import ccall "haskellWrappers.h _haskell_backend" cBackend :: Ptr (SgProject a) -> IO CInt

{-| Calls the ROSE frontend with the given command line arguments and returns an 'SgProject' object representing the resultant IR. -}
frontend :: [String] -> IO (SgProject ())
frontend args = do
	progName <- getProgName
	csArgs <- mapM newCString (progName : args)
	csArgArray <- newArray csArgs
	cProject <- cFrontend (fromIntegral (length csArgs)) csArgArray
	free csArgArray
	mapM_ free csArgs
	return (SgNode cProject)

{-| Calls the ROSE backend.  The ROSE backend uses the given IR to produce unparsed source code files, invokes the system compiler on these source code files and returns its return code. -}
backend :: Typeable a => SgProject a -> IO ExitCode
backend (SgNode prj) = do
	backResult <- cBackend prj
	if backResult == 0 then
		return ExitSuccess
	 else
		return (ExitFailure (fromIntegral backResult))
