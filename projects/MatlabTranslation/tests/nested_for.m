function nested_for ( )
  run ( 10, 10 )
end

function x = run ( m, n )
  x = 0

  for i = 1 : m
    for j = 1 : n
      x = x + i*j
    end
  end
end


