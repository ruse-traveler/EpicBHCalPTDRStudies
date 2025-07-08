#!/bin/bash
# =============================================================================
# @file   bhcal-this.sh
# @author Derek Anderson
# @date   07.07.2025
# -----------------------------------------------------------------------------
# Adds path to this directory to your path and ROOT include path.
# Allows you to call associated scripts, macros, etc. from
# wherever.
# =============================================================================

# function to add a path to another w/o duplication
add_this() {

  # function arguments
  local add="$1"
  local name="$2"

  # grab variable value
  eval "base=\${${name}:-}"

  # if directory not yet in path, add
  # add it to path
  if [[ $base != *"$add"* ]]; then
    eval "export $name=$base:$add" 
  fi

}  # end 'add_this()'

# add top directory to include paths
add_this $PWD PATH
add_this $PWD ROOT_INCLUDE_PATH

# add plotting source to library paths
add_this $PWD/plotting/src LD_LIBRARY_PATH
add_this $PWD/plotting/src ROOT_LIBRARY_PATH

# end =========================================================================
