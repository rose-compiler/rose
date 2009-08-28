module Main where

import Data.Maybe
import Control.Monad
import System
import Data.DataMc
import ROSE
import ROSE.Sage3
import Time

simplifyAddOp :: SgAddOp () -> IO (SgExpression ())
simplifyAddOp = simplify (+)

simplifySubtractOp :: SgSubtractOp () -> IO (SgExpression ())
simplifySubtractOp = simplify (-)

simplifyMultiplyOp :: SgMultiplyOp () -> IO (SgExpression ())
simplifyMultiplyOp = simplify (*)

simplifyDivideOp :: SgDivideOp () -> IO (SgExpression ())
simplifyDivideOp = simplify div

simplify op n | n == nullSgNode = return (upSgExpression n)
              | otherwise = do                               
  lhs <- binaryOpGetLhsOperand n              
  rhs <- binaryOpGetRhsOperand n              
  lhsInt <- isSgIntVal lhs                    
  rhsInt <- isSgIntVal rhs
  if isJust lhsInt && isJust rhsInt then do
    lhsVal <- intValGetValue (fromJust lhsInt)
    rhsVal <- intValGetValue (fromJust rhsInt)
    let sum = lhsVal `op` rhsVal
    fi <- sgNullFile
    liftM upSgExpression (newIntVal fi sum (show sum))
   else
    return (upSgExpression n)

main :: IO ()
main = do
  time1 <- getClockTime
  prj <- frontend =<< getArgs
  time2 <- getClockTime
  putStrLn ("Frontend took " ++ show (diffClockTimes time2 time1))
  everywhereMc (mkMn simplifyAddOp `extMn` simplifySubtractOp
        `extMn` simplifyMultiplyOp `extMn` simplifyDivideOp) prj
  time3 <- getClockTime
  putStrLn ("Traversal took " ++ show (diffClockTimes time3 time2))
  exitWith =<< backend prj
