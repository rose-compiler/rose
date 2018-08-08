function linearreg
%linear regression
  runs = 10
  result=0;
  sz = 1000;
  b=1:sz;

  for i=1:runs
    for j=1:100
       a=randn(sz,sz);

       tic;
       c = a\b';
       result=result+toc;
     end
  end

  result=result/runs * 1000;
  disp(result);
end
