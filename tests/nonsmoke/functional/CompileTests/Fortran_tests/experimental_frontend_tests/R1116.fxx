!! R1116 submodule
!     is submodule-stmt
!           [ specification-part ]
!           [ module-subprogram-part ]
!        end-submodule-stmt
!
module ancestor
end

SUBMODULE (ancestor) submod1
END SUBMODULE submod1

SUBMODULE (ancestor) submod2
  contains
END SUBMODULE

SUBMODULE (ancestor) submod2
  integer :: i
END SUBMODULE

SUBMODULE (ancestor) submod3
  contains
  subroutine bleu
  end
END

SUBMODULE (ancestor) submod4
  integer i
  contains
  subroutine bleu
  end
END
