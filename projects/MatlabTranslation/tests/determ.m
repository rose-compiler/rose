function determ()
  runs = 10
  result = 0;
  total = 0

  for i = 1:runs
    for j = 1:100
      a = randn(1500,1500);
      tic;
      b = det(a);
      result = result+toc;
      total = total + b;
    end
  end
  result=result/runs * 1000;
  disp(total)
end
