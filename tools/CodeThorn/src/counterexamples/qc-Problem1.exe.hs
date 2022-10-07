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
import Control.Exception hiding (assert)
import Control.Concurrent
import Data.Char
import Data.String.Utils
import Prelude
import System.IO
import System.IO.Error hiding (try)
import System.IO.Unsafe
import System.Process
import Test.QuickCheck
import Test.QuickCheck.Gen
import Test.QuickCheck.Monadic
import Test.QuickCheck.Property
import Text.Printf

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
         | TTTrue
         | FFFalse
         | None deriving (Eq, Show)

data State = StIn Char | StOut Char deriving (Eq, Show)
newtype RersData = RersData [Char] deriving (Eq, Show)
data BoolLattice = FFalse | TTrue | Top

-- Test input data generator
instance Arbitrary RersData where
  arbitrary = sized input
    where input size = do
            --n    <- choose (size `min` 4, maxlength)
            --vals <- vectorOf n (choose ('A','G'))
            --vals <- vectorOf size (frequency frequencies)
            --return (RersData vals)
            vals <- grow (4) []
            return $ RersData $ reverse $ vals

          grow 0 xs = do return xs
          grow size xs = do
            -- unsafePerformIO $ printf "grow %d %s\n" size (show xs)
            x <- frequency frequencies
            let r = unsafePerformIO $ actualOutput $ RersData (reverse (x:xs)) in
              if r == [] then
                if length xs > 3 then do return xs
                  else grow (size-1) (xs)
                else grow (size-1) (x:xs)
          --maxlength = 8

  shrink (RersData vals) = map RersData (shrink' vals)
    where
      shrink' []     = []
      shrink' (x:xs) = [ xs ] ++ [ x:xs' | xs' <- shrink' xs ]

prop_holds :: [(LTL, Int)] -> RersData -> Property
prop_holds formulae input =
  not (input == RersData []) ==> monadicIO (test formulae input)
  where
    test formulae input = do
      states <- run $ actualOutput input
      xs <- run $ hold' formulae states
      return True
    hold' fs ss = do mapM_ (verify ss) fs; return ()

verify :: [State] -> (LTL, Int) -> IO ()
verify ss (f, n) = do
  case holds f ss of
    FFalse -> do
      printf "===================================================\n"
      printf "checking %s\n" (show f)
      printf "%d FALSE, found counterexample\n" n
      return ()
    _ -> do
      return ()


-- execute the actual program to get its output
actualOutput :: RersData -> IO [State]
actualOutput (RersData input) = do
  printf "> ./machine %s (len=%d)\n" input (length input)
  -- unbuffer is part of GNU? expect and forces a command to use
  -- unbuffered I/O by emulating a terminal
  (m_in, m_out, m_err, pid) <- runInteractiveCommand $ "unbuffer -p "++machine
  mapM_ (flip hSetBinaryMode False) [m_in, m_out, m_err]
  hSetBuffering m_in LineBuffering
  hSetBuffering m_out NoBuffering
  hSetBuffering m_err NoBuffering
  -- fork off a thread to read from m_err, so our process doesn't
  -- block if it writes to it
  -- ... unbuffer redirects err&>out
  -- hasErrOutput <- newEmptyMVar
  -- forkIO $ do err <- hGetContents m_err; putMVar hasErrOutput err; return ()
  result <- (try $ action m_in m_out input) :: IO (Either IOException [State])
  try $ mapM_ hClose [m_in, m_out, m_err] :: IO (Either IOException ())
  terminateProcess pid  --err <- takeMVar hasErrOutput
  waitForProcess pid
  case result of
    Left _ -> do
      printf "**I/O Error\n"
      return []
    Right output -> do
      if True then -- ignore traces which wrote to stderr
        do
          prettyprint output
          hFlush stdout -- flush the *console*, so the user gets to watch our progress
          return output
        else do return []


  where inputStr = join "\n" (map show input)
        timeout = 1000 -- milliseconds
        action _ _ [] = do return []
        action m_in m_out (input:is) = do
          hPutStrLn m_in (show (rersInt input))
          hFlush m_in
          --printf "in %s\n" (show (rersInt input))

          -- make a best effort to synchronize input and output. It's
          -- really impossible because a given input may or may not
          -- trigger an output.
          hasOutput <- hWaitForInput m_out timeout
          if hasOutput then
          --if True then
            do reply <- hGetLine m_out
               case (readMaybe reply)::(Maybe Int) of
                 Just i -> do --printf "out %c\n" (rersChar (i::Int))
                              res <- action m_in m_out is
                              return $ (StIn input) : (StOut (rersChar i)) : res
                 _      -> do printf "I/O Error: '%s'\n" reply
                              ioError $ userError "invalid input"
                              --res <- action m_in m_out is
                              --return $ (StIn input) : res
            else do
               printf "no output\n"
               res <- action m_in m_out is
               return $ (StIn input) : res


readMaybe :: (Read a) => String -> Maybe a
readMaybe s = case [x | (x,t) <- reads s, ("","") <- lex t] of
  [x] -> Just x
  _   -> Nothing


prettyprint :: [State] -> IO ()
prettyprint output = do
  putStr " "
  mapM_ pp output
  --printf "\n"
  return ()
    where pp (StIn  c) = do putStr "\ESC[33m"; putStr [c]; putStr "\ESC[39m"; return ()
          pp (StOut c) = do putStr "\ESC[35m"; putStr [c]; putStr "\ESC[39m"; return ()

rersChar :: Int -> Char
rersChar i = chr (i+(ord 'A')-1)
rersInt :: Char -> Int
rersInt c = (ord c) - (ord 'A')+1

toRersData :: [Int] -> RersData
toRersData = RersData . map rersChar

holds :: LTL -> [State] -> BoolLattice
holds (TTTrue)  _ = TTrue
holds (FFFalse) _ = FFalse
holds (In  c) ((StIn  stc):_) = lift (c == stc)
holds (Out c) ((StOut stc):_) = lift (c == stc)
holds (In  c) ((StOut stc):states) = holds (In  c) states
holds (Out c) ((StIn  stc):states) = holds (Out c) states
holds       (X a) (_:states) = holds a states
holds       (F a) states = (holds a states) ||| (holds (X (F a)) states)
holds       (G a) states = (holds a states) &&& (holds (X (G a)) states)
holds     (Not a) states = nnot (holds a states)
holds (a `And` b) states = (holds a states) &&& (holds b states)
holds (a `Or`  b) states = (holds a states) ||| (holds b states)
holds (a `U`   b) states = (holds b states) ||| ((holds a states) &&& (holds (X (a `U` b)) states))
holds (a `R`   b) states = (holds b states) &&& ((holds a states) ||| (holds (X (a `R` b)) states))
holds (a `WU`  b) states = holds ((G a) `Or` (a `U` b)) states
holds _ _ = Top

-- because of lazy evaluation, these should short-circuit
(|||) :: BoolLattice -> BoolLattice -> BoolLattice
(|||) TTrue  _      = TTrue
(|||) _      TTrue  = TTrue
(|||) FFalse FFalse = FFalse
(|||) Top    FFalse = Top
(|||) FFalse Top    = Top
(|||) Top    Top    = Top


lift :: Bool -> BoolLattice
lift True = TTrue
lift False = FFalse

(&&&) :: BoolLattice -> BoolLattice -> BoolLattice
(&&&) FFalse _      = FFalse
(&&&) _      FFalse = FFalse
(&&&) TTrue  TTrue  = TTrue
(&&&) Top    TTrue  = Top
(&&&) TTrue  Top    = Top
(&&&) Top    Top    = Top

nnot Top = Top
nnot TTrue = FFalse
nnot FFalse = TTrue

#include LTL_FILE

-- last element of formulae is always None, ignore it
allbutlast list = take ((length list)-1) list
main = do
  checkAll (zip (allbutlast formulae) [1..])


-- Hard-coded input sequences version:
checkAll :: [(LTL, Int)] -> IO ()
checkAll ltls = do
  is <- readFile inputsequences
  r <- mapM_ check (map (RersData) (lines is))
  return ()
    where check :: RersData -> IO ()
          check input = do
            states <- actualOutput input
            xs <- hold' ltls states
            return ()
          hold' fs ss = do mapM_ (verify ss) fs; return ()

{--
-- QuickCheck version:
quickcheckAll :: [(LTL, Int)] -> IO ()
  --testData <- sample' 13 (arbitrary::(Gen RersData))
  --print testData
  sample (arbitrary::(Gen RersData))
  quickCheckWithResult stdArgs { maxSuccess = 100 --, maxDiscard = 10
                               } (prop_holds ltls)
  return ()
--}
