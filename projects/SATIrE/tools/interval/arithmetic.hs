-- This is a test of the interval arithmetic functions used in interval.optla

plus (al,ah) (bl,bh) = (al+bl, ah+bh)
minus (al,ah) (bl,bh) = (al-bh, ah-bl)
times (al,ah) (bl,bh) = (min (min (al*bl) (al*bh)) (min (ah*bl) (ah*bh)),
                         max (max (al*bl) (al*bh)) (max (ah*bl) (ah*bh)))
--divide (al,ah) (bl,bh) = (min al bl, max ah bh)
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

safediv a 0 = 0 -- ignore divide by zero
safediv a b = a `div` b

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

runtest = let -- We need to take at least one arbitrary +/- and the
              -- neutral element plus odd and even numbers for `div`.
              -- The rest should follow from monotonicity of the
              -- operators
              vals = [-23,-8,-3,-1,0,1,7,4,17] 
              tuples = [(i,j) | i <- vals, j <- reverse vals, i<j]
              ops = [(plus,(+),"+"),
                     (minus,(-),"-"),
                     (times,(*),"*"),
                     (divide,(safediv)," `div` ")]
          in
            foldl1 (&&) [test f a b | f <- ops, a <- tuples, b <- tuples]