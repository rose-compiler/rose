#!/bin/echo ERROR: Source, do not execute, this script:
# Define functions to push and pop the current state of set.  
# Only for one level deep.

push_set_state () {
  # Using set +u in a subshell so that detection of an unset variable does not 
  # itself trigger an error:
  if [ `(set +u; echo ${SET_STATE-not_set})` != "not_set" ]
  then
    echo "push_set_state: ERROR - SET_STATE should not be set, but is: \"${SET_STATE}\""
    return 1
  else  
    export SET_STATE=$-
  fi
}

pop_set_state () {
  # Using set +u in a subshell so that detection of an unset variable does not 
  # itself trigger an error:
  if [ `(set +u; echo ${SET_STATE-not_set})` == "not_set" ]
  then
    echo "pop_set_state: ERROR - SET_STATE should be set, but is not."
    return 1
  else
    set -${SET_STATE}
    unset SET_STATE
  fi
}

test_push_pop_u () {
  echo $-
  push_set_state
  set +u
  echo $-
  pop_set_state
  echo $-
}

test_two_pushes () {
  push_set_state
  push_set_state
}

test_two_pops () {
  pop_set_state
  pop_set_state
}

test_push_pop_u
test_two_pushes
test_two_pops

