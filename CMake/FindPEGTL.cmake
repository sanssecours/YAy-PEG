# Try to find PEGTL
# =================

# This find script defines the following variables:
#
# * `PEGTL_FOUND`:         System has PEGTL
# * `PEGTL_INCLUDE_DIRS`:  The PEGTL include directories
#
# .

find_path(PEGTL_INCLUDE_DIR NAMES tao/pegtl.hpp PATH_SUFFIXES tao)

include(FindPackageHandleStandardArgs)
# Handle the QUIETLY and REQUIRED arguments and set PEGTL_FOUND to TRUE, if all
# listed variables are TRUE
find_package_handle_standard_args(PEGTL REQUIRED_VARS PEGTL_INCLUDE_DIR)

mark_as_advanced(PEGTL_INCLUDE_DIR)

set(PEGTL_INCLUDE_DIRS ${PEGTL_INCLUDE_DIR})
