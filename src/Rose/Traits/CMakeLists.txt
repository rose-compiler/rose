
########### install files ###############

install(FILES ../Traits.h DESTINATION ${INCLUDE_INSTALL_DIR}/Rose)
install(FILES Describe.h Traversal.h DESTINATION ${INCLUDE_INSTALL_DIR}/Rose/Traits)
install(FILES describe/declarations.h describe/predicates.h describe/fields.h describe/nodes.h DESTINATION ${INCLUDE_INSTALL_DIR}/Rose/Traits/describe)
install(FILES grammar/traversal.h grammar/lists.h grammar/predicates.h DESTINATION ${INCLUDE_INSTALL_DIR}/Rose/Traits/grammar)
