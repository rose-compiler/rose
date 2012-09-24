{-- -*- coding: utf-8 -*-

Copyright (c) 2012 Lawrence Livermore National Security, LLC.
Produced at the Lawrence Livermore National Laboratory
Written by Adrian Prantl <adrian@llnl.gov>.

UCRL-CODE-155962.
All rights reserved.

This file is part of ROSE. For details, see http://www.rosecompiler.org/.
Please read the COPYRIGHT file for Our Notice and for the BSD License.

------------------------------------------------------------------------

This program is supposed to find counterexamples for a list of LTL
formulae by actually executing the state machine (compiled rers
program) with "random" input.

Dependencies:
apt-get install ghc libghc6-missingh-dev libghc6-quickcheck2-dev expect-dev

------------------------------------------------------------------------

--}
module Main where
import Prelude
import System.IO
import System.IO.Error
import System.Process
import Text.Printf
import Test.QuickCheck
import Test.QuickCheck.Monadic
import Data.String.Utils
import Data.Char
import Control.Concurrent

data LTL = In Char
         | Out Char
         | Not LTL
         | X LTL
         | F LTL
         | G LTL
         | And LTL LTL
         | Or LTL LTL
         | U LTL LTL
         | WU LTL LTL
         | R LTL LTL
         | None deriving (Eq, Show)

data State = StIn Char | StOut Char deriving (Eq, Show)
newtype RersData = RersData [Int] deriving (Eq, Show)

-- Test input data generator
instance Arbitrary RersData where
  arbitrary = sized input
    where input size = do
            n    <- choose (size `max` 1, maxlength)
            vals <- vectorOf n (choose (minval, maxval))
            return (RersData vals)
          minval = 1
          maxval = 7
          maxlength = 6
  shrink (RersData vals) = map RersData (shrink vals)

prop_holds :: LTL -> RersData -> Property
prop_holds formula input =
  not (input == RersData []) ==> monadicIO test
  where test = do states <- run $ actualOutput input
                  assert $ holds formula states


-- execute the actual program to get its output
actualOutput :: RersData -> IO [State]
actualOutput (RersData input) = do
  printf "> ./machine %s (len=%d)\n" (map rersChar input) (length input)
  -- unbuffer is part of GNU? expect and forces a command to use
  -- unbuffered I/O by emulating a terminal
  (stdin, stdout, stderr, pid) <- runInteractiveCommand "unbuffer -p ./machine.exe"
  mapM_ (flip hSetBinaryMode False) [stdin, stdout, stderr]
  hSetBuffering stdin LineBuffering
  hSetBuffering stdout NoBuffering
  -- fork off a thread to read from stderr, so our process doesn't
  -- block if it writes to it
  forkIO $ do err <- hGetContents stderr; printf err; return ()
  result <- try $ action stdin stdout input
  case result of 
    Left _ -> do 
      --printf "I/O Error\n"
      terminateProcess pid
      return []
    Right output -> do 
      prettyprint output
      terminateProcess pid
      return output
  
  where inputStr = join "\n" (map show input)
        parse string = map (\s -> (read s)::Int) (split "\n" string)

        action _ _ [] = do return []
        action stdin stdout (input:is) = do 
          hPutStrLn stdin (show input)
          hFlush stdin
          --printf "in %s\n" (show input) 

          -- make a best effort to synchronize input and output. It's
          -- really impossible because a given input may or may not
          -- trigger an output.
          hasOutput <- hWaitForInput stdout 33 -- milliseconds
          if hasOutput then 
            do reply <- hGetLine stdout
               --printf "out %s\n" (reply)
               res <- action stdin stdout is
               return $ (StIn (rersChar input)) : (StOut (rersChar (read reply))) : res
            else do 
               res <- action stdin stdout is
               return $ (StIn (rersChar input)) : res

prettyprint output = do
  printf "> "
  mapM_ pp output
  printf "\n"
  return ()
    where pp (StIn  c) = do printf "\ESC[33m%c\ESC[39m" c; return ()
          pp (StOut c) = do printf "\ESC[35m%c\ESC[39m" c; return ()

rersChar :: Int -> Char
rersChar i = chr (i+(ord 'A')-1)

-- verify that an LTL formula holds for a given chain of states
holds :: LTL -> [State] -> Bool
holds     (In  c) ((StIn  stc):_) = (c == stc)
holds     (Out c) ((StOut stc):_) = (c == stc)
holds     (Not a) states = not (holds a states)
holds       (X a) (_:states) = holds a states
holds       (F a) states = (holds a states) || (holds (X (G a)) states)
holds       (G a) states = (holds a states) && (holds (X (G a)) states)
holds (a `And` b) states = (holds a states) && (holds b states)
holds (a `Or`  b) states = (holds a states) || (holds b states)
holds (a `U`   b) states = (holds b states) || ((holds a states) && (holds (X (a `U` b)) states))
holds (a `R`   b) states = (holds b states) && ((holds a states) || (holds (X (a `R` b)) states))
holds (a `WU`  b) states = (states == []) ||
                           (holds b states) || ((holds a states) && (holds (X (a `U` b)) states))
holds _ [] = True
holds _ _ = False

#include "formulae.hs"
formulae' = [ ( Not (F (Out 'W') )), None]
-- last element of formulae is always None, ignore it
main = do mapM printResult (allbutlast formulae)
            where allbutlast list = take ((length list)-1) list

printResult :: LTL -> IO ()
printResult f = do
  printf "===================================================\n"
  printf "checking %s\n" (show f)
  quickCheck (prop_holds f)
