function matrix_access()
  sz =  2 ^ 14;
  rounds = 1

  time = 0;
  for i = 1:rounds
    M = randn(sz, sz);
    acc = 0;

    tic;
    for r = 1:sz
      for c = 1:sz
         acc = acc + M(r, c);
      end
    end

    time = toc + time;
  end

  time = time / rounds * 1000;
  disp(acc);
  disp(time);
  %disp(['Total Time:' num2str(time) 'ms'])
end
