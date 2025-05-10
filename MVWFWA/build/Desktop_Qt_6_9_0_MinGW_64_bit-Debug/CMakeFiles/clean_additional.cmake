# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles\\MVWFWA_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\MVWFWA_autogen.dir\\ParseCache.txt"
  "MVWFWA_autogen"
  )
endif()
