# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles\\et3400_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\et3400_autogen.dir\\ParseCache.txt"
  "et3400_autogen"
  )
endif()
