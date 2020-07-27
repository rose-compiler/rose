function belief()
  rounds = 10;
  duration = 0;
  N = 1000000;

  output = zeros(25, 1)

  for i = 1:rounds
    A = randn(25, 25)
    z = randn(25, 1)

    tic
    output = output + beliefprop(A, z, N)
    duration = toc + duration
  end

  disp(output)

  duration = duration / rounds * 1000
  disp(duration)
end

function x = beliefprop(A, x, N)
  for i = 1:N
    x = A * x;
    x = x / sum(x);
  end
end
