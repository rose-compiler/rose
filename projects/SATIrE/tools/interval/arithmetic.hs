-- This is a test of the interval arithmetic functions used in interval.optla

import Data.Bits

plus (al,ah) (bl,bh) = (al+bl, ah+bh)
minus (al,ah) (bl,bh) = (al-bh, ah-bl)
times (al,ah) (bl,bh) = (min (min (al*bl) (al*bh)) (min (ah*bl) (ah*bh)),
                         max (max (al*bl) (al*bh)) (max (ah*bl) (ah*bh)))
--divide (al,ah) (bl,bh) = (min (min (al `safediv` bh) (ah `safediv` bh))
--                              (min (al `safediv` bminpos) (ah `safediv` bmaxneg)),
--                          max (max (al `safediv` bminpos) (ah `safediv` bminpos))
--                              (max (al `safediv` bmaxneg) (ah `safediv` bmaxneg))
--                         )
--    where bminpos = if bl > 0 then bl
--                    else max 1 bl
--          bmaxneg = if bh < 0 then bh
--                    else max (-1) bl
divide (al,ah) (bl,bh) = (-x, x)
                         where x = max (max (abs al) (abs bl))
                                       (max (abs ah) (abs bh))
modulo a b@(bl,bh) = let bmax = max (abs bl) (abs bh)
                     in if strict_equal_sign a b then
                            if bl > 0 then (0, bmax -1)
                            else (1-bmax, 0)
                    else (1-bmax, bmax-1)

lshift (al,ah) (bl,bh) = if al<0 || bl<0 then dontknow
                         else (al `shiftL` bl, ah `shiftL` bh)

-- according to ISO C99 right-shift of a negative number is compiler-specific
rshift (al,ah) (bl,bh) = if al<0 || bl<0 then dontknow
                         else (al `shiftR` bh, ah `shiftR` bl)

bitand a@(al,ah) b@(bl,bh) = if (al==0 && ah==0) || (bl==0 && bh==0) 
                             then (0, 0)
                             else bitor a b

bitor a@(al,ah) b@(bl,bh) =  let x = next_higher_power_of_2 
                                       (max (max (abs al) (abs bl))
                                            (max (abs ah) (abs bh)))
                             in if strict_positive a b then
                                    (0, x-1) 
                                else (-x,x-1)

bitxor a@(al,ah) b@(bl,bh) = let amax = max (abs al) (abs ah)
                                 bmax = max (abs bl) (abs bh)
                                 x = next_higher_power_of_2(amax .|. bmax)
                             in if strict_equal_sign a b then
                                    (0, x-1)
                             else (-x,x-1)

dontknow = (1-(2^63), 2^63-1)
-- return True if all values in both intervals have the same sign
strict_equal_sign (al,ah) (bl,bh) = (al > 0 && ah > 0 && bl > 0 && bh > 0) ||
                                    (al < 0 && ah < 0 && bl < 0 && bh < 0)

strict_positive (al,ah) (bl,bh) = (al > 0 && ah > 0 && bl > 0 && bh > 0)
strict_negative (al,ah) (bl,bh) = (al < 0 && ah < 0 && bl < 0 && bh < 0)

lowest_bit(0) = 0
lowest_bit(n) =
    if n == (1 `shiftL` 31) then 32 else  -- the critical negative case
    if (n .&. 1) == 1       then 1
                           else 1 + lowest_bit(n `shiftR` 1)

highest_bit(0) = 0;
highest_bit(n) =
    if n < 0 then 32  -- sign bit is set
             else 1 + highest_bit(n `shiftR` 1)

lowest_power_of_2(0) = 0;
lowest_power_of_2(n) = (1 `shiftL` (lowest_bit(n) - 1))

next_higher_power_of_2(0) = 0;
next_higher_power_of_2(n) = (1 `shiftL` highest_bit(n))


------------------------------------------

safediv a 0 = 0 -- ignore divide by zero
safediv a b = a `div` b
safemod a 0 = 0 -- ignore modide by zero
safemod a b = a `mod` b

test (f,op,name) a@(al,ah) b@(bl,bh) =
    foldl1 (&&) $ 
           map check [(ai,bi, ai `op` bi) | ai <- [al..ah], bi <- [bl..bh]]
    where min = fst (f a b)
          max = snd (f a b)
          check (i,j,x) = if min <= x && max >= x 
                    then True
                    else error $ 
                             "bug! "++(show a)++name++(show b)
                             ++" = "++(show (min,max))
                             ++"\n\tCounterexample: "++(show i)++name++(show j)
                             ++" = "++(show (i `op` j))

r = runtest
runtest = let -- We need to take at least one arbitrary +/- and the
              -- neutral element plus odd and even numbers for `div`.
              -- The rest should follow from monotonicity of the
              -- operators
              vals = [-23,-8,-3,-1,0,1,7,4,17,21,32] 
              tuples = [(i,j) | i <- vals, j <- reverse vals, i<j]
              ops = [(plus,(+),"+"),
                     (minus,(-),"-"),
                     (times,(*),"*"),
                     (divide,(safediv)," `div` "),
                     (modulo,(safemod), " `mod` "),
                     (lshift,shiftL," << "),
                     (rshift,shiftR," >> "),
                     (bitand,(.&.)," & "),
                     (bitor,(.|.)," | "),
                     (bitxor,xor," `xor` ")
                    ]
          in
            foldl1 (&&) [test f a b | f <- ops, a <- tuples, b <- tuples]