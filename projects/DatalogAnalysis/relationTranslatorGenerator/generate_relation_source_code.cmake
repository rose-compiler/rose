set(ENV{PYTHONPATH} ${SOURCE_DIR})
execute_process(
  COMMAND ${PYTHON_EXECUTABLE} ${SOURCE_DIR}/main.py
)
message(WARNING "Here's what I'm running: *${PYTHON_EXECUTABLE} ${SOURCE_DIR}/main.py*")
