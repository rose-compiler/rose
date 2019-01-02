function mcmc_test()
  round = 10;
  time = 0;
  mm = time ^ 2;
  N = 10000000;

  for i = 1:round
    x = randn(2, 2);
    tic;
    p = mcmc(x, N);
    time = toc + time;
    disp(p);
  end

  time = time / round * 1000;
  disp(['Time' num2str(time) 'ms']);
end

function p_ret = mcmc(x,N)
  p = fx(x);

  for n=1:N
    [r, c] = size(x);
    x2 = x + .01 * randn(r, c);
    p2 = fx(x2);

    r = rand();

    if r < p2/p
      x = x2;
      p = p2;
    end
  end

  p_ret = p;
end

function y = fx(x)
  a = x(1)^2;
  b = x(2)^2;
  %a = 0;
  %b = 0;
  y = exp(sin(x(1) * 5) - a - b);
end
