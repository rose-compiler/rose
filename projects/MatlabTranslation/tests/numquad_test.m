function numquad_test()
  time = 0;
  N = 10 ^ 7;

  for i = 1:10
    tic;
    v = numquad(5, 15, N);
    time = toc + time;

    disp(v);
  end

  time = time * 1000;
  disp(time);
end

function val=numquad(lb,ub,npoints)
  val = 0.0;
  for x=lb : (ub-lb)/npoints : ub
    val = val + sin(x)/npoints;
  end
end
