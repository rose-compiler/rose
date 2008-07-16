     TYPE CELL
        INTEGER :: VAL
        TYPE (CELL), POINTER :: NEXT_CELL
     END TYPE CELL

!     is defined and objects such as HEAD and CURRENT are declared using

     TYPE (CELL), TARGET :: HEAD
     TYPE (CELL), POINTER :: CURRENT

!     If a linked list has been created and attached to HEAD and the pointer CURRENT has been allocated
!     space, statements such as

     CURRENT = HEAD
     CURRENT = CURRENT % NEXT_CELL

!     cause the contents of the cells referenced on the right to be copied to the cell referred to by CURRENT.
!     In particular, the right-hand side of the second statement causes the pointer component in the cell,
!     CURRENT, to be selected. This pointer is dereferenced because it is in an expression context to produce
!     the target's integer value and a pointer to a cell that is in the target's NEXT CELL component. The
!     left-hand side causes the pointer CURRENT to be dereferenced to produce its present target, namely
!     space to hold a cell (an integer and a cell pointer). The integer value on the right is copied to the integer
!     space on the left and the pointer component is pointer assigned (the descriptor on the right is copied
!     into the space for a descriptor on the left). When a statement such as

     CURRENT => CURRENT % NEXT_CELL

!     is executed, the descriptor value in CURRENT % NEXT CELL is copied to the descriptor named
!     CURRENT. In this case, CURRENT is made to point at a different target.
!     In the intrinsic assignment statement, the space associated with the current pointer does not change but
!     the values stored in that space do. In the pointer assignment, the current pointer is made to associate
!     with different space. Using the intrinsic assignment causes a linked list of cells to be moved up through
!     the current "window"; the pointer assignment causes the current pointer to be moved down through the
!     list of cells.

end