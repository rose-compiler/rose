module m_constructor_init

  TYPE T
  END TYPE

  TYPE(t):: x = T()

end module
