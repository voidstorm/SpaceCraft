cmake_minimum_required(VERSION 2.8.3)
include(CMakeParseArguments)

# ------------------------------------------------------------------------------
# Function for adding compile flags to given files.
#
# Usage:
# COFFEE_AddCompileFlags(
#   FLAGS "-DMyValue=1 -O2"
#   FILES file1.cpp file2.cpp)
#
# ------------------------------------------------------------------------------
function(COFFEE_AddCompileFlags)
  set(options)
  set(oneValueArgs FLAGS)
  set(multiValueArgs FILES)

  cmake_parse_arguments(
    ACF "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

  foreach(f ${ACF_FILES})
    get_source_file_property(old_flags ${f} COMPILE_FLAGS)

    if(old_flags)
      set(new_flags "${old_flags} ${ACF_FLAGS}")
    else()
      set(new_flags ${ACF_FLAGS})
    endif()
    set_source_files_properties(
      ${f}
      PROPERTIES
      COMPILE_FLAGS ${new_flags})
  endforeach()

endfunction()

# ------------------------------------------------------------------------------
# Function for selecting files by extension.
#
# Usage:
# COFFEE_SelectFiles(PYTON_AND_ASM_FILE
#   FILES a.cpp b.cpp c.asm d.asm e.py
#   EXTENSIONS py asm)
#
# PYTON_AND_ASM_FILE will be set to "c.asm d.asm e.py".
# ------------------------------------------------------------------------------
function(COFFEE_SelectFiles result)
  set(options)
  set(oneValueArgs)
  set(multiValueArgs FILES EXTENSIONS)

  cmake_parse_arguments(
    SF "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

  set(extensions)
  foreach(e ${SF_EXTENSIONS})
    # Add a leading dot "." to the extensions.
    list(APPEND extensions ".${e}")
  endforeach()

  set(list_of_files)
  foreach(f ${SF_FILES})
    get_filename_component(ext ${f} EXT)
    list(FIND extensions ${ext} n)
    if(NOT(${n} LESS 0))
      list(APPEND list_of_files ${f})
    endif()
  endforeach()

  set(${result} ${list_of_files} PARENT_SCOPE)

endfunction()
