function res = nargin_test()
    res = addme(1)
    res = res + addme(1, 2)
    res = res + addme()
    res = res + donothing()
end

function c = addme(a,b)
    if nargin == 0 
      c = 0
    elseif nargin == 1
      c = a
    elseif nargin == 2
      c = a+b
    else 
      c = -(a+b)
    end

    c = 0
end

function res = donothing()
    res = 1 + 1
end

