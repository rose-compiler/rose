function [product, sum] =  apply(n1, n2)
  product = n1 * n2;
  sum = n1 + n2;
  my_matrix = n1(1, :);
end

function function2()
  m1 = [ 1.5, 2.6, 3; 4, 5, 6];
  m2 = [ 2, 4 ,5];

[p, s] = apply(m1, m2);
x = doSth();
end
