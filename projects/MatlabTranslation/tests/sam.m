function res = sam()
  M = 128*128;
  N = 32;
  K = 32;
  A = rand(K,M);
  B = rand(K,N);

  %~ Count total elapsed time
  tic;

  %~ Assume matrices are stored columnwise
  A = A / colnorm_diag(A);
  B = B / colnorm_diag(B);

  %~ Time sgemm only
  C = transpose(B)*A;

  %~ Find maximum value in each column and store in a vector
  res = max(C);
  elapsedTime = toc;
  disp(elapsedTime);
end

function res = colnorm_diag(X)
   X = sqrt(X' * X);
   res = diag(diag(X));
end
