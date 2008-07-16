! R617 array-section is data-ref [ (substring-range) ]
a = array(1:N:2,M)
a = char_array (:,:,:) (2:3)
! NOTE 6.11
ARRAY_A(1:N:2)%ARRAY_B(I, J)%STRING(K)(:) = 1
SCALAR_PARENT%ARRAY_FIELD(1:N) = 1
A = SCALAR_PARENT%ARRAY_FIELD(1:N)%SCALAR_FIELD
end
